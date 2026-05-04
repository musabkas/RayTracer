#include "DOFSampler.hpp"
#include <random>
#include <cmath>

// FIXED: Base Sampler only takes 2 arguments. num_samples is initialized separately.
DOFSampler::DOFSampler(Camera *c_ptr, ViewPlane *v_ptr, int samples, float f_dist, float l_radius)
    : Sampler(c_ptr, v_ptr), num_samples(samples), focal_distance(f_dist), lens_radius(l_radius) {}

DOFSampler::DOFSampler(const DOFSampler& other) 
    : Sampler(other), num_samples(other.num_samples), focal_distance(other.focal_distance), lens_radius(other.lens_radius) {}

DOFSampler& DOFSampler::operator=(const DOFSampler& other) {
    Sampler::operator=(other);
    num_samples = other.num_samples;
    focal_distance = other.focal_distance;
    lens_radius = other.lens_radius;
    return *this;
}

std::vector<Ray> DOFSampler::get_rays(int px, int py) const {
    std::vector<Ray> rays;

    float pixel_width = (viewplane_ptr->bottom_right.x - viewplane_ptr->top_left.x) / viewplane_ptr->hres;
    float pixel_height = (viewplane_ptr->top_left.y - viewplane_ptr->bottom_right.y) / viewplane_ptr->vres;
    int samples_per_side = static_cast<int>(std::sqrt(num_samples));
    float weight = 1.0f / num_samples;

    thread_local std::random_device rd;
    thread_local std::mt19937 generator(rd());
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

    Perspective* persp_cam = dynamic_cast<Perspective*>(camera_ptr);

    for (int i = 0; i < samples_per_side; i++) {
        for (int j = 0; j < samples_per_side; j++) {
            float sub_x = static_cast<float>(i) / samples_per_side;
            float sub_y = static_cast<float>(j) / samples_per_side;
            float jitter_x = distribution(generator) / samples_per_side;
            float jitter_y = distribution(generator) / samples_per_side;

            float offset_x = sub_x + jitter_x;
            float offset_y = sub_y + jitter_y;

            float x = viewplane_ptr->top_left.x + (px + offset_x) * pixel_width;
            float y = viewplane_ptr->top_left.y - (py + offset_y) * pixel_height;
            Point3D vp_pos = Point3D(x, y, viewplane_ptr->top_left.z);

            Vector3D base_dir = camera_ptr->get_direction(vp_pos);

            // FIXED: Using get_pos() instead of trying to access protected 'pos' directly
            Point3D focal_point = persp_cam->get_pos() + base_dir * focal_distance;

            float r = lens_radius * std::sqrt(distribution(generator));
            float theta = 2.0f * M_PI * distribution(generator);
            float dx = r * std::cos(theta);
            float dy = r * std::sin(theta);

            // FIXED: Using get_pos() instead of 'pos'
            Point3D lens_origin = persp_cam->get_pos() + Vector3D(dx, dy, 0.0f);

            Vector3D dof_dir = focal_point - lens_origin;
            dof_dir.normalize();

            Ray ray(lens_origin, dof_dir);
            ray.w = weight;
            rays.push_back(ray);
        }
    }

    return rays;
}
#include "Jittered.hpp"
#include "../cameras/Camera.hpp"
#include "../world/ViewPlane.hpp"
#include "../utilities/Ray.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath>

Jittered::Jittered(Camera *c_ptr, ViewPlane *v_ptr, int samples) 
  : Sampler(c_ptr, v_ptr), num_samples(samples) {
  // Seed the random number generator
  static bool seeded = false;
  if (!seeded) {
    srand(static_cast<unsigned>(time(0)));
    seeded = true;
  }
}

Jittered::Jittered(const Jittered& other) : Sampler(other), num_samples(other.num_samples) {
}

Jittered &Jittered::operator=(const Jittered& other) {
  Sampler::operator=(other);
  num_samples = other.num_samples;
  return *this;
}

std::vector<Ray> Jittered::get_rays(int px, int py) const {
  std::vector<Ray> rays;
  
  // Calculate pixel dimensions in world space
  float pixel_width = (viewplane_ptr->bottom_right.x - viewplane_ptr->top_left.x) / viewplane_ptr->hres;
  float pixel_height = (viewplane_ptr->bottom_right.y - viewplane_ptr->top_left.y) / viewplane_ptr->vres;
  
  // Calculate number of samples per row and column (sqrt of total samples)
  int samples_per_side = static_cast<int>(sqrt(num_samples));
  
  // Weight for each ray (normalized by number of samples)
  float weight = 1.0f / num_samples;
  
  // Generate jittered samples
  for (int i = 0; i < samples_per_side; i++) {
    for (int j = 0; j < samples_per_side; j++) {
      // Calculate base position for this sub-pixel
      float sub_x = static_cast<float>(i) / samples_per_side;
      float sub_y = static_cast<float>(j) / samples_per_side;
      
      // Add random jitter within sub-pixel
      float jitter_x = static_cast<float>(rand()) / RAND_MAX / samples_per_side;
      float jitter_y = static_cast<float>(rand()) / RAND_MAX / samples_per_side;
      
      // Calculate final position within pixel
      float offset_x = sub_x + jitter_x;
      float offset_y = sub_y + jitter_y;
      
      // Calculate world coordinates
      float x = viewplane_ptr->top_left.x + (px + offset_x) * pixel_width;
      float y = viewplane_ptr->top_left.y + (py + offset_y) * pixel_height;
      
      Point3D pos = Point3D(x, y, viewplane_ptr->top_left.z);
      Vector3D dir = camera_ptr->get_direction(pos);
      
      Ray ray(pos, dir);
      ray.w = weight;
      rays.push_back(ray);
    }
  }
  
  return rays;
}

#include "Diffuse.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../lights/Light.hpp"
#include "../world/World.hpp"
#include "../tracers/Tracer.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <random> // Required for thread-safe randoms

Diffuse::Diffuse(){
    lambertian_brdf = Lambertian();
}
Diffuse::Diffuse(float c){
    lambertian_brdf = Lambertian(c);
}
Diffuse::Diffuse(float r, float g, float b){
    lambertian_brdf = Lambertian(r, g, b);
}
Diffuse::Diffuse(const RGBColor &c){
    lambertian_brdf = Lambertian(c);
}

Diffuse::Diffuse(const Diffuse &other) {
    lambertian_brdf = other.lambertian_brdf;
}
Diffuse &Diffuse::operator=(const Diffuse &other){
    lambertian_brdf = other.lambertian_brdf;
    return *this;
}

RGBColor Diffuse::shade(const ShadeInfo &sinfo, const std::vector<Light *> &lights) const {
    RGBColor color = RGBColor();
    for (Light* light_ptr : lights) {
        Vector3D dir = light_ptr->get_direction(sinfo.hit_point);
        color += lambertian_brdf.f(light_ptr->get_direction(sinfo.hit_point), sinfo.ray.d, sinfo.normal) * light_ptr->L(dir) * std::max(sinfo.normal * dir, 0.0);
    }
    color.clamp();
    return color;
}

RGBColor Diffuse::path_shade(ShadeInfo& sr) {
    if (sr.depth > 10)
        return RGBColor(0, 0, 0);
    RGBColor L_direct(0, 0, 0);

    // ==========================================
    // 1. DIRECT LIGHTING (Next Event Estimation)
    // ==========================================
    // Explicitly sample all lights in the scene so we don't rely on blind luck
    for (Light* light_ptr : sr.w->lights) {
        Vector3D wi_direct = light_ptr->get_direction(sr.hit_point);
        float ndotwi = sr.normal * wi_direct;
        
        if (ndotwi > 0.0f) {
            // Shoot a shadow ray directly into the world's BVH
            // Use larger offset to avoid self-intersection with the surface we're leaving from
            Ray shadow_ray(sr.hit_point + sr.normal * 0.001f, wi_direct);
            ShadeInfo shadow_sr = sr.w->hit_objects(shadow_ray);

            // If it hits an object, and that object is closer than the light source, it's blocked
            bool in_shadow = false;
            if (shadow_sr.hit && shadow_sr.t < light_ptr->get_distance(sr.hit_point)) {
                in_shadow = true;
            }
            
            if (!in_shadow) {
                L_direct += lambertian_brdf.f(wi_direct, sr.ray.d, sr.normal) * light_ptr->L(wi_direct) * ndotwi;
            }
        }
    }

    // ==========================================
    // 2. INDIRECT LIGHTING (The Random Bounce)
    // ==========================================
    Vector3D w = sr.normal;
    Vector3D v = Vector3D(0.0034f, 1.0f, 0.0071f) ^ w; 
    v.normalize();
    Vector3D u = v ^ w;

    // THREAD-SAFE RANDOM GENERATOR
    // 'thread_local' ensures every CPU core gets its own isolated generator
    thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

    float r1 = distribution(generator);
    float r2 = distribution(generator);
    float phi = 2.0f * 3.14159265359f * r1;
    
    float r = std::sqrt(r2);
    float x = r * std::cos(phi);
    float y = r * std::sin(phi);
    float z = std::sqrt(std::max(0.0f, 1.0f - r2));

    Vector3D wi = x * u + y * v + z * w;
    wi.normalize();

    // Russian Roulette
    float survival_prob = 0.8f;
    if (distribution(generator) > survival_prob) {
        // If the ray is killed, return ONLY the direct light we already calculated
        return L_direct; 
    }

    // The Recursive Bounce
    Ray bounce_ray(sr.hit_point + sr.normal * 0.001f, wi); 
    // only change is the bounce ray call at the bottom:
    RGBColor L_indirect = sr.w->tracer_ptr->trace_ray(bounce_ray, *(sr.w), sr.depth + 1);

    RGBColor f = lambertian_brdf.f(wi, sr.ray.d, sr.normal);
    return L_direct + ((L_indirect * f) / survival_prob);
}
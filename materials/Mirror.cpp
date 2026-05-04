#include "Mirror.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../lights/Light.hpp"
#include "../world/World.hpp"
#include "../tracers/Tracer.hpp"
#include <random>

Mirror::Mirror(){
    specular_brdf = Specular();
}

Mirror::Mirror(float c){
    specular_brdf = Specular(c);
}

Mirror::Mirror(float r, float g, float b){
    specular_brdf = Specular(r, g, b);
}

Mirror::Mirror(const RGBColor &c, float kr){
    specular_brdf = Specular(c, kr);
}

Mirror::Mirror(const Mirror &other) {
    specular_brdf = other.specular_brdf;
}
Mirror &Mirror::operator=(const Mirror &other){
    specular_brdf = other.specular_brdf;
    return *this;
}

RGBColor Mirror::shade(const ShadeInfo &sinfo, const std::vector<Light *> &lights) const {
    RGBColor color = RGBColor();
    for (Light* light_ptr : lights) {
        Vector3D dir = light_ptr->get_direction(sinfo.hit_point);
        color += specular_brdf.f(sinfo.ray.d, light_ptr->get_direction(sinfo.hit_point), sinfo.normal) * light_ptr->L(dir) * std::max(sinfo.normal * dir, 0.0);
    }
    color.clamp();
    return color;
}

// RGBColor Mirror::path_shade(ShadeInfo& sr) {
//     if (sr.depth > 5)
//         return RGBColor(0, 0, 0);

//     Vector3D wo = -sr.ray.d;
//     wo.normalize();

//     Vector3D reflected = -wo + 2.0f * (wo * sr.normal) * sr.normal;
//     reflected.normalize();

//     // Perturb the reflection based on roughness (0 = mirror, 1 = very blurry)
//     float roughness = 0.3f; // tweak this

//     thread_local std::mt19937 generator(std::random_device{}());
//     std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

//     Vector3D perturb(dist(generator), dist(generator), dist(generator));
//     perturb.normalize();

//     Vector3D blurred = reflected + roughness * perturb;
//     blurred.normalize();

//     // If perturbation pushes ray below surface, fall back to perfect reflection
//     if ((blurred * sr.normal) < 0.0f)
//         blurred = reflected;

//     Ray reflected_ray(sr.hit_point + sr.normal * 0.001f, blurred);
//     RGBColor incoming = sr.w->tracer_ptr->trace_ray(reflected_ray, *(sr.w), sr.depth + 1);

//     return specular_brdf.get_color() * incoming;
// }
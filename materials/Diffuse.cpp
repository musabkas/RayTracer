#include "Diffuse.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../lights/Light.hpp"

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

RGBColor Diffuse::shade(const ShadeInfo &sinfo, std::vector<Light *> &lights) const {
    RGBColor color = RGBColor();
    for (Light* light_ptr : lights) {
        color += lambertian_brdf.f(light_ptr->get_direction(sinfo.hit_point), sinfo.ray.d, sinfo.normal) * light_ptr->L() * std::max(sinfo.normal * light_ptr->get_direction(sinfo.hit_point), 0.0);
    }
    color.clamp();
    return color;
}
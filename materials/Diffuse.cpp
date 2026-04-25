#include "Diffuse.hpp"
#include "../utilities/ShadeInfo.hpp"

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

RGBColor Diffuse::shade(const ShadeInfo &sinfo) const {
    return lambertian_brdf.f(sinfo.ray.d, sinfo.ray.d, sinfo.normal) * (sinfo.normal * -sinfo.ray.d); // TO DO, NEED TO PROVIDE LIGHT DIRECTION
}
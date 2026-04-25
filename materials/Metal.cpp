#include "Metal.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../lights/Light.hpp"

Metal::Metal(){
    glossy_brdf = Glossy();
}

Metal::Metal(float c){
    glossy_brdf = Glossy(c);
}

Metal::Metal(float r, float g, float b){
    glossy_brdf = Glossy(r, g, b);
}

Metal::Metal(const RGBColor &c, float ks, float e){
    glossy_brdf = Glossy(c, ks, e);
}

Metal::Metal(const Metal &other) {
    glossy_brdf = other.glossy_brdf;
}
Metal &Metal::operator=(const Metal &other){
    glossy_brdf = other.glossy_brdf;
    return *this;
}

RGBColor Metal::shade(const ShadeInfo &sinfo, std::vector<Light *> &lights) const {
    RGBColor color = RGBColor();
    for (Light* light_ptr : lights) {
        color += glossy_brdf.f(sinfo.ray.d, light_ptr->get_direction(sinfo.hit_point), sinfo.normal) * light_ptr->L();
    }
    color.clamp();
    return color;
}
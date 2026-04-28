#include "Cosine.hpp"
#include "../utilities/ShadeInfo.hpp"

Cosine::Cosine(){
    color = RGBColor();
}
Cosine::Cosine(float c){
    color = RGBColor(c);
}
Cosine::Cosine(float r, float g, float b){
    color = RGBColor(r, g, b);
}
Cosine::Cosine(const RGBColor &c){
    color = c;
}

Cosine::Cosine(const Cosine &other) {
    color = other.color;
}
Cosine &Cosine::operator=(const Cosine &other){
    color = other.color;
    return *this;
}

RGBColor Cosine::shade(const ShadeInfo &sinfo, const std::vector<Light *> &lights) const {
    return color * (sinfo.normal * -sinfo.ray.d);
}
#pragma once
#include "Material.hpp"

class Emissive : public Material {
    RGBColor color;
    float intensity;
public:
    Emissive(const RGBColor& c, float i) : color(c), intensity(i) {}
    virtual Material* clone() const { return new Emissive(*this); }
    virtual RGBColor shade(const ShadeInfo& sinfo, const std::vector<Light*>& lights) const override {
        return color * intensity;
    }
    virtual RGBColor path_shade(ShadeInfo& sr) override {
        return color * intensity;
    }
};
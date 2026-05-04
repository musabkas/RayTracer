#pragma once

#include "Material.hpp"
#include "../utilities/RGBColor.hpp"

class WavyMirror : public Material {
public:
    RGBColor tint_;
    float frequency_; // How tightly packed the waves are
    float amplitude_; // How violently distorted the reflection gets

    WavyMirror(const RGBColor& tint = RGBColor(1.0f, 1.0f, 1.0f), 
               float freq = 2.0f, 
               float amp = 0.5f);

    virtual RGBColor shade(const ShadeInfo& sinfo, const std::vector<Light*>& lights) const override;
    virtual RGBColor path_shade(ShadeInfo& sr) override;
};
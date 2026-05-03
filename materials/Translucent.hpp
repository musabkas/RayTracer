#pragma once

#include "Material.hpp"
#include "../utilities/RGBColor.hpp"

/**
 * Tinted glass / translucent dielectric for path tracing:
 * probabilistic Fresnel split between reflection and transmission (Schlick +
 * Snell), with tinted multiplier on transmitted light.
 *
 * Regular shade() uses a simple tinted Lambert response so non-path tracers still show something.
 */
class Translucent : public Material {
  RGBColor tint_;
  float ior_;
  float diffuse_frac_;
  float roughness_;

public:
  Translucent(const RGBColor &tint = RGBColor(0.82f, 0.92f, 1.0f), float ior = 1.45f,
              float diffuse_frac = 0.06f, float roughness = 0.0f);

  virtual RGBColor shade(const ShadeInfo &sinfo, const std::vector<Light *> &lights) const override;
  virtual RGBColor path_shade(ShadeInfo &sr) override;
};

#pragma once

#include "Material.hpp"
#include "../BRDF/Lambertian.hpp"
#include "../BRDF/Glossy.hpp"

class SpecularDiffuse : public Material {
protected:
  Lambertian lambertian_brdf;
  Glossy glossy_brdf;
  float kd;
  float ks;

public:
  SpecularDiffuse();
  SpecularDiffuse(const RGBColor &color, float kd = 0.75f, float ks = 0.25f, float e = 32.f);
  SpecularDiffuse(const SpecularDiffuse &other);
  SpecularDiffuse &operator=(const SpecularDiffuse &other);

  virtual RGBColor shade(const ShadeInfo &sinfo, const std::vector<Light *> &lights) const override;
  virtual RGBColor path_shade(ShadeInfo& sr) override;
};

#pragma once

#include "Material.hpp"
#include "../BRDF/Glossy.hpp"

class Metal : public Material {
protected:
  Glossy glossy_brdf; // the color of the material.

public:
  // Constructors.
  Metal();                          // set color to (0, 0, 0).
  Metal(float c);                   // set color to (c, c, c).
  Metal(float r, float g, float b); // set color to (r, g, b).
  Metal(const RGBColor &c, float ks, float e);         // set color to c, ks = ks, e = e

  // Copy constuctor and assignment operator.
  Metal(const Metal &other);
  Metal &operator=(const Metal &other);

  // Desctructor.
  virtual ~Metal() = default;

  /* Returned shade is glossy */
  virtual RGBColor shade(const ShadeInfo &sinfo, std::vector<Light *> &lights) const override;
};

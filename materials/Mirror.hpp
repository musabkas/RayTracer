#pragma once

#include "Material.hpp"
#include "../BRDF/Specular.hpp"

class Mirror : public Material {
protected:
  Specular specular_brdf; // the color of the material.

public:
  // Constructors.
  Mirror();                          // set color to (0, 0, 0).
  Mirror(float c);                   // set color to (c, c, c).
  Mirror(float r, float g, float b); // set color to (r, g, b).
  Mirror(const RGBColor &c, float kr);         // set color to c, kr = kr

  // Copy constuctor and assignment operator.
  Mirror(const Mirror &other);
  Mirror &operator=(const Mirror &other);

  // Desctructor.
  virtual ~Mirror() = default;

  /* Returned shade is glossy */
  virtual RGBColor shade(const ShadeInfo &sinfo, const std::vector<Light *> &lights) const override;

  virtual RGBColor path_shade(ShadeInfo& sinfo) override;
};

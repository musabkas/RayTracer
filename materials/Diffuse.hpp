#pragma once

#include "Material.hpp"
#include "../BRDF/Lambertian.hpp"

class Diffuse : public Material {
protected:
  Lambertian lambertian_brdf; // the color of the material.

public:
  // Constructors.
  Diffuse();                          // set color to (0, 0, 0).
  Diffuse(float c);                   // set color to (c, c, c).
  Diffuse(float r, float g, float b); // set color to (r, g, b).
  Diffuse(const RGBColor &c);         // set color to c.

  // Copy constuctor and assignment operator.
  Diffuse(const Diffuse &other);
  Diffuse &operator=(const Diffuse &other);

  // Desctructor.
  virtual ~Diffuse() = default;

  /* Returned shade is: color * cos \theta.
     \theta is the angle between the normal at the hit pont and the ray.
     Assuming unit vectors, cos \theta = dot product of normal and -ray.dir.
  */
  virtual RGBColor shade(const ShadeInfo &sinfo, const std::vector<Light *> &lights) const override;
};

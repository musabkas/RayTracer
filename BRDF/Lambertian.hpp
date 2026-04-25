#pragma once

#include "BRDF.hpp"
#include "../utilities/RGBColor.hpp"

class Lambertian : public BRDF {
public:
  // Constructors.
  Lambertian();                          // set albedo to (0, 0, 0).
  Lambertian(float c);                   // set albedo to (c, c, c).
  Lambertian(float r, float g, float b); // set albedo to (r, g, b).
  Lambertian(const RGBColor &c);         // set albedo to c.

  // Copy constuctor and assignment operator.
  Lambertian(const Lambertian &other);
  Lambertian &operator=(const Lambertian &other);

  // Desctructor.
  virtual ~Lambertian() = default;

  /* Returned shade is: rho / pi -> but we exclude pi as it is a constant */
  virtual RGBColor f(const Vector3D& wi, const Vector3D& wo, const Vector3D& n) const override;
};

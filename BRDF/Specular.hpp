#pragma once

#include "BRDF.hpp"
#include "../utilities/RGBColor.hpp"

class Specular : public BRDF {
protected:
float kr;

public:
  // Constructors.
  Specular();                          // set albedo to (0, 0, 0), ks = 0
  Specular(float c);                   // set albedo to (c, c, c), ks = 1
  Specular(float r, float g, float b); // set albedo to (r, g, b), ks = 1
  Specular(const RGBColor &c, float ks);         // set albedo to c, ks = ks

  // Copy constuctor and assignment operator.
  Specular(const Specular &other);
  Specular &operator=(const Specular &other);

  // Desctructor.
  virtual ~Specular() = default;
  
  RGBColor get_color() const { return rho; }

  /* Returned shade is: rho / pi */
  virtual RGBColor f(const Vector3D& wi, const Vector3D& wo, const Vector3D& n) const override;
};

#pragma once

#include "BRDF.hpp"
#include "../utilities/RGBColor.hpp"

class Glossy : public BRDF {
protected:
float ks;
float e;

public:
  // Constructors.
  Glossy();                          // set albedo to (0, 0, 0), ks = 0, e = 0.
  Glossy(float c);                   // set albedo to (c, c, c), ks = 1, e = 1.
  Glossy(float r, float g, float b); // set albedo to (r, g, b), ks = 1, e = 1.
  Glossy(const RGBColor &c, float ks, float e);         // set albedo to c, ks = ks, e = e

  // Copy constuctor and assignment operator.
  Glossy(const Glossy &other);
  Glossy &operator=(const Glossy &other);

  // Desctructor.
  virtual ~Glossy() = default;

  /* Returned shade is: rho / pi */
  virtual RGBColor f(const Vector3D& wi, const Vector3D& wo, const Vector3D& n) const override;
};

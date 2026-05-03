#pragma once

/**
   This file declares the base Tracer class which is responsible for
   ray tracing through the scene.
*/

#include "../utilities/RGBColor.hpp"
#include "../utilities/Ray.hpp"

class World;
class ShadeInfo;

class Tracer {
public:
  // Constructor and destructor
  Tracer() = default;
  virtual ~Tracer() = default;

  // Pure virtual function for tracing rays
  virtual RGBColor trace_ray(const Ray &ray, const World &world) const = 0;
  virtual RGBColor trace_ray(const Ray& ray, const World& world, int depth) const = 0;
};

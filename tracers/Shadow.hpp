#pragma once

/**
   This file declares the Shadow tracer class which implements ray tracing
   with shadow rays to properly handle light occlusion.
*/

#include "Tracer.hpp"

class World;
class Ray;

class Shadow : public Tracer {
public:
  // Constructor and destructor
  Shadow() = default;
  ~Shadow() override = default;

  // Trace a single ray through the world, accounting for shadows
  RGBColor trace_ray(const Ray &ray, const World &world) const override;
  RGBColor trace_ray(const Ray &ray, const World &world, int depth) const override;
};

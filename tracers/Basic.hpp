#pragma once

/**
   This file declares the Basic tracer class which implements simple
   primary ray tracing without shadow rays.
*/

#include "Tracer.hpp"

class World;
class Ray;

class Basic : public Tracer {
public:
  // Constructor and destructor
  Basic() = default;
  ~Basic() override = default;

  // Trace a single ray through the world
  RGBColor trace_ray(const Ray &ray, const World &world) const override;
  RGBColor trace_ray(const Ray &ray, const World &world, int depth) const override;
};

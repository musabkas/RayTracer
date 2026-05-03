#pragma once
#include "Tracer.hpp"

class PathTrace : public Tracer {
public:
    PathTrace() = default;
    virtual ~PathTrace() = default;
    
    virtual RGBColor trace_ray(const Ray& ray, const World& world) const;
    virtual RGBColor trace_ray(const Ray& ray, const World& world, int depth) const;
};
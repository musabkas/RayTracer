#pragma once
#include "Tracer.hpp"

class PathTrace : public Tracer {
protected:
    int max_depth;
public:
    PathTrace() : max_depth(5) {}
    PathTrace(int depth) : max_depth(depth) {}
    virtual ~PathTrace() = default;
    
    virtual RGBColor trace_ray(const Ray& ray, const World& world) const;
    virtual RGBColor trace_ray(const Ray& ray, const World& world, int depth) const;
};
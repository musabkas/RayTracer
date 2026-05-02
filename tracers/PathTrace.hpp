#pragma once
#include "Tracer.hpp"

class PathTrace : public Tracer {
public:
    PathTrace() = default;
    virtual ~PathTrace() = default;
    
    // Perfectly matched to your Tracer.hpp signature
    virtual RGBColor trace_ray(const Ray& ray, const World& world) const override; 
};
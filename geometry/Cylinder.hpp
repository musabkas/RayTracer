#pragma once
#include "Geometry.hpp"
#include "../utilities/BBox.hpp"

class Cylinder : public Geometry {
public:
    float y0, y1;   // Bottom and top Y coordinates
    float radius;
    float center_x; // X position
    float center_z; // Z position

    Cylinder(float _y0, float _y1, float _r, float _cx, float _cz);
    
    virtual bool hit(const Ray& ray, double& tmin, ShadeInfo& sr) const override;
    virtual bool shadow_hit(const Ray& ray, float& tmin) const override;
    virtual BBox get_bounding_box() const override;
};
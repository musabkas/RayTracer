#pragma once

#include "Light.hpp"
#include "../utilities/Point3D.hpp"
#include "../utilities/Vector3D.hpp"
#include "../utilities/RGBColor.hpp"

class RectangularLight : public Light {
private:
    Point3D corner;         
    Vector3D v1, v2;        
    Vector3D normal;          
    RGBColor color;         
    float radiance;         

    // We must use 'mutable' here because these change during 'const' function calls
    mutable Point3D sample_point;
    mutable Vector3D wi;

public:
    RectangularLight(const Point3D& c, const Vector3D& vector1, const Vector3D& vector2, const RGBColor& col, float rad);
    
    // Matched perfectly to your Light.hpp virtual functions
    virtual Vector3D get_direction(const Point3D& p) const override;
    virtual RGBColor L(const Vector3D& d) const override;
    virtual double get_distance(const Point3D& hit_point) const override;
};
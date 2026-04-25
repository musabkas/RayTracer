#pragma once

#include "../utilities/RGBColor.hpp"
#include "../utilities/Vector3D.hpp"

class Point3D;

class Light {
protected:
    float ls; // light intensity
    RGBColor cl; // light color

public:
    Light() = default;

    Light(const Light &other) = default;
    Light &operator=(const Light &other) = default;
    virtual ~Light() = default;

    virtual Vector3D get_direction(const Point3D& p) const = 0;
    virtual RGBColor L() const = 0;
};
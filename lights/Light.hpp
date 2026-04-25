#pragma once

class Vector3D;
class RGBColor;

class Light {
protected:
    float ls; // light intensity
    float cl; // light color

public:
    virtual Vector3D get_direction() const = 0;
    virtual RGBColor L() const = 0;
};

#pragma once
#include "../utilities/RGBColor.hpp"

class Vector3D;

class BRDF {
protected:
    RGBColor rho; // albedo    

public:
    virtual RGBColor f(const Vector3D& wi, const Vector3D& wo, const Vector3D& n) const = 0;
};

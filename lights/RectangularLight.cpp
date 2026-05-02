#include "RectangularLight.hpp"
#include <cstdlib>
#include <cmath>

RectangularLight::RectangularLight(const Point3D& c, const Vector3D& vector1, const Vector3D& vector2, const RGBColor& col, float rad) 
    : corner(c), v1(vector1), v2(vector2), color(col), radiance(rad) {
    normal = v1 ^ v2; 
    normal.normalize();
}

Vector3D RectangularLight::get_direction(const Point3D& p) const {
    float r1 = (float)std::rand() / RAND_MAX;
    float r2 = (float)std::rand() / RAND_MAX;
    
    sample_point = corner + (v1 * r1) + (v2 * r2);
    wi = sample_point - p;
    wi.normalize();
    
    return wi;
}

RGBColor RectangularLight::L(const Vector3D& d) const {
    // Note: We use 'd' (the incoming parameter) instead of the internal 'wi' here
    // to match standard architecture, though they should be the same vector.
    float ndotd = -normal * d; 
    if (ndotd > 0.0) {
        return color * radiance;
    } else {
        return RGBColor(0, 0, 0); 
    }
}

// Added the missing get_distance function
double RectangularLight::get_distance(const Point3D& hit_point) const {
    return hit_point.distance(sample_point);
}
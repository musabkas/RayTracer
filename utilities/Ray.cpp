#include "Ray.hpp"

Ray::Ray() {
    o = Point3D();
    d = Vector3D();
    w = 1;
    inv_d = Vector3D(1e8, 1e8, 1e8);
}

Ray::Ray(const Point3D &origin, const Vector3D &dir){
    o = origin;
    d = dir;
    w = 1;
    inv_d = Vector3D(1.0/d.x, 1.0/d.y, 1.0/d.z);
}

std::string Ray::to_string() const {
    return o.to_string() + ", " + d.to_string() + std::to_string(w);
}
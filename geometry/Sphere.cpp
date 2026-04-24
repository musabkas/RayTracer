#include "Sphere.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/BBox.hpp"
#include <math.h>

Sphere::Sphere(){
    c = Point3D();
    r = 0;
}

Sphere::Sphere(const Point3D &center, float radius) {
    c = center;
    r = radius;
}

Sphere::Sphere(const Sphere &object){
    c = object.c;
    r = object.r;
}
Sphere &Sphere::operator=(const Sphere &rhs){
    c = rhs.c;
    r = rhs.r;
    return *this;
}

std::string Sphere::to_string() const {
    return c.to_string() + ", " + std::to_string(r);
}

bool Sphere::hit(const Ray &ray, float &t, ShadeInfo &s) const {
    float A = ray.d * ray.d;
    float B = 2.0 * (ray.d * (ray.o - c));
    float C = (ray.o - c) * (ray.o - c) - r * r;
    float disc = B * B - 4.0 * A * C;

    if (disc < 0) {
        return false;
    }

    float root_disc = sqrt(disc);
    float tnew;

    // smallest root
    tnew = (-B - root_disc) / (2.0 * A);

    if (tnew < 0.0) {
        // try larger root
        tnew = (-B + root_disc) / (2.0 * A);
    }

    if (tnew >= 0.0 && tnew < t) {
        t = tnew;
        s.hit = true;
        s.material_ptr = material_ptr;
        s.hit_point = ray.o + ray.d * t;
        s.normal = (s.hit_point - c);
        s.normal.normalize();
        s.ray = ray;
        s.t = t;
        return true;
    }

    return false;
}

BBox Sphere::getBBox() const { // unimplemented optimization
    return BBox();
}

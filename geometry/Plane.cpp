#include "Plane.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/BBox.hpp"

Plane::Plane(){
    a = Point3D();
    n = Vector3D(0, 1, 0);
}

Plane::Plane(const Point3D &pt, const Vector3D &n){
    this->a = pt;
    this->n = n;
    this->n.normalize();
}

Plane::Plane(const Plane &object){
    a = object.a;
    n = object.n;
}

Plane &Plane::operator=(const Plane &rhs) {
    a = rhs.a;
    n = rhs.n;
    return *this;
}

std::string Plane::to_string() const {
    return a.to_string() + ", " + n.to_string();
}


bool Plane::hit(const Ray &ray, float &t, ShadeInfo &s) const {
    float tnew = (a - ray.o) * n / (ray.d * n);
    if (tnew >= kEpsilon && tnew < t) {
        t = tnew;
        s.hit = true;
        s.material_ptr = material_ptr;
        s.hit_point = ray.o + ray.d * t;
        s.normal = n;
        s.ray = ray;
        s.depth = 0;
        s.t = t;
        return true;
    }
    return false;
}

BBox Plane::getBBox() const { // unimplemented optimization
    // Planes are infinite, so we use a very large bounding box
    // This represents the plane's extent for BVH acceleration purposes
    const float LARGE_EXTENT = 1e6f;
    return BBox(Point3D(-LARGE_EXTENT, -LARGE_EXTENT, -LARGE_EXTENT), 
                Point3D(LARGE_EXTENT, LARGE_EXTENT, LARGE_EXTENT));
}
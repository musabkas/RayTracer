#include "Triangle.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/BBox.hpp"

Triangle::Triangle() {
    v0 = Point3D();
    v1 = Point3D();
    v2 = Point3D();
    normal = Vector3D();
}

Triangle::Triangle(const Point3D &pt0, const Point3D &pt1, const Point3D &pt2) {
    v0 = pt0;
    v1 = pt1;
    v2 = pt2;
    normal = (v1 - v0) ^ (v2 - v0);
    normal.normalize();
}

Triangle::Triangle(const Triangle &object) {
    v0 = object.v0;
    v1 = object.v1;
    v2 = object.v2;
    normal = object.normal;
}

Triangle &Triangle::operator=(const Triangle &rhs) {
    v0 = rhs.v0;
    v1 = rhs.v1;
    v2 = rhs.v2;
    normal = rhs.normal;
    return *this;
}

std::string Triangle::to_string() const {
    return v0.to_string() + ", " + v1.to_string() + ", " + v2.to_string();
}

bool Triangle::hit(const Ray &ray, float &t, ShadeInfo &s) const {
    float a = v0.x - v1.x;
    float b = v0.x - v2.x;
    float c = ray.d.x;
    float d = v0.x - ray.o.x;

    float e = v0.y - v1.y;
    float f = v0.y - v2.y;
    float g = ray.d.y;
    float h = v0.y - ray.o.y;

    float i = v0.z - v1.z;
    float j = v0.z - v2.z;
    float k = ray.d.z;
    float l = v0.z - ray.o.z;

    float den = a*(f*k - g*j) + b*(g*i - e*k) + c*(e*j - f*i);
    
    float beta = (d*(f*k - g*j) + b*(g*l - h*k) + c*(h*j - f*l)) / den;
    float gamma = (a*(h*k - g*l) + d*(g*i - e*k) + c*(e*l - h*i)) / den;
    float tnew = (a*(f*l - h*j) + d*(h*i - e*l) + c*(e*j - f*i)) / den;

    if (tnew >= 0 && tnew < t && 0 <= beta && beta <= 1 && 0 <= gamma && gamma <= 1 && beta + gamma <= 1) {
        t = tnew;
        s.hit = true;
        s.material_ptr = material_ptr;
        s.hit_point = ray.o + ray.d * t;
        s.normal = normal;
        s.ray = ray;
        s.depth = 0;
        s.t = t;
        return true;
    }
    return false;
}


BBox Triangle::getBBox() const {
    float min_x = std::min(std::min(v0.x, v1.x), v2.x);
    float min_y = std::min(std::min(v0.y, v1.y), v2.y);
    float min_z = std::min(std::min(v0.z, v1.z), v2.z);
    
    float max_x = std::max(std::max(v0.x, v1.x), v2.x);
    float max_y = std::max(std::max(v0.y, v1.y), v2.y);
    float max_z = std::max(std::max(v0.z, v1.z), v2.z);
    
    // Add padding to prevent zero-thickness bounding boxes
    float epsilon = 0.0001f;
    if (max_x - min_x < epsilon) { min_x -= epsilon; max_x += epsilon; }
    if (max_y - min_y < epsilon) { min_y -= epsilon; max_y += epsilon; }
    if (max_z - min_z < epsilon) { min_z -= epsilon; max_z += epsilon; }
    
    return BBox(Point3D(min_x, min_y, min_z), Point3D(max_x, max_y, max_z));
}
#include "Perspective.hpp"
#include "../utilities/Vector3D.hpp"

Perspective::Perspective() {
    pos = Point3D();
}

Perspective::Perspective(float c) {
    pos = Point3D(c);
}

Perspective::Perspective(float x, float y, float z) {
    pos = Point3D(x, y, z);
}

Perspective::Perspective(const Point3D &pt) {
    pos = pt;
}

Perspective::Perspective(const Perspective &camera) {
    pos = camera.pos;
}

Perspective &Perspective::operator=(const Perspective &other) {
    pos = other.pos;
    return *this;
}

Vector3D Perspective::get_direction(const Point3D &p) const {
    Vector3D dir = p - pos;
    dir.normalize();
    return dir;
}

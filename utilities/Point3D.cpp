#include "Point3D.hpp"
#include "Vector3D.hpp"

Point3D::Point3D(){
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Point3D::Point3D(float c) {
    this->x = c;
    this->y = c;
    this->z = c;
}

Point3D::Point3D(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

std::string Point3D::to_string() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
}

Point3D Point3D::operator-() const {
    return Point3D(-this->x, -this->y, -this->z);
}

Vector3D Point3D::operator-(const Point3D &p) const {
    return Vector3D(x - p.x, y - p.y, z - p.z);
}

Point3D Point3D::operator+(const Vector3D &v) const {
    return Point3D(x + v.x, y + v.y, z + v.z);
}

Point3D Point3D::operator-(const Vector3D &v) const {
    return Point3D(x - v.x, y - v.y, z - v.z);
}

Point3D Point3D::operator*(const float s) const {
    return Point3D(s * x, s * y, s * z);
}

float Point3D::d_squared(const Point3D &p) const {
    return (*this - p).len_squared();
}

float Point3D::distance(const Point3D &p) const {
    return (*this - p).length();
}

Point3D operator*(const float a, const Point3D &pt){
    return pt * a;
}

// Unsure?
Point3D min(const Point3D& a, const Point3D& b) {
    return Point3D(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

Point3D max(const Point3D& a, const Point3D& b) {
    return Point3D(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

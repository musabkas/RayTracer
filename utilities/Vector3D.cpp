#include "Vector3D.hpp"
#include "Point3D.hpp"

Vector3D::Vector3D() { // set vector to (0, 0, 0).
    this->x = 0;
    this->y = 0;
    this->z = 0;
}                   

Vector3D::Vector3D(double c) { // set vector to (c, c, c).
    this->x = c;
    this->y = c;
    this->z = c;
}                   

Vector3D::Vector3D(double _x, double _y, double _z) { // set vector to (_x, _y, _z).
    this->x = _x;
    this->y = _y;
    this->z = _z;
} 

Vector3D::Vector3D(const Point3D &p) {
    this->x = p.x;
    this->y = p.y;
    this->z = p.z;
}

std::string Vector3D::to_string() const {
    return "(" + std::to_string(this->x) + ", " + std::to_string(this->y) + ", " + std::to_string(this->z) + ")";
}

Vector3D Vector3D::operator-() const {
    return Vector3D(-x, -y, -z);
}

Vector3D Vector3D::operator+(const Vector3D &v) const {
    return Vector3D(x + v.x, y + v.y, z + v.z);
}

Vector3D &Vector3D::operator+=(const Vector3D &v){
    this->x += v.x;
    this->y += v.y;
    this->z += v.z;
    return *this;
}

Vector3D Vector3D::operator-(const Vector3D &v) const {
    return Vector3D(x - v.x, y - v.y, z - v.z);
}

Vector3D &Vector3D::operator-=(const Vector3D &v){
    this->x -= v.x;
    this->y -= v.y;
    this->z -= v.z;
    return *this;
}

Vector3D Vector3D::operator*(const double a) const {
    return Vector3D(x * a, y * a, z * a);
}

Vector3D Vector3D::operator/(const double a) const {
    return Vector3D(x / a, y / a, z / a);
}

void Vector3D::normalize(){
    *this = *this / length();
}

double Vector3D::len_squared() const{
    return x*x + y*y + z*z;
}

double Vector3D::length() const{
    return sqrt(len_squared());
}

double Vector3D::operator*(const Vector3D &b) const {
    return x * b.x + y * b.y + z * b.z;
}

Vector3D Vector3D::operator^(const Vector3D &v) const {
    return Vector3D(y * v.z - z * v.y,
                   z * v.x - x * v.z,
                   x * v.y - y * v.x);
}

Vector3D operator*(const double a, const Vector3D &v){
    return v * a;
}
#include "PointLight.hpp"
#include "../utilities/RGBColor.hpp"

PointLight::PointLight(){
    ls = 1.0;
    cl = RGBColor(1.0);
    pos = Point3D();
}

PointLight::PointLight(float ls, RGBColor cl, Point3D pos) {
    this->ls = ls;
    this->cl = cl;
    this->pos = pos;
}
    
PointLight::PointLight(const PointLight &other){
    ls = other.ls;
    cl = other.cl;
    pos = other.pos;
}

PointLight &PointLight::operator=(const PointLight &other){
    ls = other.ls;    
    cl = other.cl;    
    pos = other.pos;
    return *this;
}

Vector3D PointLight::get_direction(const Point3D& p) const {
    Vector3D dir = pos - p;
    dir.normalize();
    return dir;
}

RGBColor PointLight::L(const Vector3D& d) const {
    return ls * cl;
}
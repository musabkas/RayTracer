#include "SpotLight.hpp"
#include "../utilities/RGBColor.hpp"

SpotLight::SpotLight(){
    ls = 1.0;
    cl = RGBColor(1.0);
    pos = Point3D();
    dir = Vector3D(0, 0, 1);
    theta = 20;
}

SpotLight::SpotLight(float ls, RGBColor cl, Point3D pos, Vector3D dir, float theta) {
    this->ls = ls;
    this->cl = cl;
    this->pos = pos;
    this->dir = dir;
    this->dir.normalize();
    this->theta = theta * M_PI / 180.0;
}
    
SpotLight::SpotLight(const SpotLight &other){
    ls = other.ls;
    cl = other.cl;
    pos = other.pos;
    dir = other.dir;
    theta = other.theta;
}

SpotLight &SpotLight::operator=(const SpotLight &other){
    ls = other.ls;    
    cl = other.cl;    
    pos = other.pos;
    dir = other.dir;
    theta = other.theta;
    return *this;
}

Vector3D SpotLight::get_direction(const Point3D& p) const {
    Vector3D d = pos - p;
    d.normalize();
    return d;
}

RGBColor SpotLight::L(const Vector3D& d) const { // needs to depend on theta?
    if (std::acos(dir * (-d)) < theta){
        return ls * cl;
    } else {
        return RGBColor();
    }
}
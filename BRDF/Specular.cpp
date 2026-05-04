#include "Specular.hpp"
#include "../utilities/ShadeInfo.hpp"
#include <cmath>
#include <iostream>

Specular::Specular(){
    rho = RGBColor();
    kr = 0;
}
Specular::Specular(float c){
    rho = RGBColor(c);
    kr = 1;
}
Specular::Specular(float r, float g, float b){
    rho = RGBColor(r, g, b);
    kr = 1;
}
Specular::Specular(const RGBColor &c, float ks){
    rho = c;
    this->kr = ks;
}

Specular::Specular(const Specular &other) {
    rho = other.rho;
    kr = other.kr;
}
Specular &Specular::operator=(const Specular &other){
    rho = other.rho;
    kr = other.kr;
    return *this;
}

RGBColor Specular::f(const Vector3D& wi, const Vector3D& wo, const Vector3D& n) const {
    Vector3D r = wi - 2*n*(n * wi);
    if ((wo - r).length() < 0.1){
        return (kr * rho) / (n * -wi);
    } else {
        return RGBColor();
    }
}
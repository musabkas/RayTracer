#include "Glossy.hpp"
#include "../utilities/ShadeInfo.hpp"

Glossy::Glossy(){
    rho = RGBColor();
    ks = 0;
    e = 0;
}
Glossy::Glossy(float c){
    rho = RGBColor(c);
    ks = 1;
    e = 1;
}
Glossy::Glossy(float r, float g, float b){
    rho = RGBColor(r, g, b);
    ks = 1;
    e = 1;
}
Glossy::Glossy(const RGBColor &c, float ks, float e){
    rho = c;
    this->ks = ks;
    this->e = e;
}

Glossy::Glossy(const Glossy &other) {
    rho = other.rho;
    ks = other.ks;
    e = other.e;
}
Glossy &Glossy::operator=(const Glossy &other){
    rho = other.rho;
    ks = other.ks;
    e = other.e;
    return *this;
}

RGBColor Glossy::f(const Vector3D& wi, const Vector3D& wo, const Vector3D& n) const {
    Vector3D r = wi - 2*n*(n * wi);
    return ks * rho * pow(std::max(r * wo, 0.0), e);
}
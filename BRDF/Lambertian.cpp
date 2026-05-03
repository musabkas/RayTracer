#include "Lambertian.hpp"
#include "../utilities/ShadeInfo.hpp"

Lambertian::Lambertian(){
    rho = RGBColor();
}
Lambertian::Lambertian(float c){
    rho = RGBColor(c);
}
Lambertian::Lambertian(float r, float g, float b){
    rho = RGBColor(r, g, b);
}
Lambertian::Lambertian(const RGBColor &c){
    rho = c;
}

Lambertian::Lambertian(const Lambertian &other) {
    rho = other.rho;
}
Lambertian &Lambertian::operator=(const Lambertian &other){
    rho = other.rho;
    return *this;
}

RGBColor Lambertian::f(const Vector3D& wi, const Vector3D& wo, const Vector3D& n) const {
    return rho / 3.14159265359f; // Lambertian BRDF: rho/pi
}
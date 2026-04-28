#include "RGBColor.hpp"
#include <cmath>

RGBColor::RGBColor(){
    this->r = 0;
    this->g = 0;
    this->b = 0;
}

RGBColor::RGBColor(float c) {
    this->r = c;
    this->g = c;
    this->b = c;
}

RGBColor::RGBColor(float _r, float _g, float _b) {
    this->r = _r;
    this->g = _g;
    this->b = _b;
}

std::string RGBColor::to_string() const {
    return "(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ")";
}

RGBColor RGBColor::operator+(const RGBColor &c) const {
    return RGBColor(r + c.r, g + c.g, b + c.b);
}

RGBColor &RGBColor::operator+=(const RGBColor &c) {
    this->r += c.r;
    this->g += c.g;
    this->b += c.b;
    return *this;
}

RGBColor RGBColor::operator*(const float a) const {
    return RGBColor(r * a, g * a, b * a);
}

RGBColor &RGBColor::operator*=(const float a) {
    this->r *= a;
    this->g *= a;
    this->b *= a;
    return *this;
}

RGBColor RGBColor::operator/(const float a) const {
    return RGBColor(r / a, g / a, b / a);
}

RGBColor &RGBColor::operator/=(const float a) {
    this->r /= a;
    this->g /= a;
    this->b /= a;
    return *this;
}

RGBColor RGBColor::operator*(const RGBColor &c) const {
    return RGBColor(r * c.r, g * c.g, b * c.b);
}

bool RGBColor::operator==(const RGBColor &c) const {
    return (r == c.r && g == c.g && b == c.b);
}

RGBColor RGBColor::powc(float p) const {
    return RGBColor(pow(r, p), pow(g, p), pow(b, p));
}

float RGBColor::average() const {
    return (r + g + b) / 3.0;
}

RGBColor operator*(const float a, const RGBColor & c){
    return c * a;
}

void RGBColor::clamp() {
    r = std::max(std::min(r, 1.0f), 0.0f);
    g = std::max(std::min(g, 1.0f), 0.0f);
    b = std::max(std::min(b, 1.0f), 0.0f);
}
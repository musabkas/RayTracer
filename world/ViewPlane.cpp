#include "ViewPlane.hpp"

// Constructor
ViewPlane::ViewPlane(){
    this->hres = 640;
    this->vres = 480;
    this->top_left = Point3D(-320, 240, 0);
    this->bottom_right = Point3D(-320 + hres, 240 - vres, 0);
    this->normal = Vector3D(0, 0, 1);
}

// Getters and Setters
int ViewPlane::get_hres() const { return this->hres; }
void ViewPlane::set_hres(int hres) { this->hres = hres; }

int ViewPlane::get_vres() const { return this->vres; }
void ViewPlane::set_vres(int vres) { this->vres = vres; }
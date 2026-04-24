#include "Simple.hpp"
#include "../cameras/Camera.hpp"
#include "../world/ViewPlane.hpp"
#include "../utilities/Ray.hpp"

Simple::Simple(Camera *c_ptr, ViewPlane *v_ptr) : Sampler(c_ptr, v_ptr) {

}

Simple::Simple(const Simple& camera) : Sampler(camera){
    
}

Simple &Simple::operator=(const Simple& other) {
    Sampler::operator=(other);
    return *this;
}

std::vector<Ray> Simple::get_rays(int px, int py) const {
    float x = viewplane_ptr->top_left.x + (px + 0.5) * (viewplane_ptr->bottom_right.x - viewplane_ptr->top_left.x) / viewplane_ptr->hres; 
    float y = viewplane_ptr->top_left.y + (py + 0.5) * (viewplane_ptr->bottom_right.y - viewplane_ptr->top_left.y) / viewplane_ptr->vres; 
    Point3D pos = Point3D(x, y, viewplane_ptr->top_left.z);
    Vector3D dir = camera_ptr->get_direction(pos);
    return {Ray(pos, dir)};
}
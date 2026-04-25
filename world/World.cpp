#include "World.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../geometry/Geometry.hpp"

World::World(){
    vplane = ViewPlane();
    bg_color = RGBColor();
    geometry = {};
    lights = {};
    camera_ptr = nullptr;
    sampler_ptr = nullptr;
}

void World::add_geometry(Geometry *geom_ptr) {
    this->geometry.push_back(geom_ptr);
}

void World::add_light(Light *light_ptr) {
    this->lights.push_back(light_ptr);
}

void World::set_camera(Camera *c_ptr){
    this->camera_ptr = c_ptr;
}

World::~World() {
    for (Geometry* g : this->geometry) {
        delete g;
    }
}

// hit objects
ShadeInfo World::hit_objects(const Ray &ray){
    float t = 1e5;
    ShadeInfo s = ShadeInfo(*this);
    s.hit = false;
    
    for (Geometry* geom_ptr : geometry){
        geom_ptr->hit(ray, t, s);
    }
    return s;
}
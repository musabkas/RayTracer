#include "World.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../geometry/Geometry.hpp"
#include "../acceleration/BVHTree.hpp"
#include "../tracers/Tracer.hpp"

World::World(){
    vplane = ViewPlane();
    bg_color = RGBColor();
    geometry = {};
    lights = {};
    camera_ptr = nullptr;
    sampler_ptr = nullptr;
    tracer_ptr = nullptr;
    bvh_tree = nullptr;
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

void World::set_tracer(Tracer *t_ptr) {
    this->tracer_ptr = t_ptr;
}

World::~World() {
    for (Geometry* g : this->geometry) {
        delete g;
    }
    if (tracer_ptr) {
        delete tracer_ptr;
    }
    if (bvh_tree) {
        delete bvh_tree;
    }
}

// hit objects
ShadeInfo World::hit_objects(const Ray &ray) const {
    // Use BVH tree if available, otherwise fall back to linear search
    if (bvh_tree) {
        return bvh_tree->hit(ray, *this);
    }
    
    // Fallback for scenes without BVH
    float t = 1e5;
    ShadeInfo s(*this);
    s.hit = false;
    
    for (Geometry* geom_ptr : geometry){
        geom_ptr->hit(ray, t, s);
    }
    return s;
}

void World::build_bvh() {
    if (!geometry.empty()) {
        if (bvh_tree) {
            delete bvh_tree;
        }
        bvh_tree = new BVHTree(geometry, *this);
    }
}
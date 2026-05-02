#include "PathTrace.hpp"
#include "../world/World.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../materials/Material.hpp"

RGBColor PathTrace::trace_ray(const Ray& ray, const World& world) const {
    ShadeInfo sr(world.hit_objects(ray));

    if (sr.hit) { // Matches your ShadeInfo "hit" boolean
        sr.ray = ray;
        sr.w = &world; // Matches your ShadeInfo World pointer "w"
        
        return sr.material_ptr->path_shade(sr);
    } else {
        return world.bg_color; 
    }
}
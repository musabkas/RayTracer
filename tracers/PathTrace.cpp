#include "PathTrace.hpp"
#include "../world/World.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../materials/Material.hpp"

RGBColor PathTrace::trace_ray(const Ray& ray, const World& world, int depth) const {
    if (depth > max_depth)
        return RGBColor(0, 0, 0);

    ShadeInfo sr(world.hit_objects(ray));
    if (sr.hit) {
        sr.ray = ray;
        sr.w = &world;
        sr.depth = depth;
        return sr.material_ptr->path_shade(sr);
    } else {
        return world.bg_color;
    }
}
RGBColor PathTrace::trace_ray(const Ray& ray, const World& world) const {
    return trace_ray(ray, world, 0);
}
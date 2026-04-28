#include "Basic.hpp"
#include "../world/World.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../materials/Material.hpp"

RGBColor Basic::trace_ray(const Ray &ray, const World &world) const {
  ShadeInfo sinfo = world.hit_objects(ray);
  
  if (sinfo.hit) {
    return sinfo.material_ptr->shade(sinfo, world.lights);
  } else {
    return world.bg_color;
  }
}

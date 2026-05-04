#include "Shadow.hpp"
#include "../world/World.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Constants.hpp"
#include "../lights/Light.hpp"
#include "../materials/Material.hpp"
#include <algorithm>

RGBColor Shadow::trace_ray(const Ray &ray, const World &world) const {
  ShadeInfo sinfo = world.hit_objects(ray);
  
  if (sinfo.hit) {
    RGBColor color = RGBColor();
    
    for (Light* light : world.lights) {
      Vector3D light_dir = light->get_direction(sinfo.hit_point);
      double cos_angle = sinfo.normal * light_dir;
      
      if (cos_angle > 0.0) {
        
        Point3D shadow_origin = sinfo.hit_point + sinfo.normal * kEpsilon;
        Ray shadow_ray(shadow_origin, light_dir);
        
        

        double distance_to_light = light->get_distance(sinfo.hit_point); 
        
        bool is_in_shadow = world.is_shadowed(shadow_ray, distance_to_light);
        if (!is_in_shadow) {
          std::vector<Light*> single_light = {light};
          RGBColor material_color = sinfo.material_ptr->shade(sinfo, single_light);
          color += material_color * light->L(light_dir) * cos_angle;
        }
      }
    }
    
    color.clamp();
    return color;
  } else {
    return world.bg_color;
  }
}

RGBColor Shadow::trace_ray(const Ray &ray, const World &world, int depth) const {
  return trace_ray(ray, world);
}


/**
   This builds a simple scene that consists of a sphere, a triangle, and a
   plane.
   Parallel viewing is used with a single sample per pixel.
*/

#include "../cameras/Perspective.hpp"

#include "../lights/PointLight.hpp"
#include "../lights/SpotLight.hpp"

#include "../geometry/Plane.hpp"
#include "../geometry/Sphere.hpp"
#include "../geometry/Triangle.hpp"

#include "../materials/Diffuse.hpp"
#include "../materials/Metal.hpp"
#include "../materials/Mirror.hpp"

#include "../samplers/Jittered.hpp"

#include "../utilities/Constants.hpp"

#include "../world/World.hpp"
#include "../tracers/Shadow.hpp"
#include "../tracers/Basic.hpp"

void
World::build(void) {
  // View plane  .
  vplane.top_left.x = -10;
  vplane.top_left.y = 10;
  vplane.top_left.z = 10;
  vplane.bottom_right.x = 10;
  vplane.bottom_right.y = -10;
  vplane.bottom_right.z = 10;
  vplane.hres = 400;
  vplane.vres = 400;

  // Background color.  
  bg_color = black;
  
  // Camera and sampler.
  set_camera(new Perspective(0, 0, 20));
  sampler_ptr = new Jittered(camera_ptr, &vplane, 16);
  set_tracer(new Basic());

  // Point Light
  PointLight* point_light_ptr = new PointLight(1.5, RGBColor(1.0), Point3D(10, 0, 20));
  add_light(point_light_ptr);
  
  // Point Light
  // SpotLight* spot_light_ptr = new SpotLight(1.0, RGBColor(1.0), Point3D(10, 20, 0), Vector3D(-0.5, -1.0, 0), 30);
  // add_light(spot_light_ptr);
	
  // sphere
  Sphere* sphere_ptr = new Sphere(Point3D(-10, 2, 0), 3); 
  sphere_ptr->set_material(new Diffuse(red));
  add_geometry(sphere_ptr);
  
  sphere_ptr = new Sphere(Point3D(0, 2, 0), 3); 
  sphere_ptr->set_material(new Mirror(red, 1.0));
  add_geometry(sphere_ptr);

  sphere_ptr = new Sphere(Point3D(10, 2, 0), 3); 
  sphere_ptr->set_material(new Metal(red, 1.0, 4.0));
  add_geometry(sphere_ptr);
  
  // plane
  Plane* plane_ptr = new Plane(Point3D(0,1,0), Vector3D(0, 20, 4)); 
  plane_ptr->set_material(new Diffuse(green));  // green
  add_geometry(plane_ptr);
}

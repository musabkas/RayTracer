
/**
   This builds a scene to showcase three types of lights:
   PointLight, SpotLight, and RectangularLight.
*/

#include "../cameras/Perspective.hpp"

#include "../lights/PointLight.hpp"
#include "../lights/SpotLight.hpp"
#include "../lights/RectangularLight.hpp"

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
  // View plane
  vplane.top_left.x = -15;
  vplane.top_left.y = 15;
  vplane.top_left.z = 15;
  vplane.bottom_right.x = 15;
  vplane.bottom_right.y = -15;
  vplane.bottom_right.z = 15;
  vplane.hres = 600;
  vplane.vres = 600;

  // Background color.  
  bg_color = black;
  
  // Camera and sampler.
  set_camera(new Perspective(0, 5, 30));
  sampler_ptr = new Jittered(camera_ptr, &vplane, 16);
  set_tracer(new Basic());

  // 1. Point Light (Reddish)
  PointLight* point_light_ptr = new PointLight(2.0, RGBColor(1.0, 0, 0), Point3D(-10, 5, 5));
  add_light(point_light_ptr);
  
  // 2. Spot Light (Greenish) - Pointing down at the middle sphere
  SpotLight* spot_light_ptr = new SpotLight(3.0, RGBColor(0, 1.0, 0), Point3D(0, 15, 5), Vector3D(0, -1, -0.5), 30);
  add_light(spot_light_ptr);

  // 3. Rectangular Light (Blueish)
  // corner, v1, v2, color, radiance
//   RectangularLight* rect_light_ptr = new RectangularLight(Point3D(8, 15, 5), Vector3D(4, 0, 0), Vector3D(0, 0, 4), RGBColor(0, 0, 1.0), 3.0);
//   add_light(rect_light_ptr);
	
  // Spheres to catch the light
  Sphere* sphere_ptr;

  // Left sphere - primarily lit by Point Light
  sphere_ptr = new Sphere(Point3D(-10, 2, 0), 4); 
  sphere_ptr->set_material(new Diffuse(white));
  add_geometry(sphere_ptr);

  // Middle sphere - primarily lit by Spot Light
  sphere_ptr = new Sphere(Point3D(0, 2, 0), 4); 
  sphere_ptr->set_material(new Diffuse(white));
  add_geometry(sphere_ptr);

  // Right sphere - primarily lit by Rectangular Light
//   sphere_ptr = new Sphere(Point3D(10, 2, 0), 4); 
//   sphere_ptr->set_material(new Diffuse(white));
//   add_geometry(sphere_ptr);
  
  // Ground plane
  Plane* plane_ptr = new Plane(Point3D(0, -2, 0), Vector3D(0, 1, 0)); 
  plane_ptr->set_material(new Diffuse(RGBColor(0.5)));
  add_geometry(plane_ptr);
}


/**
   This builds a simple scene for Path Tracing, based on buildHelloWorld.
   It uses Jittered sampling (100 samples per pixel) and the PathTrace tracer.
   Materials are changed to Diffuse and a PointLight is added to support Next Event Estimation.
*/

#include "../cameras/Perspective.hpp"

#include "../geometry/Plane.hpp"
#include "../geometry/Sphere.hpp"
#include "../geometry/Triangle.hpp"

#include "../materials/Diffuse.hpp"

#include "../samplers/Jittered.hpp"

#include "../lights/PointLight.hpp"

#include "../utilities/Constants.hpp"

#include "../world/World.hpp"
#include "../tracers/PathTrace.hpp"

void
World::build(void) {
  // View plane
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
  // Path tracing requires many samples for a clean image
  sampler_ptr = new Jittered(camera_ptr, &vplane, 16); 
  set_tracer(new PathTrace(5));

  // Add a light source for direct lighting (Next Event Estimation)
  add_light(new PointLight(5.0, white, Point3D(10, 15, 10)));
	
  // sphere
  Sphere* sphere_ptr = new Sphere(Point3D(-3, 2, 0), 5); 
  sphere_ptr->set_material(new Diffuse(red));
  add_geometry(sphere_ptr);
  
  // triangle
  Point3D a(2.5, -5, 1); 
  Point3D b(14, -1, 0); 
  Point3D c(8.5, 5, 0.5); 
  Triangle* triangle_ptr = new Triangle(a, b, c);
  triangle_ptr->set_material(new Diffuse(blue));
  add_geometry(triangle_ptr);

  // plane
  Plane* plane_ptr = new Plane(Point3D(0, 1, 0), Vector3D(0, 10, 2)); 
  plane_ptr->set_material(new Diffuse(green));
  add_geometry(plane_ptr);

  // Build BVH for performance (essential for path tracing)
  build_bvh();
}

/**
   This builds a psychedelic Menger Sponge fractal out of spheres.
   Uses a BVH (Bounding Volume Hierarchy) for efficient ray tracing.
*/

#include <cmath> // Required for std::sin

#include "../cameras/Perspective.hpp"

#include "../lights/PointLight.hpp"
#include "../lights/SpotLight.hpp"

#include "../geometry/Plane.hpp"
#include "../geometry/Sphere.hpp"
#include "../geometry/Triangle.hpp"

#include "../materials/Cosine.hpp"
#include "../materials/Diffuse.hpp"
#include "../materials/Metal.hpp"

#include "../samplers/Jittered.hpp"

#include "../utilities/Constants.hpp"

#include "../world/World.hpp"
#include "../tracers/Shadow.hpp"


// Recursive function to generate the Menger Sponge structure.
// Adds sphere geometries directly to the world's geometry list.
void buildMengerSponge(const Point3D& center, float size, int depth, World* world) {
    // Base case: place a sphere when we hit the bottom of the recursion tree
    if (depth == 0) {
        // Psychedelic coordinate-based coloring
        float r = 0.5 + 0.5 * std::sin(center.x * 0.8);
        float g = 0.5 + 0.5 * std::sin(center.y * 0.8);
        float b = 0.5 + 0.5 * std::sin(center.z * 0.8);
        
        Sphere* sphere_ptr = new Sphere(center, size * 0.5); 
        sphere_ptr->set_material(new Diffuse(RGBColor(r, g, b)));
        world->add_geometry(sphere_ptr);
        return;
    }

    // Recursive step: subdivide space into a 3x3x3 grid
    float newSize = size / 3.0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                
                // Menger logic: punch out the center and the center of each face
                int zeroCount = (x == 0) + (y == 0) + (z == 0);
                if (zeroCount >= 2) continue; 

                Point3D newCenter(
                    center.x + (x * newSize),
                    center.y + (y * newSize),
                    center.z + (z * newSize)
                );
                
                buildMengerSponge(newCenter, newSize, depth - 1, world);
            }
        }
    }
}


void
World::build(void) {
  // View plane
  vplane.top_left.x = -10;
  vplane.top_left.y = 10;
  vplane.top_left.z = 10;
  vplane.bottom_right.x = 10;
  vplane.bottom_right.y = -10;
  vplane.bottom_right.z = 10;
  vplane.hres = 480;
  vplane.vres = 360;

  // Background color
  bg_color = black; 
  
  // Camera and sampler.
  set_camera(new Perspective(0, 0, 120));
  
  sampler_ptr = new Jittered(camera_ptr, &vplane, 4);
  set_tracer(new Shadow());

  // Faked Directional Light ("The Sun")
  // We place a PointLight extremely far away so its rays arrive essentially parallel.
  // The high Y and Z values give it a nice angled downward trajectory.
  PointLight* sun_light_ptr = new PointLight(2.0, white, Point3D(5000, 10000, 5000));
  add_light(sun_light_ptr);
  
  // Sky/Ambient Fill Light
  // A secondary light placed far away on the opposite side so the deep 
  // internal cavities of the labyrinth aren't completely pitch black.
  PointLight* fill_light_ptr = new PointLight(0.6, white, Point3D(-5000, -2000, -5000));
  add_light(fill_light_ptr);
  
  // Build the Fractal (Size 100, Depth 3)
  // Adds all geometry spheres to the world
  buildMengerSponge(Point3D(0, 0, 0), 100.0, 3, this);
  
  // Build the BVH tree for efficient ray tracing
  build_bvh();
}
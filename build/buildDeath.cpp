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
// #include "../tracers/Shadow.hpp"
#include "../tracers/PathTrace.hpp"
#include "../lights/RectangularLight.hpp"
#include "../utilities/ShadeInfo.hpp"
#include <vector> // Just in case it's not included yet
#include <cstdlib>
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

// Helper function to stab the sponge with tall, thin triangles from the outside in
void addSpikes(World* world, int num_spikes) {
    // Material* black_matte = new Diffuse(RGBColor(0.0, 0.0, 0.0));
    Material* crystal_mat = new Metal(RGBColor(0.85f, 0.92f, 1.0f), 1.0f, 5.0f);
    
    // The center of your Menger sponge
    Point3D center(0, 0, 50); 
    const float PI = 3.14159265359f;

    for (int i = 0; i < num_spikes; i++) {
        // 1. Generate a uniformly random 3D direction
        float u = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; // -1 to 1
        float theta = ((float)rand() / RAND_MAX) * 2.0f * PI; // 0 to 2PI
        
        float r = std::sqrt(std::max(0.0f, 1.0f - u * u));
        Vector3D dir(r * std::cos(theta), r * std::sin(theta), u);
        dir.normalize();

        // 2. Place the base completely OUTSIDE the sponge
        // The sponge is 100 units wide, so a radius of ~86 is the absolute max corner.
        // Pushing the base 130 units away guarantees it starts in the void.
        Point3D base_center = center + dir * 130.0f;

        // 3. Place the tip completely THROUGH the center
        // We shoot backwards along the direction between 60 and 110 units.
        // This ensures the spike pierces at least past the center point, 
        // and some will completely impale the other side.
        float pierce_depth = 60.0f + ((float)rand() / RAND_MAX) * 50.0f;
        Point3D tip = center - dir * pierce_depth;

        // 4. Calculate the thin base vertices perpendicular to the spike's direction
        // We need an arbitrary vector to calculate the cross product
        Vector3D arbitrary(1.0f, 0.0f, 0.0f);
        if (std::abs(dir.x) > 0.9f) {
            arbitrary = Vector3D(0.0f, 1.0f, 0.0f);
        }
        
        // Manual cross product: tangent = dir x arbitrary
        Vector3D tangent(
            dir.y * arbitrary.z - dir.z * arbitrary.y,
            dir.z * arbitrary.x - dir.x * arbitrary.z,
            dir.x * arbitrary.y - dir.y * arbitrary.x
        );
        tangent.normalize();

        // Offset the base center to create a super narrow triangle
        float width = 12.0f; // 1 unit wide (very sharp!)
        Point3D v1 = base_center + tangent * width;
        Point3D v2 = base_center - tangent * width;

        // Add the geometry
        Triangle* spike = new Triangle(tip, v1, v2);
        // spike->set_material(black_matte);
        spike->set_material(crystal_mat);

        world->add_geometry(spike);
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
  bg_color = bg; 
  
  // Camera and sampler.
  set_camera(new Perspective(0, 0, 0));
  
  sampler_ptr = new Jittered(camera_ptr, &vplane, 16);
  set_tracer(new PathTrace());

//   // Faked Directional Light ("The Sun")
//   // We place a PointLight extremely far away so its rays arrive essentially parallel.
//   // The high Y and Z values give it a nice angled downward trajectory.
//   PointLight* sun_light_ptr = new PointLight(2.0, white, Point3D(5000, 10000, 5000));
//   add_light(sun_light_ptr);
  
//   // Sky/Ambient Fill Light
//   // A secondary light placed far away on the opposite side so the deep 
//   // internal cavities of the labyrinth aren't completely pitch black.
//   PointLight* fill_light_ptr = new PointLight(0.6, white, Point3D(-5000, -2000, -5000));
//   add_light(fill_light_ptr);
  
// Create a giant glowing square in the sky
Point3D corner(20, 100, 30);       // Top center
Vector3D edge1(40, 0, 0);        // 40 units wide on X
Vector3D edge2(0, 0, 40);        // 40 units wide on Z

RectangularLight* area_light = new RectangularLight(corner, edge1, edge2, RGBColor(1.0, 0.5, 1.0), 5.0);
add_light(area_light);
// Build the Fractal (Size 100, Depth 3)
// Adds all geometry spheres to the world
buildMengerSponge(Point3D(0, 0, 50), 100.0, 3, this);

addSpikes(this, 6);

// Build the BVH tree for efficient ray tracing
build_bvh();
}
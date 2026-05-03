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
#include "../materials/Emissive.hpp"

#include "../samplers/Jittered.hpp"

#include "../utilities/Constants.hpp"

#include "../world/World.hpp"
// #include "../tracers/Shadow.hpp"
#include "../tracers/PathTrace.hpp"
#include "../lights/RectangularLight.hpp"
#include "../utilities/ShadeInfo.hpp"
#include <vector> // Just in case it's not included yet
#include <cstdlib>
#include <iostream>
#include <ostream>
// Recursive function to generate the Menger Sponge structure.
// Adds sphere geometries directly to the world's geometry list.

void addCenterOrb(World* world) {
    // Just a soft central fill light, no geometry
    PointLight* pl = new PointLight(2.0f, RGBColor(1.0f, 0.85f, 0.6f), Point3D(0, 0, 50));
    world->add_light(pl);
}

void addDiamondFrame(World* world) {
    float s = 20.0f;
    float tilt = 0.3f;

    auto rot = [&](Point3D p) -> Point3D {
        float y = p.y * std::cos(tilt) - p.z * std::sin(tilt);
        float z = p.y * std::sin(tilt) + p.z * std::cos(tilt);
        return Point3D(p.x, y, z + 50);
    };

    Point3D top   = rot(Point3D( 0,  s,  0));
    Point3D bot   = rot(Point3D( 0, -s,  0));
    Point3D front = rot(Point3D( 0,  0,  s));
    Point3D back  = rot(Point3D( 0,  0, -s));
    Point3D left  = rot(Point3D(-s,  0,  0));
    Point3D right = rot(Point3D( s,  0,  0));

    // Point lights at each vertex of the frame
    float light_intensity = 3.0f;
    RGBColor light_color(1.0f, 0.85f, 0.6f);
    world->add_light(new PointLight(light_intensity, light_color, top));
    world->add_light(new PointLight(light_intensity, light_color, bot));
    world->add_light(new PointLight(light_intensity, light_color, front));
    world->add_light(new PointLight(light_intensity, light_color, back));
    world->add_light(new PointLight(light_intensity, light_color, left));
    world->add_light(new PointLight(light_intensity, light_color, right));

    // Material* frame_mat = new Metal(RGBColor(0.7f, 0.75f, 0.8f), 1.0f, 1.0f);
    Material* frame_mat = new Diffuse(RGBColor(0.1f, 0.1f, 0.3f));

    auto addEdge = [&](Point3D a, Point3D b) {
        Vector3D along(b.x - a.x, b.y - a.y, b.z - a.z);
        along.normalize();

        Vector3D arbitrary(1.0f, 0.0f, 0.0f);
        if (std::abs(along.x) > 0.9f) arbitrary = Vector3D(0.0f, 1.0f, 0.0f);

        Vector3D perp(
            along.y * arbitrary.z - along.z * arbitrary.y,
            along.z * arbitrary.x - along.x * arbitrary.z,
            along.x * arbitrary.y - along.y * arbitrary.x
        );
        perp.normalize();

        float thickness = 2.5f;
        Point3D a1(a.x + perp.x * thickness, a.y + perp.y * thickness, a.z + perp.z * thickness);
        Point3D a2(a.x - perp.x * thickness, a.y - perp.y * thickness, a.z - perp.z * thickness);
        Point3D b1(b.x + perp.x * thickness, b.y + perp.y * thickness, b.z + perp.z * thickness);
        Point3D b2(b.x - perp.x * thickness, b.y - perp.y * thickness, b.z - perp.z * thickness);

        Triangle* t1  = new Triangle(a1, b1, a2);
        Triangle* t2  = new Triangle(b1, b2, a2);
        Triangle* t1b = new Triangle(a2, b1, a1);
        Triangle* t2b = new Triangle(a2, b2, b1);

        t1->set_material(frame_mat);
        t2->set_material(frame_mat);
        t1b->set_material(frame_mat);
        t2b->set_material(frame_mat);

        world->add_geometry(t1);
        world->add_geometry(t2);
        world->add_geometry(t1b);
        world->add_geometry(t2b);
    };

    addEdge(top, front);  addEdge(top, back);
    addEdge(top, left);   addEdge(top, right);
    addEdge(bot, front);  addEdge(bot, back);
    addEdge(bot, left);   addEdge(bot, right);
    addEdge(front, left); addEdge(front, right);
    addEdge(back, left);  addEdge(back, right);
}

void buildMengerSponge(const Point3D& center, float size, int depth, World* world, bool on_tunnel_wall = false) {
    if (depth == 0) {
        float r = 0.5 + 0.5 * std::sin(center.x * 0.8);
        float g = 0.5 + 0.5 * std::sin(center.y * 0.8);
        float b = 0.5 + 0.5 * std::sin(center.z * 0.8);

        Sphere* sphere_ptr = new Sphere(center, size * 0.5);

        sphere_ptr->set_material(new Diffuse(RGBColor(r, g, b)));
        

        world->add_geometry(sphere_ptr);
        return;
    }

    float newSize = size / 3.0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                int zeroCount = (x == 0) + (y == 0) + (z == 0);
                if (zeroCount >= 2) continue;

                // zeroCount == 1 means exactly one axis is 0 — this cell lines a tunnel
                bool cell_on_tunnel = (zeroCount == 1);

                Point3D newCenter(
                    center.x + (x * newSize),
                    center.y + (y * newSize),
                    center.z + (z * newSize)
                );

                buildMengerSponge(newCenter, newSize, depth - 1, world, on_tunnel_wall || cell_on_tunnel);
            }
        }
    }
}

void addSpikes(World* world) {
    Material* spike_mat = new Metal(RGBColor(0.85f, 0.92f, 1.0f), 1.0f, 1.0f);

    Point3D orb(0, 0, 50);
    float half = 50.0f; // sponge is 100 units wide, centered at orb

    // 8 corners of the bounding cube
    float corners[8][3] = {
        {-half, -half, orb.z - half},
        { half, -half, orb.z - half},
        {-half,  half, orb.z - half},
        { half,  half, orb.z - half},
        {-half, -half, orb.z + half},
        { half, -half, orb.z + half},
        {-half,  half, orb.z + half},
        { half,  half, orb.z + half},
    };

    float pyramid_height = 90.0f; // how far the tip reaches inward — doesn't touch orb
    float base_size = 8.0f;       // how wide the base is

    for (int i = 0; i < 8; i++) {
        Point3D base_center(corners[i][0], corners[i][1], corners[i][2]);

        // Direction from corner toward orb
        Vector3D dir(
            orb.x - base_center.x,
            orb.y - base_center.y,
            orb.z - base_center.z
        );
        dir.normalize();

        // Tip reaches inward but stops short of the orb
        Point3D tip(
            base_center.x + dir.x * pyramid_height,
            base_center.y + dir.y * pyramid_height,
            base_center.z + dir.z * pyramid_height
        );

        // Build a local coordinate frame at the base
        Vector3D arbitrary(1.0f, 0.0f, 0.0f);
        if (std::abs(dir.x) > 0.9f) arbitrary = Vector3D(0.0f, 1.0f, 0.0f);

        Vector3D tangent(
            dir.y * arbitrary.z - dir.z * arbitrary.y,
            dir.z * arbitrary.x - dir.x * arbitrary.z,
            dir.x * arbitrary.y - dir.y * arbitrary.x
        );
        tangent.normalize();

        Vector3D bitangent(
            dir.y * tangent.z - dir.z * tangent.y,
            dir.z * tangent.x - dir.x * tangent.z,
            dir.x * tangent.y - dir.y * tangent.x
        );
        bitangent.normalize();

        // 4 base vertices of the pyramid
        Point3D v0(
            base_center.x + tangent.x * base_size + bitangent.x * base_size,
            base_center.y + tangent.y * base_size + bitangent.y * base_size,
            base_center.z + tangent.z * base_size + bitangent.z * base_size
        );
        Point3D v1(
            base_center.x - tangent.x * base_size + bitangent.x * base_size,
            base_center.y - tangent.y * base_size + bitangent.y * base_size,
            base_center.z - tangent.z * base_size + bitangent.z * base_size
        );
        Point3D v2(
            base_center.x - tangent.x * base_size - bitangent.x * base_size,
            base_center.y - tangent.y * base_size - bitangent.y * base_size,
            base_center.z - tangent.z * base_size - bitangent.z * base_size
        );
        Point3D v3(
            base_center.x + tangent.x * base_size - bitangent.x * base_size,
            base_center.y + tangent.y * base_size - bitangent.y * base_size,
            base_center.z + tangent.z * base_size - bitangent.z * base_size
        );

        // 4 side faces of the pyramid, each double-sided
        auto addFace = [&](Point3D a, Point3D b, Point3D c) {
            Triangle* front = new Triangle(a, b, c);
            Triangle* back  = new Triangle(a, c, b);
            front->set_material(spike_mat);
            back->set_material(spike_mat);
            world->add_geometry(front);
            world->add_geometry(back);
        };

        addFace(tip, v0, v1);
        addFace(tip, v1, v2);
        addFace(tip, v2, v3);
        addFace(tip, v3, v0);

        // Base face (square, split into 2 triangles), double-sided
        addFace(v0, v1, v2);
        addFace(v0, v2, v3);
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
  bg_color = RGBColor(0.0, 0.0, 0.1); 
  
  // Camera and sampler.
  set_camera(new Perspective(0, 0, 0));
  
  sampler_ptr = new Jittered(camera_ptr, &vplane, 64);
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
Point3D corner(0, 100, 0);       // Top center
Vector3D edge1(40, 0, 0);        // 40 units wide on X
Vector3D edge2(0, 0, 40);        // 40 units wide on Z

RectangularLight* area_light = new RectangularLight(corner, edge1, edge2, RGBColor(0.0, 0.0, 1.0), 3.0);
add_light(area_light);
// Build the Fractal (Size 100, Depth 3)
// Adds all geometry spheres to the world
buildMengerSponge(Point3D(0, 0, 50), 100.0, 2, this);

addCenterOrb(this);
addDiamondFrame(this);

addSpikes(this);

// Build the BVH tree for efficient ray tracing
int sphere_count = 0, triangle_count = 0;
for (Geometry* g : geometry) {
    if (dynamic_cast<Sphere*>(g)) sphere_count++;
    else if (dynamic_cast<Triangle*>(g)) triangle_count++;
}
std::cout << "Primitives: " << geometry.size() << " total ("
          << sphere_count << " spheres, "
          << triangle_count << " triangles)" << std::endl;
build_bvh();
}
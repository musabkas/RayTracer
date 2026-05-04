/**
   This builds a psychedelic Menger Sponge fractal out of spheres.
   Uses a BVH (Bounding Volume Hierarchy) for efficient ray tracing.
*/

#include <algorithm>
#include <cmath>

#include "../cameras/Perspective.hpp"

#include "../lights/PointLight.hpp"
#include "../lights/SpotLight.hpp"

#include "../geometry/Plane.hpp"
#include "../geometry/Sphere.hpp"
#include "../geometry/Triangle.hpp"

#include "../materials/Cosine.hpp"
#include "../materials/Diffuse.hpp"
#include "../materials/Metal.hpp"
#include "../materials/SpecularDiffuse.hpp"
#include "../materials/Emissive.hpp"
#include "../materials/Translucent.hpp"
#include "../materials/Mirror.hpp"
#include "../materials/Wavy.hpp"

#include "../samplers/Jittered.hpp"
#include "../samplers/DOFSampler.hpp"

#include "../utilities/Constants.hpp"

#include "../world/World.hpp"
#include "../tracers/PathTrace.hpp"
#include "../lights/RectangularLight.hpp"
#include "../utilities/ShadeInfo.hpp"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <ostream>

void addCenterOrb(World* world) {
    // PointLight* pl = new PointLight(1.5f, RGBColor(1.0f, 1.0f, 1.0f), Point3D(0, 20, 50));
    PointLight* pl = new PointLight(2.0f, RGBColor(1.0f, 1.0f, 0.9f), Point3D(0, 20, 50));
    world->add_light(pl);
    return;
}

void addDiamondFrame(World* world) {
    const float cz = 50.f;
    const float s = 10.f;
    const Point3D center(0.f, 0.f, cz);

    Point3D top(0.f, s, cz);
    Point3D bot(0.f, -s, cz);
    Point3D front(0.f, 0.f, cz + s);
    Point3D back(0.f, 0.f, cz - s);
    Point3D left(-s, 0.f, cz);
    Point3D right(s, 0.f, cz);

    const float hull_scale = 0.90f;
    auto hull_v = [&](Point3D v) -> Point3D {
        return Point3D(
            center.x + (v.x - center.x) * hull_scale,
            center.y + (v.y - center.y) * hull_scale,
            center.z + (v.z - center.z) * hull_scale
        );
    };

    // Material* hull_mat = new Translucent(RGBColor(0.30f, 0.30f, 1.0f), 1.50f, 1.01f, 1.00f);
    
    // Material* hull_mat = new Metal(RGBColor(1.0f, 0.84f, 0.0f), 0.8f, 40.0f); 
    Material* hull_mat = new WavyMirror(RGBColor(1.0f, 1.0f, 1.0f), 2.5f, 0.4f);
    Material* rib_mat = new Diffuse(RGBColor(0.0f, 0.0f, 0.0f));

    auto addDiamondFace = [&](Point3D a, Point3D b, Point3D c) {
        Triangle* t = new Triangle(a, b, c);
        Triangle* tb = new Triangle(a, c, b);
        t->set_material(hull_mat);
        tb->set_material(hull_mat);
        world->add_geometry(t);
        world->add_geometry(tb);
    };

    addDiamondFace(hull_v(top), hull_v(front), hull_v(right));
    addDiamondFace(hull_v(top), hull_v(right), hull_v(back));
    addDiamondFace(hull_v(top), hull_v(back), hull_v(left));
    addDiamondFace(hull_v(top), hull_v(left), hull_v(front));
    addDiamondFace(hull_v(bot), hull_v(front), hull_v(left));
    addDiamondFace(hull_v(bot), hull_v(left), hull_v(back));
    addDiamondFace(hull_v(bot), hull_v(back), hull_v(right));
    addDiamondFace(hull_v(bot), hull_v(right), hull_v(front));

    Material* frame_mat = rib_mat;

    auto addEdge = [&](Point3D a, Point3D b) {
        Vector3D along(b.x - a.x, b.y - a.y, b.z - a.z);
        float len2 = along.x * along.x + along.y * along.y + along.z * along.z;
        if (len2 < 1e-6f) return;
        along.normalize();

        Vector3D arbitrary(1.0f, 0.0f, 0.0f);
        if (std::abs(along.x) > 0.9f) arbitrary = Vector3D(0.0f, 1.0f, 0.0f);

        Vector3D perp(
            along.y * arbitrary.z - along.z * arbitrary.y,
            along.z * arbitrary.x - along.x * arbitrary.z,
            along.x * arbitrary.y - along.y * arbitrary.x
        );
        perp.normalize();

        float thickness = 0.2f * s;
        thickness = std::max(0.15f, std::min(thickness, s * 0.18f));
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

        // 1. Static counter persists across all recursive calls
        static int counter = 0;

        // 2. Modulo operator checks if it's the 5th sphere
        if (counter % 10 == 0) {
            // Make it Specular (using your commented-out parameters)
            sphere_ptr->set_material(new Mirror(RGBColor(1.0f, 1.0f, 1.0f), 1.0f));
        } else {
            // Make it normal Diffuse
            sphere_ptr->set_material(new Diffuse(RGBColor(r, g, b)));
        }
        
        // Increment the counter for the next sphere
        counter++;

        world->add_geometry(sphere_ptr);
        return;
    }

    float newSize = size / 3.0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                int zeroCount = (x == 0) + (y == 0) + (z == 0);
                if (zeroCount >= 2) continue;

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

// // ── Mandala Shell ─────────────────────────────────────────────────────────────

// static bool mandalaKeep(float u, float v) {
//     // Simple circular hole in the center of each face.
//     // Tune this:
//     const float holeRadius = 0.35f; // 0.0 = no hole, 0.5 = hole touches edges

//     float px = u - 0.5f;
//     float py = v - 0.5f;
//     float r  = std::sqrt(px * px + py * py);

//     return r > holeRadius; // keep if outside the hole
// }

// static void addMandalaFace(World* world, Material* mat,
//                             Point3D p00, Point3D p10,
//                             Point3D p01, Point3D p11,
//                             int gridN = 32) {
//     auto lerp4 = [&](float u, float v) -> Point3D {
//         float ou = 1.f - u, ov = 1.f - v;
//         return Point3D(
//             ou*ov*p00.x + u*ov*p10.x + ou*v*p01.x + u*v*p11.x,
//             ou*ov*p00.y + u*ov*p10.y + ou*v*p01.y + u*v*p11.y,
//             ou*ov*p00.z + u*ov*p10.z + ou*v*p01.z + u*v*p11.z
//         );
//     };

//     for (int j = 0; j < gridN; ++j) {
//         for (int i = 0; i < gridN; ++i) {
//             float u0 = float(i)     / gridN;
//             float u1 = float(i + 1) / gridN;
//             float v0 = float(j)     / gridN;
//             float v1 = float(j + 1) / gridN;
//             float uc = (u0 + u1) * 0.5f;
//             float vc = (v0 + v1) * 0.5f;

//             if (!mandalaKeep(uc, vc)) continue;

//             Point3D a = lerp4(u0, v0);
//             Point3D b = lerp4(u1, v0);
//             Point3D c = lerp4(u0, v1);
//             Point3D d = lerp4(u1, v1);

//             auto addTri = [&](Point3D x, Point3D y, Point3D z) {
//                 Triangle* tf = new Triangle(x, y, z);
//                 tf->set_material(mat);
//                 world->add_geometry(tf);
//             };

//             addTri(a, b, d); // upper-right triangle
//             addTri(a, d, c); // lower-left triangle
//         }
//     }
// }

// void buildMandalaShell(World* world, const Point3D& center, float size) {
//     Material* mat = new Diffuse(RGBColor(1.00f, 0.30f, 0.3f));

//     float step = size / 3.f;
//     float h    = step * 0.5f;

//     for (int x = -1; x <= 1; ++x) {
//         for (int y = -1; y <= 1; ++y) {
//             for (int z = -1; z <= 1; ++z) {
//                 // Menger depth-1: skip face-centers (one zero) and center (all zero)
//                 int zeros = (x==0) + (y==0) + (z==0);
//                 if (zeros >= 2) continue;

//                 Point3D c(
//                     center.x + x * step,
//                     center.y + y * step,
//                     center.z + z * step
//                 );

//                 // 8 corners of this sub-cube
//                 Point3D p000(c.x-h, c.y-h, c.z-h);
//                 Point3D p100(c.x+h, c.y-h, c.z-h);
//                 Point3D p010(c.x-h, c.y+h, c.z-h);
//                 Point3D p110(c.x+h, c.y+h, c.z-h);
//                 Point3D p001(c.x-h, c.y-h, c.z+h);
//                 Point3D p101(c.x+h, c.y-h, c.z+h);
//                 Point3D p011(c.x-h, c.y+h, c.z+h);
//                 Point3D p111(c.x+h, c.y+h, c.z+h);

//                 // All 6 faces get mandala treatment
//                 // -Z face
//                 addMandalaFace(world, mat, p000, p100, p010, p110);
//                 // +Z face
//                 addMandalaFace(world, mat, p001, p101, p011, p111);
//                 // -X face
//                 addMandalaFace(world, mat, p000, p001, p010, p011);
//                 // +X face
//                 addMandalaFace(world, mat, p100, p101, p110, p111);
//                 // -Y face
//                 addMandalaFace(world, mat, p000, p100, p001, p101);
//                 // +Y face
//                 addMandalaFace(world, mat, p010, p110, p011, p111);
//             }
//         }
//     }
// }

// Returns true if the cell (x,y,z) in {-1,0,1}^3 is SOLID in the Menger sponge
static bool mengerSolid(int x, int y, int z) {
    // A cell is removed if 2+ coordinates are zero (face-center or body-center)
    int zeros = (x == 0) + (y == 0) + (z == 0);
    return zeros < 2;
}

// Emits a single quad (as 2 triangles) with CCW winding when viewed from 'outside'
static void emitQuad(World* world, Material* mat,
                     Point3D a, Point3D b, Point3D c, Point3D d) {
    Triangle* t1 = new Triangle(a, b, c);
    Triangle* t2 = new Triangle(a, c, d);
    // Back-face so rays from inside also see it
    Triangle* t1b = new Triangle(a, c, b);
    Triangle* t2b = new Triangle(a, d, c);
    t1->set_material(mat);  t2->set_material(mat);
    t1b->set_material(mat); t2b->set_material(mat);
    world->add_geometry(t1);  world->add_geometry(t2);
    world->add_geometry(t1b); world->add_geometry(t2b);
}

// Builds the OUTER SHELL of a depth-1 Menger sponge:
// Only emits faces where a solid sub-cube is adjacent to a removed (tunnel) cell.
// The face gets the Sierpiński carpet subdivided treatment so holes are visible.
void buildMengerShell(World* world, const Point3D& center, float size) {
    Material* mat = new Diffuse(RGBColor(0.85f, 0.85f, 0.85f));

    float step = size / 3.f;
    float h    = step * 0.5f;

    // Face directions and their axes
    // For each of the 6 face directions, we check: is the neighbor in that direction empty?
    // If yes, emit this face.

    struct FaceDir { int dx, dy, dz; };
    const FaceDir dirs[6] = {
        { 1, 0, 0}, {-1, 0, 0},
        { 0, 1, 0}, { 0,-1, 0},
        { 0, 0, 1}, { 0, 0,-1}
    };

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                if (!mengerSolid(x, y, z)) continue; // skip removed cells

                Point3D c(
                    center.x + x * step,
                    center.y + y * step,
                    center.z + z * step
                );

                // 8 corners
                Point3D p000(c.x-h, c.y-h, c.z-h);
                Point3D p100(c.x+h, c.y-h, c.z-h);
                Point3D p010(c.x-h, c.y+h, c.z-h);
                Point3D p110(c.x+h, c.y+h, c.z-h);
                Point3D p001(c.x-h, c.y-h, c.z+h);
                Point3D p101(c.x+h, c.y-h, c.z+h);
                Point3D p011(c.x-h, c.y+h, c.z+h);
                Point3D p111(c.x+h, c.y+h, c.z+h);

                // For each of 6 directions, check if the neighbor is empty (a hole)
                // +X neighbor
                if (!mengerSolid(x+1, y, z) || x == 1)
                    emitQuad(world, mat, p101, p100, p110, p111); // +X face, outward CCW
                // -X neighbor
                if (!mengerSolid(x-1, y, z) || x == -1)
                    emitQuad(world, mat, p000, p001, p011, p010); // -X face
                // +Y neighbor
                if (!mengerSolid(x, y+1, z) || y == 1)
                    emitQuad(world, mat, p011, p111, p110, p010); // +Y face
                // -Y neighbor
                if (!mengerSolid(x, y-1, z) || y == -1)
                    emitQuad(world, mat, p001, p000, p100, p101); // -Y face
                // +Z neighbor
                if (!mengerSolid(x, y, z+1) || z == 1)
                    emitQuad(world, mat, p001, p101, p111, p011); // +Z face (front)
                // -Z neighbor
                if (!mengerSolid(x, y, z-1) || z == -1)
                    emitQuad(world, mat, p100, p000, p010, p110); // -Z face (back)
            }
        }
    }
}



// ─────────────────────────────────────────────────────────────────────────────

void
World::build(void) {
  vplane.top_left.x = -10;
  vplane.top_left.y = 10;
  vplane.top_left.z = 10;
  vplane.bottom_right.x = 10;
  vplane.bottom_right.y = -10;
  vplane.bottom_right.z = 10;
  vplane.hres = 480;
  vplane.vres = 360;

//   bg_color = RGBColor(0.5, 0.5, 0.5);
    bg_color = black;
    Perspective* my_cam = new Perspective(0, 0, 0);

  set_camera(new Perspective(0, 0, 0));
  sampler_ptr = new Jittered(camera_ptr, &vplane, 64);
// sampler_ptr = new DOFSampler(my_cam, &vplane, 32, 100.0f, 0.0f);  
set_tracer(new PathTrace(5));

//   Point3D corner(0, 100, 0);
//   Vector3D edge1(40, 0, 0);
//   Vector3D edge2(0, 0, 40);
//   RectangularLight* area_light = new RectangularLight(corner, edge1, edge2, RGBColor(1.0, 1.0, 1.0), 1.0);
//   add_light(area_light);
    // 1. Move the corner way down below the sponge (Y = -80)
    // We offset X and Z to center the 40x40 light directly under the sponge at (0, 0, 50)
    Point3D corner(-20, -80, 30); 
    
    // 2. CRITICAL: Swap edge1 and edge2! 
    // In area lights, the normal vector (which way it shines) is calculated using 
    // the Cross Product (edge1 x edge2). 
    // Z crossed with X points UP (+Y). X crossed with Z points DOWN (-Y).
    Vector3D edge1(0, 0, 40); // Swapped to Z
    Vector3D edge2(40, 0, 0); // Swapped to X
    
    // 3. Set the color to a soft, ethereal blue
    RGBColor soft_blue(0.1f, 0.3f, 1.0f);
    
    RectangularLight* area_light = new RectangularLight(corner, edge1, edge2, soft_blue, 1.5f);
    add_light(area_light);

  buildMengerSponge(Point3D(0, 0, 50), 100.0, 3, this);

//   buildMandalaShell(this, Point3D(0, 0, 50), 100.0f);
    // buildMengerShell(this, Point3D(0, 0, 50), 100.0f);     // solid shell with holes over it


  addCenterOrb(this);
  addDiamondFrame(this);

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
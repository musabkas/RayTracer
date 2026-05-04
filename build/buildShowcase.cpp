#include <algorithm>
#include <cmath>
#include <iostream>

#include "../cameras/Perspective.hpp"

#include "../lights/PointLight.hpp"
#include "../lights/SpotLight.hpp"
#include "../lights/RectangularLight.hpp"

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
#include "../materials/Wavy.hpp" // Note: using Wavy.hpp based on your compiler logs

#include "../samplers/Jittered.hpp"
#include "../utilities/Constants.hpp"
#include "../world/World.hpp"
#include "../tracers/PathTrace.hpp"

void World::build(void) {
    // 1. Setup Camera and View Plane
    vplane.top_left.x = -10;
    vplane.top_left.y = 10;
    vplane.top_left.z = 10;
    vplane.bottom_right.x = 10;
    vplane.bottom_right.y = -10;
    vplane.bottom_right.z = 10;
    vplane.hres = 800; 
    vplane.vres = 400;

    bg_color = RGBColor(0.2f, 0.2f, 0.3f); 

    // Keep the camera perfectly still at the origin
    set_camera(new Perspective(0, 0, 0));
    sampler_ptr = new Jittered(camera_ptr, &vplane, 64);
    set_tracer(new PathTrace(5)); 

    // 2. Setup Lighting 
    // Overhead light is now moved to cast light onto our "wall"
    Point3D corner(-20, 40, 20);
    Vector3D edge1(40, 0, 0);
    Vector3D edge2(0, 0, 40);
    RectangularLight* area_light = new RectangularLight(corner, edge1, edge2, RGBColor(1.0f, 1.0f, 1.0f), 0.8f);
    add_light(area_light);

    PointLight* warm_light = new PointLight(0.5f, RGBColor(1.0f, 0.8f, 0.6f), Point3D(-30, 20, 10));
    add_light(warm_light);

    // 3. Setup The Stage (The Floor is now a Back Wall)
    Material* floor_mat = new Diffuse(RGBColor(0.7f, 0.7f, 0.7f)); 
    
    // Push the plane to Z = 65, facing backward toward the camera (0, 0, -1)
    Plane* floor = new Plane(Point3D(0, 0, 65), Vector3D(0, 0, -1));
    floor->set_material(floor_mat);
    add_geometry(floor);

    // 4. THE SHOWCASE SPHERES
    float radius = 6.0f;
    float z_pos = 50.0f; // Floating right in front of the wall
    float y_pos = 0.0f;  // Centered vertically in the camera's view

    // 1. Diffuse 
    Sphere* s1 = new Sphere(Point3D(-32.0f, y_pos, z_pos), radius);
    s1->set_material(new Diffuse(RGBColor(0.8f, 0.2f, 0.2f))); 
    add_geometry(s1);

    // 2. SpecularDiffuse
    Sphere* s2 = new Sphere(Point3D(-16.0f, y_pos, z_pos), radius);
    s2->set_material(new SpecularDiffuse(RGBColor(0.2f, 0.8f, 0.2f), 0.5f, 1.6f, 2.0f)); 
    add_geometry(s2);

    // 3. Metal
    Sphere* s3 = new Sphere(Point3D(0.0f, y_pos, z_pos), radius);
    s3->set_material(new Metal(RGBColor(1.0f, 0.84f, 0.0f), 0.8f, 40.0f)); 
    add_geometry(s3);

    // 4. Mirror 
    Sphere* s5 = new Sphere(Point3D(16.0f, y_pos, z_pos), radius);
    s5->set_material(new Mirror(RGBColor(1.0f, 1.0f, 1.0f), 1.0f)); 
    add_geometry(s5);

    // 5. WavyMirror 
    Sphere* s6 = new Sphere(Point3D(32.0f, y_pos, z_pos), radius);
    s6->set_material(new WavyMirror(RGBColor(0.8f, 0.2f, 1.0f), 2.5f, 0.4f)); 
    add_geometry(s6);

    // Build Acceleration Structure
    std::cout << "Showcase Scene Built: " << geometry.size() << " primitives." << std::endl;
    build_bvh();
}
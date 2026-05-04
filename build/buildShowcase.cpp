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
#include "../materials/Wavy.hpp" 
#include "../samplers/Jittered.hpp"
#include "../samplers/DOFSampler.hpp"
#include "../utilities/Constants.hpp"
#include "../world/World.hpp"
#include "../tracers/Basic.hpp"
#include "../tracers/Shadow.hpp"
#include "../tracers/PathTrace.hpp"

void World::build(void) {
    // =========================================================================
    // ⚙️ THE RENDER TOGGLE
    // =========================================================================
    int RENDER_MODE = 0; 
    // =========================================================================

    vplane.top_left = Point3D(-10, 10, 10);
    vplane.bottom_right = Point3D(10, -10, 10);
    vplane.hres = 800; 
    vplane.vres = 400;
    bg_color = RGBColor(0.2f, 0.2f, 0.3f); 

    Perspective* my_cam = new Perspective(0, 0, 0); // Original Camera Position
    set_camera(my_cam);

    std::cout << "--- Starting Render in MODE " << RENDER_MODE << " ---" << std::endl;

    // LIGHTING & SAMPLER SETTINGS BY MODE
    if (RENDER_MODE == 0) {
        sampler_ptr = new Jittered(my_cam, &vplane, 1);
        set_tracer(new Shadow());
        add_light(new PointLight(2.0f, RGBColor(1.0f), Point3D(-20, 40, 20))); 
    } 
    else if (RENDER_MODE == 1) {
        sampler_ptr = new Jittered(my_cam, &vplane, 64);
        set_tracer(new Shadow());
        add_light(new PointLight(2.0f, RGBColor(1.0f), Point3D(-20, 40, 20))); 
    }
    else if (RENDER_MODE >= 2) {
        if (RENDER_MODE == 2) {
            sampler_ptr = new Jittered(my_cam, &vplane, 64);
            set_tracer(new Shadow());
        } else if (RENDER_MODE == 3) {
            sampler_ptr = new Jittered(my_cam, &vplane, 64);
            set_tracer(new PathTrace(5));
        } else { // MODE 4
            sampler_ptr = new DOFSampler(my_cam, &vplane, 64, 50.0f, 1.5f);
            set_tracer(new PathTrace(5));
        }
        
        // Soft Overhead Area Light
        add_light(new RectangularLight(Point3D(-20, 50, 30), Vector3D(40, 0, 0), Vector3D(0, 0, 40), RGBColor(1.0f), 0.8f));
        // Fill Point Light
        add_light(new PointLight(0.5f, RGBColor(1.0f, 0.8f, 0.6f), Point3D(-30, 20, 20)));
    }

    // 3. THE STAGE (Floor is at Y = -10 again)
    Material* floor_mat = new Diffuse(RGBColor(0.7f, 0.7f, 0.7f)); 
    Plane* floor = new Plane(Point3D(0, -10, 0), Vector3D(0, 1, 0));
    floor->set_material(floor_mat);
    add_geometry(floor);

    float radius = 6.0f;
    float z_pos = 50.0f; 
    float y_pos = -10.0f + radius; // Sitting on the floor

    // SPHERES LINEUP
    Sphere* s1 = new Sphere(Point3D(-32.0f, y_pos, z_pos), radius);
    s1->set_material(new Diffuse(RGBColor(0.8f, 0.2f, 0.2f))); 
    add_geometry(s1);

    Sphere* s2 = new Sphere(Point3D(-16.0f, y_pos, z_pos), radius);
    s2->set_material(new SpecularDiffuse(RGBColor(0.2f, 0.8f, 0.2f), 0.5f, 1.6f, 2.0f)); 
    add_geometry(s2);

    Sphere* s3 = new Sphere(Point3D(0.0f, y_pos, z_pos), radius);
    s3->set_material(new Metal(RGBColor(1.0f, 0.84f, 0.0f), 0.8f, 40.0f)); 
    add_geometry(s3);

    Sphere* s4 = new Sphere(Point3D(16.0f, y_pos, z_pos), radius);
    s4->set_material(new Mirror(RGBColor(1.0f, 1.0f, 1.0f), 1.0f)); 
    add_geometry(s4);

    Sphere* s5 = new Sphere(Point3D(32.0f, y_pos, z_pos), radius);
    s5->set_material(new WavyMirror(RGBColor(0.8f, 0.2f, 1.0f), 2.5f, 0.4f)); 
    add_geometry(s5);

    build_bvh();
}
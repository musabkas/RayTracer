#include "Wavy.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../world/World.hpp"
#include "../tracers/Tracer.hpp"
#include "../utilities/Ray.hpp"

#include "../lights/Light.hpp"
#include <cmath>
#include "../utilities/Constants.hpp"

WavyMirror::WavyMirror(const RGBColor& tint, float freq, float amp)
    : Material(), tint_(tint), frequency_(freq), amplitude_(amp) {}

RGBColor WavyMirror::shade(const ShadeInfo& sinfo, const std::vector<Light*>& lights) const {
    RGBColor L(0.0f);

    // 1. Calculate the distorted normal for the waves
    Point3D p = sinfo.hit_point;
    float distort = amplitude_ * std::sin(frequency_ * p.x) * std::cos(frequency_ * p.y);
    Vector3D wavy_normal = sinfo.normal + Vector3D(distort, distort, distort);
    wavy_normal.normalize();

    // 2. Setup viewer direction (from hit point back to camera)
    Vector3D wo = -sinfo.ray.d;

    for (Light* light : lights) {
        // 3. Get direction to light and its radiance
        Vector3D wi = light->get_direction(sinfo.hit_point); 
        float ndotwi = wavy_normal * wi;

        if (ndotwi > 0.0f) {
            // 4. Calculate Phong Specular Highlight (The 'glint' of the light)
            Vector3D r = (wavy_normal * 2.0f * ndotwi) - wi;
            float rdotwo = r * wo;
            
            if (rdotwo > 0.0f) {
                // Use the tint_ variable found in your .hpp
                L += tint_ * std::pow(rdotwo, 50.0f) * light->L(wi);
            }
        }
    }
    return L;
}

RGBColor WavyMirror::path_shade(ShadeInfo& sr) {
    if (sr.depth > 5)
        return RGBColor(0.0f);

    Vector3D wo = -sr.ray.d;
    wo.normalize();

    Vector3D n = sr.normal;
    n.normalize();

    // 1. THE TRIPPY MATH: Overlapping sine waves based on 3D hit coordinates
    Vector3D p = sr.hit_point;
    Vector3D distortion(
        std::sin(frequency_ * p.y) * std::cos(frequency_ * p.z),
        std::sin(frequency_ * p.x) * std::cos(frequency_ * p.z),
        std::sin(frequency_ * p.x) * std::cos(frequency_ * p.y)
    );

    // 2. Warp the normal
    Vector3D wavy_normal = n + (distortion * amplitude_);
    wavy_normal.normalize();

    // 3. Calculate perfect mirror reflection around the FAKE, wavy normal
    Vector3D reflected = -wo + 2.0f * (wo * wavy_normal) * wavy_normal;
    reflected.normalize();

    // SAFETY CHECK: If the wave amplitude is so high that it reflects the ray 
    // INSIDE the solid object, fallback to the true normal to prevent black pixel artifacts.
    if ((reflected * n) < 0.0f) {
        reflected = -wo + 2.0f * (wo * n) * n;
        reflected.normalize();
    }

    // 4. Shoot the bounced ray
    Ray reflected_ray(sr.hit_point + n * 0.001f, reflected);
    RGBColor incoming = sr.w->tracer_ptr->trace_ray(reflected_ray, *(sr.w), sr.depth + 1);

    return incoming * tint_;
}
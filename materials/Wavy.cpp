#include "Wavy.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../world/World.hpp"
#include "../tracers/Tracer.hpp"
#include "../utilities/Ray.hpp"

#include "../lights/Light.hpp"
#include <cmath>

WavyMirror::WavyMirror(const RGBColor& tint, float freq, float amp)
    : tint_(tint), frequency_(freq), amplitude_(amp) {}

RGBColor WavyMirror::shade(const ShadeInfo& sinfo, const std::vector<Light*>& lights) const {
    // Basic fallback for non-path tracers. We keep it dark because it's a pure mirror.
    return RGBColor(0.0f);
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
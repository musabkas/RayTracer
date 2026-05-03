#include "Translucent.hpp"

#include "../BRDF/Lambertian.hpp"
#include "../lights/Light.hpp"
#include "../tracers/Tracer.hpp"
#include "../utilities/Constants.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../world/World.hpp"

#include <algorithm>
#include <cmath>
#include <random>

namespace {

inline float schlick_reflectance(float cosine, float ref_idx_ratio) {
  cosine = std::clamp(cosine, 0.f, 1.f);
  float r0 = (1.f - ref_idx_ratio) / (1.f + ref_idx_ratio);
  r0 = r0 * r0;
  return std::clamp(r0 + (1.f - r0) * std::pow(1.f - cosine, 5.f), 0.f, 1.f);
}

inline Vector3D reflect_unit(const Vector3D &unit_v, const Vector3D &unit_n) {
  return unit_v - unit_n * (2.f * (float)(unit_v * unit_n));
}

// RT Weekend refract().
inline bool refract_unit(const Vector3D &unit_v, const Vector3D &unit_n, float etai_over_etat, Vector3D &out) {
  float cos_theta = std::min(-(float)(unit_v * unit_n), 1.f);
  Vector3D r_out_perp = (unit_v + unit_n * cos_theta) * etai_over_etat;
  float perp_sq = (float)r_out_perp.len_squared();
  if (perp_sq >= 1.f)
    return false;
  Vector3D r_out_parallel = unit_n * (-std::sqrt(1.f - perp_sq));
  out = r_out_perp + r_out_parallel;
  out.normalize();
  return true;
}

} // namespace

Translucent::Translucent(const RGBColor &tint, float ior, float diffuse_frac, float roughness)
    : tint_(tint), ior_(ior), diffuse_frac_(diffuse_frac), roughness_(roughness) {}

RGBColor Translucent::shade(const ShadeInfo &sinfo, const std::vector<Light *> &lights) const {
  Lambertian lm(tint_ * (1.f - diffuse_frac_));
  RGBColor color;
  for (Light *light_ptr : lights) {
    Vector3D dir = light_ptr->get_direction(sinfo.hit_point);
    float nd = (float)(sinfo.normal * dir);
    if (nd > 0.0)
      color +=
          lm.f(light_ptr->get_direction(sinfo.hit_point), sinfo.ray.d, sinfo.normal) * light_ptr->L(dir) * nd;
  }
  color.clamp();
  return color;
}

RGBColor Translucent::path_shade(ShadeInfo &sr) {
  if (sr.depth > 5)
    return black;

  thread_local std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<float> u01(0.0f, 1.0f);

  Vector3D unit_direction(sr.ray.d);
  unit_direction.normalize();

  Vector3D n(sr.normal);
  n.normalize();

  bool front_face = (unit_direction * n) < 0.f;
  float eta_ratio = front_face ? (1.f / ior_) : ior_;

  float cos_theta = std::min(-(float)(unit_direction * n), 1.f);
  float sin_theta = std::sqrt(std::max(0.f, 1.f - cos_theta * cos_theta));
  bool cannot_refract = eta_ratio * sin_theta > 1.f;
  float reflect_prob = cannot_refract ? 1.f : schlick_reflectance(cos_theta, eta_ratio);

  Vector3D scatter_dir;
  bool tinted_transmission_path = false;

  if (cannot_refract || u01(gen) < reflect_prob) {
    scatter_dir = reflect_unit(unit_direction, n);
  } else {
    Vector3D refr;
    if (refract_unit(unit_direction, n, eta_ratio, refr)) {
      scatter_dir = refr;
      tinted_transmission_path = true;
    } else {
      scatter_dir = reflect_unit(unit_direction, n);
    }
  }

  if (roughness_ > 0.0f) {
    std::uniform_real_distribution<float> perturb_dist(-1.0f, 1.0f);
    Vector3D perturb(perturb_dist(gen), perturb_dist(gen), perturb_dist(gen));
    perturb.normalize();
    scatter_dir = scatter_dir + roughness_ * perturb;
    scatter_dir.normalize();

    if (!tinted_transmission_path && (scatter_dir * n) < 0.0f) {
      scatter_dir = reflect_unit(unit_direction, n);
    }
  }

  Ray scattered(sr.hit_point + scatter_dir * (kEpsilon * 500.f), scatter_dir);
  RGBColor next = sr.w->tracer_ptr->trace_ray(scattered, *sr.w, sr.depth + 1);

  return tinted_transmission_path ? next * tint_ : next;
}

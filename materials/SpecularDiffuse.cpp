#include "SpecularDiffuse.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../lights/Light.hpp"
#include "../world/World.hpp"
#include "../tracers/Tracer.hpp"
#include <random>
#include <cmath>
#include <algorithm>

SpecularDiffuse::SpecularDiffuse() {
  lambertian_brdf = Lambertian();
  glossy_brdf = Glossy();
  kd = 0.75f;
  ks = 0.25f;
}

SpecularDiffuse::SpecularDiffuse(const RGBColor &color, float kd, float ks, float e) {
  lambertian_brdf = Lambertian(color);
  glossy_brdf = Glossy(RGBColor(1.0f, 1.0f, 1.0f), 1.0f, e);
  this->kd = kd;
  this->ks = ks;
}

SpecularDiffuse::SpecularDiffuse(const SpecularDiffuse &other) {
  lambertian_brdf = other.lambertian_brdf;
  glossy_brdf = other.glossy_brdf;
  kd = other.kd;
  ks = other.ks;
}

SpecularDiffuse &SpecularDiffuse::operator=(const SpecularDiffuse &other) {
  lambertian_brdf = other.lambertian_brdf;
  glossy_brdf = other.glossy_brdf;
  kd = other.kd;
  ks = other.ks;
  return *this;
}

RGBColor SpecularDiffuse::shade(const ShadeInfo &sinfo, const std::vector<Light *> &lights) const {
  RGBColor color(0.0f);
  Vector3D wo = -sinfo.ray.d;
  wo.normalize();

  for (Light* light_ptr : lights) {
    Vector3D wi = light_ptr->get_direction(sinfo.hit_point);
    float ndotwi = static_cast<float>(sinfo.normal * wi);
    if (ndotwi <= 0.0f)
      continue;

    RGBColor diffuse = lambertian_brdf.f(wi, wo, sinfo.normal) * kd * light_ptr->L(wi) * ndotwi;
    RGBColor specular = glossy_brdf.f(wi, wo, sinfo.normal) * ks * light_ptr->L(wi) * ndotwi;
    color += diffuse + specular;
  }

  color.clamp();
  return color;
}

RGBColor SpecularDiffuse::path_shade(ShadeInfo& sr) {
  if (sr.depth > 5)
    return RGBColor(0, 0, 0);

  RGBColor L_direct(0.0f);
  Vector3D wo = -sr.ray.d;
  wo.normalize();

  for (Light* light_ptr : sr.w->lights) {
    Vector3D wi = light_ptr->get_direction(sr.hit_point);
    float ndotwi = static_cast<float>(sr.normal * wi);
    if (ndotwi <= 0.0f)
      continue;

    Ray shadow_ray(sr.hit_point + sr.normal * 0.001f, wi);
    ShadeInfo shadow_sr = sr.w->hit_objects(shadow_ray);
    bool in_shadow = shadow_sr.hit && shadow_sr.t < light_ptr->get_distance(sr.hit_point);

    if (!in_shadow) {
      L_direct += lambertian_brdf.f(wi, wo, sr.normal) * kd * light_ptr->L(wi) * ndotwi;
      L_direct += glossy_brdf.f(wi, wo, sr.normal) * ks * light_ptr->L(wi) * ndotwi;
    }
  }

  float kd_norm = kd;
  float ks_norm = ks;
  float sum = kd_norm + ks_norm;
  if (sum <= 0.0f)
    return L_direct;

  kd_norm /= sum;
  ks_norm /= sum;

  thread_local std::mt19937 generator(std::random_device{}());
  std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

  RGBColor L_indirect(0.0f);
  if (distribution(generator) < kd_norm) {
    Vector3D w = sr.normal;
    Vector3D v = Vector3D(0.0034f, 1.0f, 0.0071f) ^ w;
    v.normalize();
    Vector3D u = v ^ w;

    float r1 = distribution(generator);
    float r2 = distribution(generator);
    float phi = 2.0f * 3.14159265359f * r1;
    float r = std::sqrt(r2);
    float x = r * std::cos(phi);
    float y = r * std::sin(phi);
    float z = std::sqrt(std::max(0.0f, 1.0f - r2));

    Vector3D diffuse_dir = x * u + y * v + z * w;
    diffuse_dir.normalize();

    Ray bounce_ray(sr.hit_point + sr.normal * 0.001f, diffuse_dir);
    RGBColor incoming = sr.w->tracer_ptr->trace_ray(bounce_ray, *(sr.w), sr.depth + 1);
    RGBColor f = lambertian_brdf.f(diffuse_dir, wo, sr.normal);

    L_indirect = incoming * f / kd_norm;
  } else {
    Vector3D wo_dir = wo;
    Vector3D reflected = -wo_dir + 2.0f * (wo_dir * sr.normal) * sr.normal;
    reflected.normalize();

    float roughness = 0.07f;
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    Vector3D perturb(dist(generator), dist(generator), dist(generator));
    perturb.normalize();

    Vector3D glossy_dir = reflected + roughness * perturb;
    glossy_dir.normalize();
    if ((glossy_dir * sr.normal) < 0.0f)
      glossy_dir = reflected;

    Ray reflection_ray(sr.hit_point + sr.normal * 0.001f, glossy_dir);
    RGBColor incoming = sr.w->tracer_ptr->trace_ray(reflection_ray, *(sr.w), sr.depth + 1);
    RGBColor f = glossy_brdf.f(glossy_dir, wo, sr.normal);

    L_indirect = incoming * f / ks_norm;
  }

  RGBColor result = L_direct + L_indirect;
  result.clamp();
  return result;
}

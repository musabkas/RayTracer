// #include "Cylinder.hpp"
// #include <cmath>

// Cylinder::Cylinder(float _y0, float _y1, float _r, float _cx, float _cz)
//     : Geometry(), y0(_y0), y1(_y1), radius(_r), center_x(_cx), center_z(_cz) {}

// BBox Cylinder::get_bounding_box() const {
//     return BBox(center_x - radius, y0, center_z - radius, 
//                 center_x + radius, y1, center_z + radius);
// }

// bool Cylinder::hit(const Ray& ray, double& tmin, ShadeInfo& sr) const {
//     double a = (ray.d.x * ray.d.x) + (ray.d.z * ray.d.z);
//     double b = 2.0 * (ray.d.x * (ray.o.x - center_x) + ray.d.z * (ray.o.z - center_z));
//     double c = (ray.o.x - center_x) * (ray.o.x - center_x) + 
//                (ray.o.z - center_z) * (ray.o.z - center_z) - (radius * radius);

//     double discriminant = b * b - 4.0 * a * c;

//     if (discriminant < 0.0) return false;

//     double e = sqrt(discriminant);
//     double denom = 2.0 * a;
    
//     // Check smaller root first
//     double t = (-b - e) / denom;
//     if (t > kEpsilon) {
//         double yhit = ray.o.y + t * ray.d.y;
//         if (yhit > y0 && yhit < y1) {
//             tmin = t;
//             sr.normal = Vector3D((ray.o.x + t * ray.d.x) - center_x, 0.0, (ray.o.z + t * ray.d.z) - center_z);
//             sr.normal.normalize();
//             sr.local_hit_point = ray.o + t * ray.d;
//             return true;
//         }
//     }

//     // Check larger root
//     t = (-b + e) / denom;
//     if (t > kEpsilon) {
//         double yhit = ray.o.y + t * ray.d.y;
//         if (yhit > y0 && yhit < y1) {
//             tmin = t;
//             sr.normal = Vector3D((ray.o.x + t * ray.d.x) - center_x, 0.0, (ray.o.z + t * ray.d.z) - center_z);
//             sr.normal.normalize();
//             sr.local_hit_point = ray.o + t * ray.d;
//             return true;
//         }
//     }

//     return false;
// }

// bool Cylinder::shadow_hit(const Ray& ray, float& tmin) const {
//     // Exact same math as hit(), but without ShadeInfo calculations for maximum speed
//     double a = (ray.d.x * ray.d.x) + (ray.d.z * ray.d.z);
//     double b = 2.0 * (ray.d.x * (ray.o.x - center_x) + ray.d.z * (ray.o.z - center_z));
//     double c = (ray.o.x - center_x) * (ray.o.x - center_x) + 
//                (ray.o.z - center_z) * (ray.o.z - center_z) - (radius * radius);

//     double discriminant = b * b - 4.0 * a * c;
//     if (discriminant < 0.0) return false;

//     double e = sqrt(discriminant);
//     double denom = 2.0 * a;
    
//     double t = (-b - e) / denom;
//     if (t > kEpsilon) {
//         double yhit = ray.o.y + t * ray.d.y;
//         if (yhit > y0 && yhit < y1) {
//             tmin = t;
//             return true;
//         }
//     }

//     t = (-b + e) / denom;
//     if (t > kEpsilon) {
//         double yhit = ray.o.y + t * ray.d.y;
//         if (yhit > y0 && yhit < y1) {
//             tmin = t;
//             return true;
//         }
//     }
//     return false;
// }
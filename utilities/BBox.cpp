#include "BBox.hpp"
#include "Ray.hpp"
#include "../geometry/Geometry.hpp"

BBox::BBox(const Point3D& min, const Point3D& max){
    this->pmin = min;
    this->pmax = max;
}

std::string BBox::to_string() const {
    return this->pmin.to_string() + ", " + this->pmax.to_string();
}

// hit, extend, contain, overlap
bool BBox::hit(const Ray &ray, float &t_enter, float &t_exit) const {
    float tx0, tx1;
    if (ray.d.x == 0) {
        if (ray.o.x >= pmin.x && ray.o.x <= pmax.x) {
            tx0 = 0;
            tx1 = 1e5;
        } else {
            tx0 = 1;
            tx1 = 0;
        }
    } else {
        tx0 = (pmin.x - ray.o.x) / ray.d.x;
        tx1 = (pmax.x - ray.o.x) / ray.d.x;
        if (tx0 > tx1) {
            float tmp = tx1;
            tx1 = tx0;
            tx0 = tmp;
        }
    }

    float ty0, ty1;
    if (ray.d.y == 0) {
        if (ray.o.y >= pmin.y && ray.o.y <= pmax.y) {
            ty0 = 0;
            ty1 = 1e5;
        } else {
            ty0 = 1;
            ty1 = 0;
        }
    } else {
        ty0 = (pmin.y - ray.o.y) / ray.d.y;
        ty1 = (pmax.y - ray.o.y) / ray.d.y;
        if (ty0 > ty1) {
            float tmp = ty1;
            ty1 = ty0;
            ty0 = tmp;
        }
    }

    float tz0, tz1;
    if (ray.d.z == 0) {
        if (ray.o.z >= pmin.z && ray.o.z <= pmax.z) {
            tz0 = 0;
            tz1 = 1e5;
        } else {
            tz0 = 1;
            tz1 = 0;
        }
    } else {
        tz0 = (pmin.z - ray.o.z) / ray.d.z;
        tz1 = (pmax.z - ray.o.z) / ray.d.z;
        if (tz0 > tz1) {
            float tmp = tz1;
            tz1 = tz0;
            tz0 = tmp;
        }
    }

    t_enter = std::max(tx0, std::max(ty0, tz0));
    t_exit = std::min(tx1, std::min(ty1, tz1));
    return t_enter >= t_exit;
}

void BBox::extend(const BBox& b){
    pmin = min(pmin, b.pmin);
    pmax = min(pmax, b.pmax);
}

void BBox::extend(Geometry* g){
    extend(g->getBBox());
}

bool BBox::contains(const Point3D& p){
    return (p.x >= pmin.x && p.x <= pmax.x) && (p.y >= pmin.y && p.y <= pmax.y) && (p.z >= pmin.z && p.z <= pmax.z);
}

bool BBox::overlaps(const BBox& b){
    return (std::max(pmin.x, b.pmin.x) <= std::min(pmax.x, b.pmax.x)) && (std::max(pmin.y, b.pmin.y) <= std::min(pmax.y, b.pmax.y)) && (std::max(pmin.z, b.pmin.z) <= std::min(pmax.z, b.pmax.z));
}

bool BBox::overlaps(Geometry* g){
    return overlaps(g->getBBox());
}
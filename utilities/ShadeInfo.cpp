#include "ShadeInfo.hpp"

ShadeInfo::ShadeInfo(const World &wr) {
    w = &wr;
    hit = false;
    material_ptr = nullptr;
}
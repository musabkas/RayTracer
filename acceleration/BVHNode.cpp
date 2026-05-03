#include "BVHNode.hpp"
#include "../geometry/Geometry.hpp"
#include "../utilities/Ray.hpp"

BVHNode::BVHNode() : left(nullptr), right(nullptr), lower(0), upper(-1) {}

BVHNode::BVHNode(int lower_idx, int upper_idx) 
  : left(nullptr), right(nullptr), lower(lower_idx), upper(upper_idx) {}

BVHNode::~BVHNode() {
  if (left) delete left;
  if (right) delete right;
}

bool BVHNode::isLeaf() const { return left == nullptr && right == nullptr; }
void BVHNode::setLeft(BVHNode *l) { left = l; }
void BVHNode::setRight(BVHNode *r) { right = r; }
BVHNode *BVHNode::getLeft() const { return left; }
BVHNode *BVHNode::getRight() const { return right; }
void BVHNode::setBBox(const BBox &b) { bbox = b; }
BBox BVHNode::getBBox() const { return bbox; }
int BVHNode::getLower() const { return lower; }
int BVHNode::getUpper() const { return upper; }

bool BVHNode::hit(const Ray &ray, ShadeInfo &sinfo, const std::vector<Geometry *> &primitives,
                       const std::vector<int> &indices, const World &world) const {
  float t_enter, t_exit;
  if (!bbox.hit(ray, t_enter, t_exit)) {
    return false;
  }

  // PRUNING FIX
  if (t_enter >= sinfo.t) {
    return false;
  }

  bool hit_anything = false;

  // Inside BVHNode::hit ...
if (isLeaf()) {
  for (int i = lower; i <= upper; ++i) {
    // Add 'indices' indirection here!
    if (primitives[indices[i]]->hit(ray, sinfo.t, sinfo)) {
        hit_anything = true;
    }
  }
  return hit_anything;
}


  // Front-to-back optimization
  float t_left_enter = 1e6, t_left_exit;
  float t_right_enter = 1e6, t_right_exit;
  bool hit_left_box = left && left->getBBox().hit(ray, t_left_enter, t_left_exit);
  bool hit_right_box = right && right->getBBox().hit(ray, t_right_enter, t_right_exit);

  BVHNode* first = nullptr;
  BVHNode* second = nullptr;

  if (hit_left_box && hit_right_box) {
      if (t_left_enter < t_right_enter) {
          first = left;
          second = right;
      } else {
          first = right;
          second = left;
      }
  } else if (hit_left_box) {
      first = left;
  } else if (hit_right_box) {
      first = right;
  }

  
if (first) { if (first->hit(ray, sinfo, primitives, indices, world)) hit_anything = true; }
if (second) { if (second->hit(ray, sinfo, primitives, indices, world)) hit_anything = true; }

  return hit_anything;
}

bool BVHNode::is_shadowed(const Ray &ray, float max_t, const std::vector<Geometry *> &primitives,
                          const std::vector<int> &indices, const World &world) const {
  float t_enter, t_exit;
  if (!bbox.hit(ray, t_enter, t_exit) || t_enter >= max_t) {
    return false;
  }

  if (isLeaf()) {
    for (int i = lower; i <= upper; ++i) {
      float t = max_t;
      // create dummy shadeinfo as geometry hit requires it
      ShadeInfo dummy_sinfo(world);
      
      // FIX: Use the indices array to look up the correct, sorted primitive
      if (primitives[indices[i]]->hit(ray, t, dummy_sinfo) && dummy_sinfo.t > 0.0001f && dummy_sinfo.t < max_t) {
          return true; // Any hit!
      }
    }
    return false;
  }

  // FIX: Pass the indices array down recursively
  if (left && left->is_shadowed(ray, max_t, primitives, indices, world)) return true;
  if (right && right->is_shadowed(ray, max_t, primitives, indices, world)) return true;

  return false;
}

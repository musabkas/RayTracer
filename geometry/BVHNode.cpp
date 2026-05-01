#include "BVHNode.hpp"
#include "Geometry.hpp"
#include "../utilities/Ray.hpp"

BVHNode::BVHNode() : left(nullptr), right(nullptr), lower(0), upper(-1) {}

BVHNode::BVHNode(int lower_idx, int upper_idx) 
  : left(nullptr), right(nullptr), lower(lower_idx), upper(upper_idx) {}

BVHNode::~BVHNode() {
  if (left) delete left;
  if (right) delete right;
}

bool BVHNode::isLeaf() const {
  return left == nullptr && right == nullptr;
}

void BVHNode::setLeft(BVHNode *l) {
  left = l;
}

void BVHNode::setRight(BVHNode *r) {
  right = r;
}

BVHNode *BVHNode::getLeft() const {
  return left;
}

BVHNode *BVHNode::getRight() const {
  return right;
}

void BVHNode::setBBox(const BBox &b) {
  bbox = b;
}

BBox BVHNode::getBBox() const {
  return bbox;
}

int BVHNode::getLower() const {
  return lower;
}

int BVHNode::getUpper() const {
  return upper;
}

ShadeInfo BVHNode::hit(const Ray &ray, const std::vector<Geometry *> &primitives,
                       const World &world) const {
  ShadeInfo shade_info(world);
  shade_info.hit = false;
  shade_info.t = 1e5f;
  
  // Check if ray hits this node's bounding box
  float t_enter, t_exit;
  if (!bbox.hit(ray, t_enter, t_exit)) {
    // Ray missed bbox, no intersection possible in this subtree
    return shade_info;
  }

  // CRITICAL OPTIMIZATION: If the bbox intersection starts behind the current
  // closest hit, we can prune this entire subtree - no need to recurse!
  if (t_enter >= shade_info.t) {
    return shade_info;
  }

  // If this is a leaf node, test intersection with its primitives
  if (isLeaf()) {
    for (int i = lower; i <= upper; ++i) {
      primitives[i]->hit(ray, shade_info.t, shade_info);
    }
    return shade_info;
  }

  // Internal node: recurse to both children
  // As we find closer intersections, shade_info.t gets updated,
  // which allows further pruning in deeper nodes
  if (left) {
    ShadeInfo left_info = left->hit(ray, primitives, world);
    if (left_info.hit && left_info.t < shade_info.t) {
      shade_info = left_info;
    }
  }
  if (right) {
    ShadeInfo right_info = right->hit(ray, primitives, world);
    if (right_info.hit && right_info.t < shade_info.t) {
      shade_info = right_info;
    }
  }

  return shade_info;
}

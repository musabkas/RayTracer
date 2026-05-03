#include "BVHTree.hpp"
#include "../geometry/Geometry.hpp"
#include <algorithm>
#include <climits>

BVHTree::BVHTree(const std::vector<Geometry *> &geometries, const World &world) : root(nullptr) {
  primitives = geometries;
  
  // Initialize indices array
  indices.resize(primitives.size());
  for (size_t i = 0; i < primitives.size(); ++i) {
    indices[i] = i;
  }
  
  if (!primitives.empty()) {
    root = buildNode(0, primitives.size() - 1);
  }
}

BVHTree::~BVHTree() {
  if (root) {
    delete root;
  }
}

BBox BVHTree::computeBBox(int lower, int upper) const {
  if (lower > upper) {
    return BBox();
  }
  
  BBox bbox = primitives[indices[lower]]->getBBox();
  for (int i = lower + 1; i <= upper; ++i) {
    bbox.extend(primitives[indices[i]]);
  }
  return bbox;
}

int BVHTree::findSplitAxis(int lower, int upper) const {
  BBox bbox = computeBBox(lower, upper);
  float dx = bbox.pmax.x - bbox.pmin.x;
  float dy = bbox.pmax.y - bbox.pmin.y;
  float dz = bbox.pmax.z - bbox.pmin.z;
  
  if (dx > dy && dx > dz) return 0;  // X axis
  if (dy > dz) return 1;               // Y axis
  return 2;                            // Z axis
}

int BVHTree::findSplitPosition(int lower, int upper, int axis) {
  int mid = (lower + upper) / 2;
  
  // Sort indices by centroid along the split axis (don't modify primitives array)
  std::sort(indices.begin() + lower, indices.begin() + upper + 1,
    [this, axis](int a, int b) {
      BBox abox = primitives[a]->getBBox();
      BBox bbox = primitives[b]->getBBox();
      float acentroid = (abox.pmin.x + abox.pmax.x) / 2.0f;
      float bcentroid = (bbox.pmin.x + bbox.pmax.x) / 2.0f;
      
      if (axis == 1) {
        acentroid = (abox.pmin.y + abox.pmax.y) / 2.0f;
        bcentroid = (bbox.pmin.y + bbox.pmax.y) / 2.0f;
      } else if (axis == 2) {
        acentroid = (abox.pmin.z + abox.pmax.z) / 2.0f;
        bcentroid = (bbox.pmin.z + bbox.pmax.z) / 2.0f;
      }
      return acentroid < bcentroid;
    });
  
  return mid;
}

BVHNode *BVHTree::buildNode(int lower, int upper) {
  if (lower > upper) return nullptr;
  
  BVHNode *node = new BVHNode(lower, upper);
  BBox bbox = computeBBox(lower, upper);
  node->setBBox(bbox);
  
  // If only one primitive, it's a leaf
  if (lower == upper) {
    return node;
  }
  
  // Otherwise, split the primitives
  int axis = findSplitAxis(lower, upper);
  int mid = findSplitPosition(lower, upper, axis);
  
  // Ensure we actually split
  if (mid <= lower) mid = lower + 1;
  if (mid > upper) mid = upper;
  
  BVHNode *left = buildNode(lower, mid - 1);
  BVHNode *right = buildNode(mid, upper);
  
  node->setLeft(left);
  node->setRight(right);
  
  return node;
}

void BVHTree::hit(const Ray &ray, ShadeInfo &sinfo, const World &world) const {
  if (root) {
    root->hit(ray, sinfo, primitives, indices, world);
  }
}

bool BVHTree::is_shadowed(const Ray &ray, float max_t, const World &world) const {
    if (root) {
        return root->is_shadowed(ray, max_t, primitives, indices, world);
    }
    return false;
}


BVHNode *BVHTree::getRoot() const {
  return root;
}

const std::vector<Geometry *> &BVHTree::getPrimitives() const {
  return primitives;
}

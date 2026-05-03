#pragma once

/**
   This file declares the BVHNode class which represents a single node in a
   Bounding Volume Hierarchy tree. Each node is either an internal node with
   two children, or a leaf node containing references to primitive geometries.

   Based on standard BVH construction patterns for efficient ray tracing.
*/

#include <vector>
#include "../utilities/BBox.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../world/World.hpp"

class Geometry;

class BVHNode {
private:
  BVHNode *left;      // Left child (nullptr if leaf)
  BVHNode *right;     // Right child (nullptr if leaf)
  BBox bbox;          // Bounding box of this node
  int lower;          // Index of first primitive in this node (leaf only)
  int upper;          // Index of last primitive in this node (leaf only)

public:
  // Constructors
  BVHNode();
  BVHNode(int lower_idx, int upper_idx);
  
  // Copy operations. NOT
  BVHNode(const BVHNode &other) = delete;
  BVHNode &operator=(const BVHNode &other) = delete;

  // Destructor
  virtual ~BVHNode();

  // Tree structure
  bool isLeaf() const;
  void setLeft(BVHNode *l);
  void setRight(BVHNode *r);
  BVHNode *getLeft() const;
  BVHNode *getRight() const;

  // Bounding box
  void setBBox(const BBox &b);
  BBox getBBox() const;

  // Indices (for leaves)
  int getLower() const;
  int getUpper() const;

  // Ray intersection
  bool hit(const Ray &ray, ShadeInfo &sinfo, const std::vector<Geometry *> &primitives,
           const World &world) const;
  // Any hit
  bool is_shadowed(const Ray &ray, float max_t, const std::vector<Geometry *> &primitives,
                   const World &world) const;
};

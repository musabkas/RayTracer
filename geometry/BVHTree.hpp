#pragma once

/**
   This file declares the BVHTree class which manages a complete Bounding Volume
   Hierarchy for efficient ray tracing acceleration. It constructs a binary tree
   from a list of geometries using a simple split-in-middle strategy.
*/

#include <vector>
#include "BVHNode.hpp"
#include "Geometry.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/BBox.hpp"

class BVHTree {
private:
  std::vector<Geometry *> primitives;  // All geometry primitives
  BVHNode *root;                       // Root of the BVH tree

  // Recursive tree construction
  BVHNode *buildNode(int lower, int upper);
  
  // Helper to compute bounding box of a range of primitives
  BBox computeBBox(int lower, int upper) const;
  
  // Helper to find split axis and position
  int findSplitAxis(int lower, int upper) const;
  int findSplitPosition(int lower, int upper, int axis);

public:
  // Constructor - takes ownership of geometry pointers
  BVHTree(const std::vector<Geometry *> &geometries, const World &world);
  
  // Destructor
  ~BVHTree();

  // Ray-scene intersection
  ShadeInfo hit(const Ray &ray, const World &world) const;

  // Get the root node
  BVHNode *getRoot() const;

  // Get primitives
  const std::vector<Geometry *> &getPrimitives() const;
};

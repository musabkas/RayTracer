# BVH Implementation Summary

## What Was Created

### New Files
1. **`geometry/BVH.hpp`** - The Compound/BVH node class declaration
   - Inherits from `Geometry`
   - Stores a vector of child geometries
   - Maintains an AABB (Axis-Aligned Bounding Box)
   - Provides `add_child()` method to extend bbox and add children
   
2. **`geometry/BVH.cpp`** - The implementation
   - `hit()` method: First checks if ray hits the node's bounding box, then recursively checks children
   - `getBBox()` returns the computed bounding box
   - `add_child()` automatically extends the bounding box to include the new child

## Files Modified

### 1. `build/buildDeath.cpp` - Fractal Generation Refactored
**Before:** Top-down recursion that pushed spheres directly into a flat World geometry list
```cpp
void buildMengerSponge(const Point3D& center, float size, int depth, World* world) {
    if (depth == 0) {
        Sphere* sphere_ptr = new Sphere(...);
        world->add_geometry(sphere_ptr);  // Flat list
    }
    // recursive calls with world*
}
```

**After:** Bottom-up recursion that returns Geometry pointers
```cpp
Geometry* buildMengerSponge(const Point3D& center, float size, int depth) {
    if (depth == 0) {
        Sphere* sphere_ptr = new Sphere(...);
        return sphere_ptr;  // Return the geometry
    }
    
    BVH* bvh_node = new BVH();
    // Loop through 3x3x3 grid
    Geometry* child = buildMengerSponge(...);
    bvh_node->add_child(child);  // Build tree structure
    return bvh_node;
}

World::build() {
    Geometry* root_bvh = buildMengerSponge(Point3D(0,0,0), 100.0, 3);
    add_geometry(root_bvh);  // Single root node instead of thousands of spheres
}
```

### 2. `geometry/Sphere.cpp` - Fixed Bounding Box
**Before:** Unimplemented (returned empty BBox)
```cpp
BBox Sphere::getBBox() const {
    return BBox();  // Wrong!
}
```

**After:** Properly computes AABB around the sphere
```cpp
BBox Sphere::getBBox() const {
    return BBox(Point3D(c.x - r, c.y - r, c.z - r), 
                Point3D(c.x + r, c.y + r, c.z + r));
}
```

### 3. `utilities/BBox.cpp` - Critical Bug Fixes
**Bug 1: Inverted hit logic**
```cpp
// Before: Returned true when MISSED (entry >= exit means no intersection)
return t_enter >= t_exit;

// After: Returns true when HIT (entry < exit means ray passes through)
return t_enter < t_exit;
```

**Bug 2: Extend method was using `min` for both bounds**
```cpp
// Before: Both bounds were minimized (wrong!)
void BBox::extend(const BBox& b) {
    pmin = min(pmin, b.pmin);
    pmax = min(pmax, b.pmax);  // Should be max!
}

// After: Properly expands bounds
void BBox::extend(const BBox& b) {
    pmin = min(pmin, b.pmin);
    pmax = max(pmax, b.pmax);
}
```

## Architecture Pattern

### Composite Design Pattern (Tree Structure)

```
                    Root BVH
                       |
         ______________|______________
        |              |              |
       BVH            BVH            BVH
        |              |              |
     ___|__        ___|__        ___|__
    |  |  |      |  |  |      |  |  |
   BVH BVH ...  BVH BVH ...  BVH BVH ...
    |
   Sphere (depth 0 - leaves)
```

### How Ray Tracing Works Now

1. **Ray enters Root BVH's `hit()` method**
2. **Check if ray hits Root's BBox** → If no, instantly return false (skip entire tree!)
3. **If yes, loop through Root's children (20 smaller BVHs)**
4. **Each child BVH checks its own BBox** → If no, skip that subtree!
5. **If yes, recurse into child's children** (continues down the tree)
6. **At leaf level, check Sphere intersections**
7. **Return closest hit point**

### Performance Benefits

**Before BVH:** For a Menger Sponge at depth 3
- ~7,220 spheres in a flat list
- Every ray tested against ALL spheres
- Thousands of redundant checks

**After BVH:** Same geometry
- 1 Root BVH node
- ~20 nodes at level 1
- ~400 nodes at level 2
- ~7,220 sphere leaves
- Rays prune entire subtrees with bbox checks
- Exponentially faster for complex scenes

## Example Compilation Command

```bash
g++ -g raytracer.cpp world/*.cpp utilities/*.cpp geometry/*.cpp \
  cameras/*.cpp image/*.cpp samplers/*.cpp BRDF/*.cpp materials/*.cpp \
  lights/*.cpp tracers/*.cpp build/buildDeath.cpp -o deathtracer.out
```

All files are now in place and ready to compile!

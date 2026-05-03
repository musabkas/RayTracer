#pragma once


/**
 This file declares the Material class which is an abstract class for concrete
 materials to inherit from.
 
 Courtesy Kevin Suffern.
 */

 #include "../utilities/RGBColor.hpp"
#include "../utilities/Constants.hpp"
#include <vector>

class RGBColor;
class ShadeInfo;
class Light;

class Material {
public:
  // Constructors.
  Material() = default; // does nothing.

  // Copy constuctor and assignment operator.
  Material(const Material &other) = default;
  Material &operator=(const Material &other) = default;

  // Desctructor.
  virtual ~Material() = default;

  // Get color.
  virtual RGBColor shade(const ShadeInfo &sinfo, const std::vector<Light *> &lights) const = 0;
  // Add this virtual function so the PathTracer can call it
  virtual RGBColor path_shade(ShadeInfo& sr) {
      return black; // Default returns black if not overridden
  }
};

#pragma once

/*
   high level geometry work
*/

#include <string>

class BBox;
class Material;
class Ray;
class ShadeInfo;

class Geometry {
protected:
  Material *material_ptr; // this object's material which has to be accessed by its children

public:
  // oop magic
  Geometry(); // sets material_ptr to NULL.

  // Copy constructor and assignment operator.
  Geometry(const Geometry &object) = default;
  Geometry &operator=(const Geometry &rhs) = default;

  // Destructor.
  virtual ~Geometry() = default;
  
  // do we need this? oh well
  virtual std::string to_string() const = 0;

  // get set etcetera
  Material *get_material() const;
  void set_material(Material *mPtr);

  // look for a hit
  virtual bool hit(const Ray &ray, float &t, ShadeInfo &sinfo) const = 0;

  // bounding box; that's the problem for the children just like everything else in life
  virtual BBox getBBox() const = 0;
};

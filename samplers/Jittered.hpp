#pragma once

/**
   This file declares the Jittered class which represents a jittered sampler.

   It shoots multiple rays per pixel with random offsets within the pixel 
   to reduce aliasing.

   Courtesy Kevin Suffern.
*/

#include "Sampler.hpp"

class Jittered : public Sampler {
protected:
  int num_samples; // number of samples per pixel

public:
  // Constructors.
  Jittered() = default;                                     // initializes members to NULL.
  Jittered(Camera *c_ptr, ViewPlane *v_ptr, int samples); // set members and number of samples.

  // Copy constuctor and assignment operator.
  Jittered(const Jittered &other);
  Jittered &operator=(const Jittered &other);

  // Desctructor.
  virtual ~Jittered() = default;

  // Shoot multiple rays with random jittered offsets through the pixel.
  std::vector<Ray> get_rays(int px, int py) const override;
};

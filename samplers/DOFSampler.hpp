#pragma once

#include "Sampler.hpp"
#include "../cameras/Perspective.hpp"
#include "../world/ViewPlane.hpp"
#include "../utilities/Ray.hpp"
#include <vector>

class DOFSampler : public Sampler {
public:
    int num_samples; // FIXED: Added this variable
    float focal_distance;
    float lens_radius;

    // Takes your base Camera, ViewPlane, sample count, and the two DoF parameters
    DOFSampler(Camera *c_ptr, ViewPlane *v_ptr, int samples, float f_dist, float l_radius);
    
    DOFSampler(const DOFSampler& other);
    DOFSampler& operator=(const DOFSampler& other);

    virtual std::vector<Ray> get_rays(int px, int py) const override;
};
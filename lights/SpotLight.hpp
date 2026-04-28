#include "Light.hpp"

#include "../utilities/Point3D.hpp"
#include "../utilities/Vector3D.hpp"


class SpotLight : public Light {
protected:
    Point3D pos;
    Vector3D dir;
    float theta;

public:
    SpotLight(); // (origin, dir = 0, 0, 1, theta = 20 degrees, intensity = 1, color = white)
    SpotLight(float ls, RGBColor cl, Point3D pos, Vector3D dir, float theta); // assign accordingly
    
    SpotLight(const SpotLight &other);
    SpotLight &operator=(const SpotLight &other);
    virtual ~SpotLight() = default;

    virtual Vector3D get_direction(const Point3D& p) const override;
    virtual RGBColor L(const Vector3D& d) const override;
    virtual double get_distance(const Point3D& hit_point) const override;
};

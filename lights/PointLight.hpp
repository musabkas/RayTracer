#include "Light.hpp"

#include "../utilities/Point3D.hpp"


class PointLight : public Light {
protected:
    Point3D pos;

public:
    PointLight(); // (origin, intensity = 1, color = white)
    PointLight(float ls, RGBColor cl, Point3D pos); // assign accordingly
    
    PointLight(const PointLight &other);
    PointLight &operator=(const PointLight &other);
    virtual ~PointLight() = default;

    virtual Vector3D get_direction(const Point3D& p) const override;
    virtual RGBColor L(const Vector3D& d) const override;
};

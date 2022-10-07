#pragma once

#include "BaseObject.hpp"
#include "Ray.hpp"
#include "Vector3d.hpp"

class Sphere final : public BaseObject {
    public:
        Vector3D center_ = {NAN, NAN, NAN};
        double radius_ = NAN;

        Sphere(const Vector3D& center, const double radius, const Material* material) :
        BaseObject(material),
        center_(center), radius_(radius)
        {};

        
        bool IsOnSphere(const Vector3D& point) const;

        Vector3D GetOutsideNormal(const Ray& ray) const override;
        Vector3D GetNormal(const Ray& ray) const override;
        double GetDistance(const Vector3D& point) const override;

        Ray Trace(const Ray& ray, MyColor* colorAbsorbed) const override;

        double SubstituteAt(const Vector3D& point) const;
};

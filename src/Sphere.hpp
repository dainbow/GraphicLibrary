#pragma once

#include "BaseObject.hpp"
#include "Ray.hpp"
#include "Vector3d.hpp"

class Sphere : public BaseObject {
    public:
        Vector3D center_ = {NAN, NAN, NAN};
        double radius_ = NAN;

        Sphere(const Vector3D& center, const double radius, const MyColor& color, 
        const uint32_t reflectibility, const double powerDecrease) :
        BaseObject(color, powerDecrease, reflectibility),
        center_(center), radius_(radius) {};

        bool IsOnSphere(const Vector3D& point) const;

        Vector3D GetNormal(const Ray& ray) const override;
        Ray Trace(const Ray& ray) const override;
};
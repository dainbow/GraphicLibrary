#pragma once

#include "BaseObject.hpp"
#include "Ray.hpp"
#include "Vector3d.hpp"

class Sphere : public BaseObject {
    public:
        Vector3D center_ = {NAN, NAN, NAN};
        double radius_ = NAN;

        Sphere(const Vector3D& center, const double radius, const MyColor& color, 
        const Material& material, const LightSource& light, bool isLight = 0, const double externReftCoef = 1) :
        BaseObject(color, material, light, isLight, externReftCoef),
        center_(center), radius_(radius) {};

        
        bool IsOnSphere(const Vector3D& point) const;

        Vector3D GetOutsideNormal(const Ray& ray) const override;
        Vector3D GetNormal(const Ray& ray) const override;
        double GetDistance(const Vector3D& point) const override;

        Ray Trace(const Ray& ray) const override;

        double SubstituteAt(const Vector3D& point) const;
};

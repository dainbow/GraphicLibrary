#pragma once

#include "Vector.hpp"
#include "BaseObject.hpp"
#include "Matrix.hpp"

class Plane : public BaseObject {
    public:
        Matrix plane_;
        Matrix limitations_;

        Plane(const Matrix& plane, const Matrix& limitations, const MyColor& color, 
        const Material& material, const LightSource& light, bool isLight = 0, const double externRefrCoef = 1) : 
        BaseObject(color, material, light, isLight, externRefrCoef),
        plane_(plane), limitations_(limitations) {};

        bool IsAtPlane(const Vector3D& point) const;

        Vector3D GetOutsideNormal(const Ray& ray) const override;
        Vector3D GetNormal(const Ray& ray) const override;
        double GetDistance(const Vector3D& point) const override;

        Ray Trace(const Ray& ray) const override;
};

double SubstituteAtPlane(const Vector3D& point, const Matrix& plane, const uint32_t row = 0);

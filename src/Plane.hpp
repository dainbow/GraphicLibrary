#pragma once

#include "Vector.hpp"
#include "BaseObject.hpp"
#include "Matrix.hpp"

class Plane : public BaseObject {
    public:
        Matrix plane_;
        Matrix limitations_;

        Plane(const Matrix& plane, const Matrix& limitations, const MyColor& color, 
        const uint32_t reflectibility, const double powerDecrease) : 
        BaseObject(color, powerDecrease, reflectibility),
        plane_(plane), limitations_(limitations) {};

        ~Plane();

        bool IsAtPlane(const Vector3D& point) const;

        Vector3D GetNormal(const Ray& ray) const override;
        Ray Trace(const Ray& ray) const override;
};

double SubstituteAtPlane(const Vector3D& point, const Matrix& plane, const uint32_t row = 0);

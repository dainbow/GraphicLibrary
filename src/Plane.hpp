#pragma once

#include "Vector.hpp"
#include "BaseObject.hpp"
#include "Matrix.hpp"

class Plane final : public BaseObject {
    public:
        Matrix plane_;
        Matrix limitations_;

        Plane(const Matrix& plane, const Matrix& limitations, const Material* material) : 
        BaseObject(material),
        plane_(plane), limitations_(limitations) 
        {};

        virtual void Serialize(FILE* outStream, uint32_t depth) const override;
        bool IsAtPlane(const Vector3D& point) const;

        Vector3D GetOutsideNormal(const Ray& ray) const override;
        Vector3D GetNormal(const Ray& ray) const override;
        double GetDistance(const Vector3D& point) const override;

        Ray Trace(const Ray& ray, MyColor* absorbedColor) const override;
};

double SubstituteAtPlane(const Vector3D& point, const Matrix& plane, const uint32_t row = 0);

Plane* GetXZPlane(const Material* material, const double z, const double xMin = -INFINITY, const double xMax = INFINITY, const double yMin = -INFINITY, const double yMax = INFINITY); 
Plane* GetXYPlane(const Material* material, const double z, const double xMin = -INFINITY, const double xMax = INFINITY, const double yMin = -INFINITY, const double yMax = INFINITY);
Plane* GetZYPlane(const Material* material, const double x, const double zMin = -INFINITY, const double zMax = INFINITY, const double yMin = -INFINITY, const double yMax = INFINITY);

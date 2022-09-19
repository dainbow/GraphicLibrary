#pragma once

#include "Ray.hpp"
#include "Color.hpp"

class BaseObject {
    public:
        MyColor color_ = 0;

        double powerDecrease_    = 0;
        uint32_t reflectibility_ = 0;

        BaseObject(const MyColor& color, const double powerDecrease, const uint32_t reflectibility) :
        color_(color), powerDecrease_(powerDecrease), reflectibility_(reflectibility) {};

        virtual Ray Trace(const Ray& ray) const = 0;
        virtual Vector3D GetNormal(const Ray& ray) const = 0;

        Vector3D GetReflected(const Vector3D& point, const Ray& ray) const {
            Vector3D normal = GetNormal({point, -ray.vector_});
            if (normal.IsNan()) {
                return normal;
            }
            
            normal.Normalise();
            normal.Resize(2 * normal.CosBetween(-ray.vector_));

            return ray.vector_ + normal;
        }

        virtual ~BaseObject() {};
};

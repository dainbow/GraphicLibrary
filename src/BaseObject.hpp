#pragma once

#include "LightSource.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "Color.hpp"

class BaseObject : public Material, public LightSource {
    public:
        double externRefractionCoef_ = 1;
        MyColor color_ = 0;

        bool isLight_ = 0;

        BaseObject(const MyColor& color, const Material& material, const LightSource& light, bool isLight = 0, const double externRefrCoef = 1) :
        Material(material), LightSource(light), externRefractionCoef_(externRefrCoef), color_(color), isLight_(isLight) {};

        virtual Ray Trace(const Ray& ray) const = 0;
        
        virtual Vector3D GetOutsideNormal(const Ray& ray) const = 0;
        virtual Vector3D GetNormal(const Ray& ray) const = 0;
        virtual double GetDistance(const Vector3D& point) const = 0;

        Ray GetRefracted(const Ray& ray) const;

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

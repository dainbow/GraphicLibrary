#pragma once

#include <cassert>

#include "HitInfo.hpp"
#include "LightSource.hpp"
#include "Ray.hpp"
#include "Color.hpp"
#include "Texture.hpp"

struct HitInfo;
struct Material {
    virtual Ray GetProcessedRay(const Ray& fallingRay, const HitInfo& info,
                                MyColor* colorAbsorbed) const = 0;

    virtual MyColor GetEmitted([[maybe_unused]] const Vector3D& point) const {
        return MyColor(0, 0, 0);
    }

    virtual ~Material() {};
};

class Raytracer;

class BaseObject {
    public:
        const Material* objectMaterial_;
        std::string name_;

        Raytracer* parent_;

        BaseObject(const BaseObject& object) = default;
        BaseObject& operator=(const BaseObject& object) = default;

        BaseObject(const Material* material, const std::string name = "") :
        objectMaterial_(material), name_(name), parent_(nullptr)
        {};

        virtual Ray Trace(const Ray& ray, MyColor* absorbedColor) const = 0;
        
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

struct HitInfo {
    Vector3D point_;
    
    const BaseObject* hittedObject_;
    Vector3D normal_;
};

struct Mirror : public Material {
    const Texture* albedo_;

    Mirror(const Texture* newAlbedo) : 
    albedo_(newAlbedo)
    {};

    Mirror(const Mirror& mirror):
    albedo_(mirror.albedo_)
    {};

    Mirror& operator=(const Mirror& mirror) {
        albedo_ = mirror.albedo_;

        return *this;
    }

    Ray GetProcessedRay(const Ray& fallingRay, const HitInfo& info,
                        MyColor* colorAbsorbed) const override 
    {
        Vector3D reflectedRay = info.hittedObject_->GetReflected(info.point_, fallingRay);

        *colorAbsorbed = albedo_->GetColor(info.point_);
        
        if (!reflectedRay.IsNan()) {
            return {info.point_, reflectedRay};
        }
        
        return {info.point_, {NAN, NAN, NAN}, NAN};
    };
};

struct Scattering : public Material {   
    const Texture* albedo_;

    Scattering(const Texture* newAlbedo) :
    albedo_(newAlbedo)
    {};

    Scattering(const Scattering& scatt):
    albedo_(scatt.albedo_)
    {}

    Scattering& operator=(const Scattering& scatt) {
        albedo_ = scatt.albedo_;

        return *this;
    }

    Ray GetProcessedRay([[maybe_unused]] const Ray& fallingRay, const HitInfo& info,
                        MyColor* colorAbsorbed) const override 
    {
        Vector3D normalNormal = Normalise(info.normal_);
        Vector3D scaterredVector = normalNormal + RandomNormalVector();

        if (scaterredVector.IsZero()) {
            scaterredVector = normalNormal;
        }

        *colorAbsorbed = albedo_->GetColor(info.point_);

        return {info.point_, scaterredVector};
    };
};

struct Transparent : public Material {
    double materialRefractionCoef_ = 0;
    double externRefractionCoef_   = 0;

    Transparent(double materialCoef, double externCoef = 1) :
    materialRefractionCoef_(materialCoef), externRefractionCoef_(externCoef)
    {}

    Ray GetProcessedRay(const Ray& fallingRay, const HitInfo& info,
                        MyColor* colorAbsorbed) const override 
    {
        *colorAbsorbed = {1, 1, 1};
        Vector3D normal = info.normal_;

        double fallingCos = normal.CosBetween(fallingRay.vector_);
        double fallingSin = sqrt(1 - pow(fallingCos, 2));
        
        Ray refractedRay = {info.point_, fallingRay.vector_};

        double refractedSin = fallingSin;
        if (CmpDbl(refractedSin, 0)) {
            return refractedRay;
        }

        double refrCoef = 0;
        if (fallingCos < 0) {
            fallingCos = -fallingCos;
            refrCoef   = (externRefractionCoef_ / materialRefractionCoef_);
        }
        else {
            refrCoef   = (materialRefractionCoef_ / externRefractionCoef_);
        }
        refractedSin *= refrCoef;

        if ((refractedSin > 1) || (GetShickReflectance(fallingCos, refrCoef) > GetRandomDouble())) {
            return {info.point_, info.hittedObject_->GetReflected(info.point_, fallingRay)};
        }

        double refractedCos = sqrt(1 - pow(refractedSin , 2));
        
        refractedRay.vector_.Normalise();
        double leftSin = 0;
        if (fallingSin < refractedSin) {
            leftSin = refractedSin * fallingCos - refractedCos * fallingSin;

            normal.Resize(leftSin / refractedSin);
            refractedRay.vector_ -= normal;
        }
        else {
            leftSin = fallingSin * refractedCos - fallingCos * refractedSin;

            normal.Resize(leftSin / refractedSin);
            refractedRay.vector_ -= normal;
        }
        
        return refractedRay;
    }

    private:
        double GetShickReflectance(const double fallingCos, const double refrCoef) const {
            double r0 = (1 - refrCoef) / (1 + refrCoef);
            r0 *= r0;

            return r0 + (1 - r0) * pow(1 - fallingCos, 5);
        }
};

struct DiffLight : public Material {
    const Texture* emitLight_;

    DiffLight(const Texture* text):
    emitLight_(text)
    {};

    DiffLight(const DiffLight& diff):
    emitLight_(diff.emitLight_)
    {};

    DiffLight& operator=(const DiffLight diff) {
        emitLight_ = diff.emitLight_;

        return *this;
    }

    Ray GetProcessedRay([[maybe_unused]] const Ray& fallingRay, const HitInfo& info,
                        [[maybe_unused]] MyColor* colorAbsorbed) const override 
    {
        return {info.point_, {NAN, NAN, NAN}, NAN};
    }

    virtual MyColor GetEmitted(const Vector3D& point) const override {
        return emitLight_->GetColor(point);
    }
};

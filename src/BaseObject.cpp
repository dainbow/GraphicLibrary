#include "BaseObject.hpp"

Ray BaseObject::GetRefracted(const Ray& ray) const {
    Vector3D normal = GetOutsideNormal(ray);
    normal.Normalise();

    double fallingCos = normal.CosBetween(ray.vector_);
    double fallingSin = sqrt(1 - pow(fallingCos, 2));
    
    Ray refractedRay = ray;
    refractedRay.power_ *= refractPowerDecrease_;

    double refractedSin = fallingSin;
    if (CmpDbl(refractedSin, 0)) {
        return refractedRay;
    }

    if (fallingCos < 0) {
        fallingCos = -fallingCos;
        refractedSin *= (externRefractionCoef_ / materialRefractionCoef_);
    }
    else {
        refractedSin *= (materialRefractionCoef_ / externRefractionCoef_);
    }

    if ((refractedSin > 1) || (CmpDbl(refractedSin, 1))) 
        return {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};

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


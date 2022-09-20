#include "Plane.hpp"

Plane::~Plane() {
    plane_.~Matrix();
    limitations_.~Matrix();
}

bool Plane::IsAtPlane(const Vector3D& point) const {
    bool flag = 1;

    if (!(CmpDbl(SubstituteAtPlane(point, plane_), 0))) {
        flag = 0;
    }

    for (uint32_t curLimit = 0; (curLimit < limitations_.GetRows()) && flag; curLimit++) {
        double subs = SubstituteAtPlane(point, limitations_, curLimit);
    
        if (CmpDbl(limitations_.GetElem(curLimit, 4), 0)) {
            if ((subs > 0) && !CmpDbl(subs, 0))
                flag = 0;
        }
        else {
            if ((subs < 0) && !CmpDbl(subs, 0))
                flag = 0;
        }
    }

    return flag;
}

Vector3D Plane::GetNormal(const Ray& ray) const {
    Vector3D normal = {plane_.GetElem(0, 0), plane_.GetElem(0, 1), plane_.GetElem(0, 2)};
    
    Vector3D point1 = ray.point_ + ray.vector_;
    Vector3D point2 = ray.point_ + normal;

    if ((SubstituteAtPlane(point1, plane_, 0) * SubstituteAtPlane(point2, plane_, 0)) >= 0) {
        return normal;
    }

    return -normal;
}

Ray Plane::Trace(const Ray& ray) const {
    if ((plane_.GetColumns() != 4) || (plane_.GetRows() != 1)) {
        return {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};
    }

    if (CmpDbl(plane_.GetElem(0, 0) * ray.vector_.x_+ plane_.GetElem(0, 1) * ray.vector_.y_ + plane_.GetElem(0, 2) * ray.vector_.z_, 0))
        return {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};

    double intersectionTime = - (plane_.GetElem(0, 0) * ray.point_.x_ + plane_.GetElem(0, 1) * ray.point_.y_  + plane_.GetElem(0, 2) * ray.point_.z_ + plane_.GetElem(0, 3)) 
                             /  (plane_.GetElem(0, 0) * ray.vector_.x_+ plane_.GetElem(0, 1) * ray.vector_.y_ + plane_.GetElem(0, 2) * ray.vector_.z_);
    assert(std::isfinite(intersectionTime));
    if (!CmpDbl(intersectionTime, 0) && (intersectionTime < 0)) {
        return {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};
    }

    Vector3D intersectionPoint = {ray.vector_.x_ * intersectionTime + ray.point_.x_,
                                  ray.vector_.y_ * intersectionTime + ray.point_.y_,
                                  ray.vector_.z_ * intersectionTime + ray.point_.z_};

    if (!IsAtPlane(intersectionPoint)) {
        return {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};
    }

    Ray resultRay = {intersectionPoint, GetReflected(intersectionPoint, ray), ray.power_ * powerDecrease_};
    
    return resultRay;
}

double SubstituteAtPlane(const Vector3D& point, const Matrix& plane, const uint32_t row) {
    if (row >= plane.GetRows()) {
        return NAN;
    }

    return point.x_ * plane.GetElem(row, 0) + point.y_ * plane.GetElem(row, 1) + point.z_ * plane.GetElem(row, 2) + plane.GetElem(row, 3);
}

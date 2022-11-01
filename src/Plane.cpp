#include "Plane.hpp"

void Plane::Serialize(FILE* outStream, uint32_t depth) const {
    FPutNChars(outStream, ' ', depth);

    fprintf(outStream, "{PLNN, ");
    
    plane_.Serialize(outStream, depth + 1);
    limitations_.Serialize(outStream, depth + 1);
    
    objectMaterial_->Serialize(outStream, depth + 1);

    fprintf(outStream, "}\n");
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

Vector3D Plane::GetOutsideNormal([[maybe_unused]] const Ray& ray) const {
    return {plane_.GetElem(0, 0), plane_.GetElem(0, 1), plane_.GetElem(0, 2)};
}

Vector3D Plane::GetNormal(const Ray& ray) const {
    Vector3D normal = {plane_.GetElem(0, 0), plane_.GetElem(0, 1), plane_.GetElem(0, 2)};
    
    Vector3D point1 = ray.point_ + ray.vector_;
    Vector3D point2 = ray.point_ + normal;

    // std::cout << ray.point_ << " " << normal << std::endl;

    if ((SubstituteAtPlane(point1, plane_, 0) * SubstituteAtPlane(point2, plane_, 0)) >= 0) {
        return normal;
    }

    return -normal;
}

double Plane::GetDistance([[maybe_unused]] const Vector3D& point) const {
    return 0;
}


Ray Plane::Trace(const Ray& ray, MyColor* absorbedColor) const {
    if ((plane_.GetColumns() != 4) || (plane_.GetRows() != 1)) {
        return {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};
    }

    if (CmpDbl(plane_.GetElem(0, 0) * ray.vector_.x_+ plane_.GetElem(0, 1) * ray.vector_.y_ + plane_.GetElem(0, 2) * ray.vector_.z_, 0))
        return {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};

    double intersectionTime = - (plane_.GetElem(0, 0) * ray.point_.x_ + plane_.GetElem(0, 1) * ray.point_.y_  + plane_.GetElem(0, 2) * ray.point_.z_ + plane_.GetElem(0, 3)) 
                             /  (plane_.GetElem(0, 0) * ray.vector_.x_+ plane_.GetElem(0, 1) * ray.vector_.y_ + plane_.GetElem(0, 2) * ray.vector_.z_);
   
    if (!std::isfinite(intersectionTime)) {
        std::cout << ray.point_ << ray.vector_ << ray.power_ << std::endl;
        abort();
    }

    if (!CmpDbl(intersectionTime, 0) && (intersectionTime < 0)) {
        return {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};
    }

    Vector3D intersectionPoint = {ray.vector_.x_ * intersectionTime + ray.point_.x_,
                                  ray.vector_.y_ * intersectionTime + ray.point_.y_,
                                  ray.vector_.z_ * intersectionTime + ray.point_.z_};

    if (!IsAtPlane(intersectionPoint)) {
        return {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};
    }

    HitInfo curInfo = {};
    curInfo.hittedObject_ = this;
    curInfo.normal_ = GetNormal({intersectionPoint, -ray.vector_});
    curInfo.point_ = intersectionPoint;

    return objectMaterial_->GetProcessedRay(ray, curInfo, absorbedColor);
}

double SubstituteAtPlane(const Vector3D& point, const Matrix& plane, const uint32_t row) {
    if (row >= plane.GetRows()) {
        return NAN;
    }

    return point.x_ * plane.GetElem(row, 0) + point.y_ * plane.GetElem(row, 1) + point.z_ * plane.GetElem(row, 2) + plane.GetElem(row, 3);
}

Plane* GetXZPlane(const Material* material, const double y, const double xMin, const double xMax, const double zMin, const double zMax) {
    Matrix plane1(1, 4);
    plane1.SetElem(0, 0, 0);
    plane1.SetElem(0, 1, 1);
    plane1.SetElem(0, 2, 0);
    plane1.SetElem(0, 3, -y);

    Matrix limits(4, 5);
    limits.SetElem(0, 0, 1);
    limits.SetElem(0, 1, 0);
    limits.SetElem(0, 2, 0);
    limits.SetElem(0, 3, -xMax);
    limits.SetElem(0, 4, 0);

    limits.SetElem(1, 0, 1);
    limits.SetElem(1, 1, 0);
    limits.SetElem(1, 2, 0);
    limits.SetElem(1, 3, -xMin);
    limits.SetElem(1, 4, 1);

    limits.SetElem(2, 0, 0);
    limits.SetElem(2, 1, 0);
    limits.SetElem(2, 2, 1);
    limits.SetElem(2, 3, -zMax);
    limits.SetElem(2, 4, 0);

    limits.SetElem(3, 0, 0);
    limits.SetElem(3, 1, 0);
    limits.SetElem(3, 2, 1);
    limits.SetElem(3, 3, -zMin);
    limits.SetElem(3, 4, 1);

    return new Plane(plane1, limits, material);
}

Plane* GetXYPlane(const Material* material, const double z, const double xMin, const double xMax, const double yMin, const double yMax) {
    Matrix plane1(1, 4);
    plane1.SetElem(0, 0, 0);
    plane1.SetElem(0, 1, 0);
    plane1.SetElem(0, 2, 1);
    plane1.SetElem(0, 3, -z);

    Matrix limits(4, 5);
    limits.SetElem(0, 0, 1);
    limits.SetElem(0, 1, 0);
    limits.SetElem(0, 2, 0);
    limits.SetElem(0, 3, -xMax);
    limits.SetElem(0, 4, 0);

    limits.SetElem(1, 0, 1);
    limits.SetElem(1, 1, 0);
    limits.SetElem(1, 2, 0);
    limits.SetElem(1, 3, -xMin);
    limits.SetElem(1, 4, 1);

    limits.SetElem(2, 0, 0);
    limits.SetElem(2, 1, 1);
    limits.SetElem(2, 2, 0);
    limits.SetElem(2, 3, -yMax);
    limits.SetElem(2, 4, 0);

    limits.SetElem(3, 0, 0);
    limits.SetElem(3, 1, 1);
    limits.SetElem(3, 2, 0);
    limits.SetElem(3, 3, -yMin);
    limits.SetElem(3, 4, 1);

    return new Plane(plane1, limits, material);
}

Plane* GetZYPlane(const Material* material, const double x, const double zMin, const double zMax, const double yMin, const double yMax) {
    Matrix plane1(1, 4);
    plane1.SetElem(0, 0, 1);
    plane1.SetElem(0, 1, 0);
    plane1.SetElem(0, 2, 0);
    plane1.SetElem(0, 3, -x);

    Matrix limits(4, 5);
    limits.SetElem(0, 0, 0);
    limits.SetElem(0, 1, 0);
    limits.SetElem(0, 2, 1);
    limits.SetElem(0, 3, -zMax);
    limits.SetElem(0, 4, 0);

    limits.SetElem(1, 0, 0);
    limits.SetElem(1, 1, 0);
    limits.SetElem(1, 2, 1);
    limits.SetElem(1, 3, -zMin);
    limits.SetElem(1, 4, 1);

    limits.SetElem(2, 0, 0);
    limits.SetElem(2, 1, 1);
    limits.SetElem(2, 2, 0);
    limits.SetElem(2, 3, -yMax);
    limits.SetElem(2, 4, 0);

    limits.SetElem(3, 0, 0);
    limits.SetElem(3, 1, 1);
    limits.SetElem(3, 2, 0);
    limits.SetElem(3, 3, -yMin);
    limits.SetElem(3, 4, 1);

    return new Plane(plane1, limits, material);
}


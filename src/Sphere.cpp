#include "Sphere.hpp"

bool Sphere::IsOnSphere(const Vector3D& point) const {
    if (!CmpDbl(pow(center_.x_ - point.x_, 2) + pow(center_.y_ - point.y_, 2) + pow(center_.z_ - point.z_, 2), 
                    pow(radius_, 2))) {
        return 0;
    }

    return 1;
}

Ray Sphere::Trace(const Ray& ray) const {
    // (x - x0)^2 + (y - y0)^2 + (z - z0)^2 = R^2
    // x = lt + x1
    // y = mt + y1
    // z = nt + z1
    // (lt + x1 - x0)^2 + (mt + y1 - y0)^2 + (nt + z1 - z0)^2 = R^2
    // l^2t^2 + x1^2 + x0^2 + 2lt(x1) - 2lt(x0) - 2(x1)(x0) +
    // m^2t^2 + y1^2 + y0^2 + 2mt(y1) - 2mt(y0) - 2(y1)(y0) +
    // n^2t^2 + z1^2 + z0^2 + 2nt(z1) - 2nt(z0) - 2(z1)(z0) = R^2
    // (l^2 + m^2 + n^2)t^2 + (2l(x1 - x0) + 2m(y1 - y0) + 2n(z1 - z0))t + x1^2

    double c = pow(ray.point_.x_, 2) + pow(ray.point_.y_, 2) + pow(ray.point_.z_, 2) +
               pow(center_.x_, 2) + pow(center_.y_, 2) + pow(center_.z_, 2) - pow(radius_, 2);

    double a = pow(ray.vector_.x_, 2) + pow(ray.vector_.y_, 2) + pow(ray.vector_.z_, 2);

    Vector3D deltaVector = ray.point_ - center_;
    double b = 2 * (deltaVector * ray.vector_);

    double descriminant = pow(b, 2) - 4 * a * c;
    if (descriminant <= 0) {
        return {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};
    }

    double t1 = (-b - sqrt(descriminant)) / (2 * a);

    Ray tracedRay = {};

    tracedRay.point_  = (ray.vector_ * t1) + ray.point_;
    tracedRay.vector_ = GetReflected(tracedRay.point_, ray); 
    tracedRay.power_  = ray.power_ * powerDecrease_;

    return tracedRay;
}   

Vector3D Sphere::GetNormal(const Ray& ray) const {
    if (!IsOnSphere(ray.point_)) {
        return {NAN, NAN, NAN};
    }

    Vector3D normal = {ray.point_.x_ - center_.x_, ray.point_.y_ - center_.y_, ray.point_.z_ - center_.z_};

    return normal;
}

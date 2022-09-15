#include "CoordSystem.hpp"

float CoordSystem::ConvertXToPxl(const float mathX) const {
    return x0y0_.x_ + (mathX - xyMin_.x_) * xyScale_.x_;
}

float CoordSystem::ConvertYToPxl(const float mathY) const {
    return x0y0_.y_ + (xyMax_.y_ - mathY) * xyScale_.y_;
}

float CoordSystem::ConvertXToMath(const float pxlX) const {
    return (pxlX - x0y0_.x_) / xyScale_.x_ + xyMin_.x_;
}

float CoordSystem::ConvertYToMath(const float pxlY) const {
    return xyMax_.y_ - (pxlY - x0y0_.y_) / xyScale_.y_;
}

Vector CoordSystem::ConvertVectorToMath(const Vector& curVector) const {
    return Vector(ConvertXToMath(curVector.x_), ConvertYToMath(curVector.y_));
}

Vector CoordSystem::ConvertVectorToPxl(const Vector& curVector) const {
    return Vector(ConvertXToPxl(curVector.x_), ConvertYToPxl(curVector.y_));
}

bool CoordSystem::CheckPoint(const Vector& curVector) const {
    if (curVector.x_ < xyMin_.x_)
        return 1;

    if (curVector.y_ < xyMin_.y_)
        return 1;

    if (curVector.x_ > xyMax_.x_)
        return 1;

    if (curVector.y_ > xyMax_.y_)
        return 1;

    return 0;
}

CoordSystem::CoordSystem(const Vector& x0y0, const Vector& x1y1,
                        const Vector& xyMin, const Vector& xyMax) :
            Rectangle(x0y0, x1y1),
            xyMin_(xyMin),
            xyMax_(xyMax),
            xyScale_((x1y1_.x_ - x0y0_.x_) / (xyMax.x_ - xyMin.x_), (x1y1_.y_ - x0y0_.y_) / (xyMax.y_ - xyMin.y_))
    {
}

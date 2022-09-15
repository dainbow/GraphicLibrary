#include "Rectangle.hpp"

bool Rectangle::IsPressed(const Vector& curPoint) const {
    if (curPoint.x_ < x0y0_.x_)
        return 0;

    if (curPoint.x_ > x1y1_.x_)
        return 0;

    if (curPoint.y_ < x0y0_.y_)
        return 0;

    if (curPoint.y_ > x1y1_.y_)
        return 0;

    return 1;
}

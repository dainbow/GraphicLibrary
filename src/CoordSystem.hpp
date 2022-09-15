#pragma once

#include <cstdint>

#include "Vector.hpp"
#include "Rectangle.hpp"

class Window;
const uint32_t BorderWidth = 5;

class CoordSystem : public Rectangle {
        friend Window;
    private:
        Vector xyMin_;
        Vector xyMax_;

        Vector xyScale_;    
    public:

    float ConvertXToPxl (const float mathX) const;
    float ConvertYToPxl(const float mathY) const;

    float ConvertXToMath(const float pxlX) const;
    float ConvertYToMath(const float pxlY) const;

    Vector ConvertVectorToMath(const Vector& curVector) const;
    Vector ConvertVectorToPxl(const Vector& curVector) const;

    bool CheckPoint(const Vector& curVector) const;

    CoordSystem(const Vector& x0y0, const Vector& x1y1,
                const Vector& xyMin, const Vector& xyMax);
};

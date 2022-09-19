#pragma once

#include <cmath>
#include <iostream>


#include "Utilities.hpp"

class Vector3D {
    public:
        double x_;
        double y_;
        double z_;

        double CosBetween(const Vector3D& vector2) const;

        double operator*(const Vector3D& vector2) const;

        void Resize(const double newLength);

        void operator *=(double scalar);
        void operator +=(const Vector3D& vectorToAdd);
        void operator -=(const Vector3D& vectorToSub);

        Vector3D operator*(double scalar) const;
        Vector3D operator+(const Vector3D& vectorToAdd) const;
        Vector3D operator-(const Vector3D& vectorToSub) const;

        Vector3D operator-() const;

        void Normalise();
        bool IsNan() const;

        double LengthSquared() const;
        double Length() const;
};

std::ostream& operator<<(std::ostream& outStream, const Vector3D& curVector);

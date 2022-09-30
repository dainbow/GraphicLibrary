#pragma once

#include <cmath>
#include <iostream>

#include "Color.hpp"
#include "Utilities.hpp"

class Vector3D {
    public:
        double x_;
        double y_;
        double z_;

        Vector3D(const MyColor& color) :
        x_(color.fRed_), y_(color.fGreen_), z_(color.fBlue_)
        {};

        Vector3D(const double x = 0, const double y = 0, const double z = 0):
        x_(x), y_(y), z_(z)
        {};

        double CosBetween(const Vector3D& vector2) const;
        double SinBetween(const Vector3D& vector2) const;

        double operator*(const Vector3D& vector2) const;

        void Resize(const double newLength);

        void operator *=(double scalar);

        void operator +=(const Vector3D& vectorToAdd);
        void operator +=(const MyColor& colorToAdd);

        void operator -=(const Vector3D& vectorToSub);

        Vector3D operator*(double scalar) const;
        Vector3D operator+(const Vector3D& vectorToAdd) const;
        Vector3D operator-(const Vector3D& vectorToSub) const;

        Vector3D operator-() const;

        bool operator==(const Vector3D& vectorToCmp) const;

        void Normalise();

        bool IsNan()  const;
        bool IsZero() const;

        double LengthSquared() const;
        double Length() const;
};

std::ostream& operator<<(std::ostream& outStream, const Vector3D& curVector);

Vector3D Normalise(const Vector3D& curVector);
Vector3D RandomNormalVector();

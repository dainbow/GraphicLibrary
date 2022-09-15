#pragma once

#include <cmath>
#include <iostream>

class Vector3D {
    public:
        float x_;
        float y_;
        float z_;

        float CosBetween(const Vector3D& vector2) const;

        float operator*(const Vector3D& vector2) const;

        void Resize(const float newLength);

        void operator *=(float scalar);
        void operator +=(const Vector3D& vectorToAdd);
        void operator -=(const Vector3D& vectorToSub);

        Vector3D operator*(float scalar) const;
        Vector3D operator+(const Vector3D& vectorToAdd) const;
        Vector3D operator-(const Vector3D& vectorToSub) const;

        Vector3D operator-() const;

        void Normalise();

        float LengthSquared() const;
        float Length() const;
};

std::ostream& operator<<(std::ostream& outStream, const Vector3D& curVector);

#pragma once

#include <utility>
#include <cstdint>
#include <cmath>
#include <iostream>

#include "Utilities.hpp"

const float ArrowCoef = float(0.3);
const float PerpendCoef = float(0.3);

class Vector {
    public:
        float x_;
        float y_;

        Vector(float x = 0, float y = 0) : x_(x), y_(y) {};
 
        void Rotate(const float radian);

        float LengthSquared() const;
        float Length() const;
        void Resize(const float newLength);

        void operator *=(float scalar);
        void operator +=(const Vector& vectorToAdd);
        void operator -=(const Vector& vectorToSub);

        Vector operator*(float scalar) const;
        Vector operator+(const Vector& vectorToAdd) const;
        Vector operator-(const Vector& vectorToSub) const;

        bool operator==(const Vector& vectorToCmp) const;

        Vector operator-() const;
};

std::ostream& operator<<(std::ostream& outStream, const Vector& curVector);

Vector CreatePerpendicularVector(const Vector& vector);
float GetDistanceSquared(const Vector& startPoint, const Vector& endPoint, const Vector& curPoint);

float OrientSquare(const Vector& a, const Vector& b, const Vector& c);

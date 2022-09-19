#pragma once

#include <utility>
#include <cstdint>
#include <cmath>
#include <iostream>

#include "Utilities.hpp"

const double ArrowCoef = double(0.3);
const double PerpendCoef = double(0.3);

class Vector {
    public:
        double x_;
        double y_;

        Vector(double x = 0, double y = 0) : x_(x), y_(y) {};
 
        void Rotate(const double radian);

        double LengthSquared() const;
        double Length() const;
        void Resize(const double newLength);

        void operator *=(double scalar);
        void operator +=(const Vector& vectorToAdd);
        void operator -=(const Vector& vectorToSub);

        Vector operator*(double scalar) const;
        Vector operator+(const Vector& vectorToAdd) const;
        Vector operator-(const Vector& vectorToSub) const;

        bool operator==(const Vector& vectorToCmp) const;

        Vector operator-() const;
};

std::ostream& operator<<(std::ostream& outStream, const Vector& curVector);

Vector CreatePerpendicularVector(const Vector& vector);
double GetDistanceSquared(const Vector& startPoint, const Vector& endPoint, const Vector& curPoint);

double OrientSquare(const Vector& a, const Vector& b, const Vector& c);

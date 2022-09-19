#include "Vector.hpp"

void Vector::Rotate(const double radian) {
    double oldX = x_;
    double oldY = y_;

    x_ = cos(radian) * oldX - sin(radian) * oldY;
    y_ = sin(radian) * oldX + cos(radian) * oldY;
}

double Vector::LengthSquared() const {
    return x_ * x_ + y_ * y_;
}

double Vector::Length() const {
    return sqrt(LengthSquared());
}

void Vector::Resize(const double newSize) {
    *this *= (newSize / Length());
}

void Vector::operator*=(double scalar) {
    x_ *= scalar;
    y_ *= scalar;
}

void Vector::operator+=(const Vector& vectorToAdd) {
    x_ += vectorToAdd.x_;
    y_ += vectorToAdd.y_;
}

void Vector::operator-=(const Vector& vectorToSub) {
    x_ -= vectorToSub.x_;
    y_ -= vectorToSub.y_;
}

Vector Vector::operator*(double scalar) const {
    Vector result = *this;
    result *= scalar;

    return result;
}

Vector Vector::operator+(const Vector& vectorToAdd) const {
    Vector result = *this;
    result += vectorToAdd;

    return result;
}

Vector Vector::operator-(const Vector& vectorToSub) const {
    Vector result = *this;
    result -= vectorToSub;

    return result;
}

Vector Vector::operator-() const {
    return (*this) * -1;
}

bool Vector::operator==(const Vector& vectorToCmp) const {
    return CmpDbl(x_, vectorToCmp.x_) && CmpDbl(y_, vectorToCmp.y_);
}

std::ostream& operator<<(std::ostream& outStream, const Vector& curVector) {
    return outStream << "(" << curVector.x_ << ":" << curVector.y_ << ")";
}

Vector CreatePerpendicularVector(const Vector& vector) {
    return {-vector.y_, vector.x_};
}

double GetDistanceSquared(const Vector& startPoint, const Vector& endPoint, const Vector& curPoint) {
    Vector CB = endPoint   - curPoint;
    Vector CA = startPoint - curPoint;
    Vector AB = endPoint - startPoint;

    double a = CB.LengthSquared();
    double b = CA.LengthSquared();
    double c = AB.LengthSquared();

    if (((a + c) >= b) && ((b + c) >= a)) {
        if (!CmpDbl(startPoint.x_, endPoint.x_)) {
            double kCoef = (endPoint.y_ - startPoint.y_) / (endPoint.x_ - startPoint.x_);
            double bCoef = startPoint.y_ - kCoef * startPoint.x_;

            double numerator = curPoint.y_ - kCoef * curPoint.x_ - bCoef;

            return (numerator * numerator) / (1 + kCoef * kCoef);
        }

        return (curPoint.x_ - endPoint.x_) * (curPoint.x_ - endPoint.x_);
    }

    return (a < b) ? a : b;
}

double OrientSquare(const Vector& a, const Vector& b, const Vector& c) {
    return a.x_ * (b.y_ - c.y_) + b.x_ * (c.y_ - a.y_) + c.x_ * (a.y_ - b.y_);
}

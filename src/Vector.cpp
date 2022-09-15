#include "Vector.hpp"

void Vector::Rotate(const float radian) {
    float oldX = x_;
    float oldY = y_;

    x_ = cosf(radian) * oldX - sinf(radian) * oldY;
    y_ = sinf(radian) * oldX + cosf(radian) * oldY;
}

float Vector::LengthSquared() const {
    return x_ * x_ + y_ * y_;
}

float Vector::Length() const {
    return sqrtf(LengthSquared());
}

void Vector::Resize(const float newSize) {
    *this *= (newSize / Length());
}

void Vector::operator*=(float scalar) {
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

Vector Vector::operator*(float scalar) const {
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
    return (x_ == vectorToCmp.x_) && (y_ == vectorToCmp.y_);
}

std::ostream& operator<<(std::ostream& outStream, const Vector& curVector) {
    return outStream << "(" << curVector.x_ << ":" << curVector.y_ << ")";
}

Vector CreatePerpendicularVector(const Vector& vector) {
    return {-vector.y_, vector.x_};
}

float GetDistanceSquared(const Vector& startPoint, const Vector& endPoint, const Vector& curPoint) {
    Vector CB = endPoint   - curPoint;
    Vector CA = startPoint - curPoint;
    Vector AB = endPoint - startPoint;

    float a = CB.LengthSquared();
    float b = CA.LengthSquared();
    float c = AB.LengthSquared();

    if (((a + c) >= b) && ((b + c) >= a)) {
        if (!CmpFloat(startPoint.x_, endPoint.x_)) {
            float kCoef = (endPoint.y_ - startPoint.y_) / (endPoint.x_ - startPoint.x_);
            float bCoef = startPoint.y_ - kCoef * startPoint.x_;

            float numerator = curPoint.y_ - kCoef * curPoint.x_ - bCoef;

            return (numerator * numerator) / (1 + kCoef * kCoef);
        }

        return (curPoint.x_ - endPoint.x_) * (curPoint.x_ - endPoint.x_);
    }

    return (a < b) ? a : b;
}

float OrientSquare(const Vector& a, const Vector& b, const Vector& c) {
    return a.x_ * (b.y_ - c.y_) + b.x_ * (c.y_ - a.y_) + c.x_ * (a.y_ - b.y_);
}

#include "Vector3d.hpp"

float Vector3D::operator*(const Vector3D& vector2) const {
    return x_ * vector2.x_ + y_ * vector2.y_ + z_ * vector2.z_;
}

void Vector3D::Resize(const float newSize) {
    *this *= sqrtf(newSize / Length());
}

void Vector3D::operator*=(const float scalar) {
    x_ *= scalar;
    y_ *= scalar;
    z_ *= scalar;
}

void Vector3D::operator+=(const Vector3D& vectorToAdd) {
    x_ += vectorToAdd.x_;
    y_ += vectorToAdd.y_;
    z_ += vectorToAdd.z_;
}

void Vector3D::operator-=(const Vector3D& vectorToSub) {
    x_ -= vectorToSub.x_;
    y_ -= vectorToSub.y_;
    z_ -= vectorToSub.z_;
}

Vector3D Vector3D::operator*(const float scalar) const {
    Vector3D result = *this;
    result *= scalar;

    return result;
}

Vector3D Vector3D::operator+(const Vector3D& vectorToAdd) const {
    Vector3D result = *this;
    result += vectorToAdd;

    return result;
}

Vector3D Vector3D::operator-(const Vector3D& vectorToSub) const {
    Vector3D result = *this;
    result -= vectorToSub;

    return result;
}

Vector3D Vector3D::operator-() const {
    return (*this) * -1;
}

void Vector3D::Normalise() {
    *this *= float(1) / Length();
}

float Vector3D::LengthSquared() const {
    return (*this) * (*this);
}

float Vector3D::Length() const {
    return sqrtf(LengthSquared());
}

float Vector3D::CosBetween(const Vector3D& vector2) const {
    return ((*this) * vector2) / (Length() * vector2.Length()); 
}

std::ostream& operator<<(std::ostream& outStream, const Vector3D& curVector) {
    outStream << "(" << curVector.x_ << " : " << curVector.y_ << " : " << curVector.z_ << ")";

    return outStream;
}

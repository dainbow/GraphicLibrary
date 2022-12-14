#include "Vector3d.hpp"

double Vector3D::operator*(const Vector3D& vector2) const {
    return x_ * vector2.x_ + y_ * vector2.y_ + z_ * vector2.z_;
}

void Vector3D::Resize(const double newSize) {
    *this *= newSize / Length();
}

void Vector3D::operator*=(const double scalar) {
    x_ *= scalar;
    y_ *= scalar;
    z_ *= scalar;
}

void Vector3D::operator+=(const Vector3D& vectorToAdd) {
    x_ += vectorToAdd.x_;
    y_ += vectorToAdd.y_;
    z_ += vectorToAdd.z_;
}

void Vector3D::operator+=(const MyColor& colorToAdd) {
    x_ += colorToAdd.fRed_;
    y_ += colorToAdd.fGreen_;
    z_ += colorToAdd.fBlue_;
}

void Vector3D::operator-=(const Vector3D& vectorToSub) {
    x_ -= vectorToSub.x_;
    y_ -= vectorToSub.y_;
    z_ -= vectorToSub.z_;
}

Vector3D Vector3D::operator*(const double scalar) const {
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

bool Vector3D::operator==(const Vector3D& vectorToCmp) const {
    return (CmpDbl(vectorToCmp.x_, x_)) && (CmpDbl(vectorToCmp.y_, y_)) && (CmpDbl(vectorToCmp.z_, z_));
}

void Vector3D::Normalise() {
    *this *= double(1) / Length();
}

bool Vector3D::IsNan() const {
    return std::isnan(x_) && std::isnan(y_) && std::isnan(z_);
}

bool Vector3D::IsZero() const {
    return CmpDbl(x_, 0) && CmpDbl(y_, 0) && CmpDbl(z_, 0);
}

double Vector3D::LengthSquared() const {
    return (*this) * (*this);
}

double Vector3D::Length() const {
    return sqrt(LengthSquared());
}

double Vector3D::CosBetween(const Vector3D& vector2) const {
    return ((*this) * vector2) / (Length() * vector2.Length()); 
}

double Vector3D::SinBetween(const Vector3D& vector2) const {
    return sqrt(1 - pow(CosBetween(vector2), 2));
}

std::ostream& operator<<(std::ostream& outStream, const Vector3D& curVector) {
    outStream << "(" << curVector.x_ << " : " << curVector.y_ << " : " << curVector.z_ << ")";

    return outStream;
}

Vector3D Normalise(const Vector3D& curVector) {
    Vector3D result = curVector;
    result.Normalise();

    return result;
}

Vector3D RandomNormalVector() {
    Vector3D result = {GetRandomDouble(-1, 1), 
                       GetRandomDouble(-1, 1), 
                       GetRandomDouble(-1, 1)};
    result.Normalise();

    return result;
}

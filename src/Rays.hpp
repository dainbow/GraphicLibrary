#pragma once

#include "Utilities.hpp"

#include "Matrix.hpp"
#include "Vector3d.hpp"
#include "Vector.hpp"

bool VocabCmp(const Vector& vec1, const Vector& vec2);
bool TgCmp(const Vector& vec1, const Vector& vec2);

bool CheckPointInPolygon(const Vector& intersectionPoint, Vector* points, const uint32_t pointsAmount);
bool CheckPointInPolygon3D(const Vector3D& intersectionPoint, const Vector3D* points, const uint32_t pointAmount);

Vector3D GetIntersectionSection(const Vector3D& intersectionPoint, const Matrix& planeCoefs);

Vector3D GetIntersection(const Vector3D& rayPoint, const Vector3D& rayVector, const Matrix& planeCoefs);
Vector3D GetIntersectionSection(const Vector3D& rayPoint, const Vector3D& rayVector, 
                                       const Vector3D& sphereCenter, const float radius);



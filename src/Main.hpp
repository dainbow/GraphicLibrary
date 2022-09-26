#pragma once

#include <thread>
#include <iostream>
#include <unistd.h>

#include <array>

#include "Vector.hpp"
#include "CoordSystem.hpp"
#include "Window.hpp"
#include "Vector3d.hpp"
#include "Image.hpp"

#include "BaseObject.hpp"
#include "Matrix.hpp"
#include "Sphere.hpp"

#include "LightSource.hpp"

#include "Ray.hpp"
#include "Plane.hpp"

#include "Array.hpp"

constexpr double threadsAmount = 16.0;
const double LambertInterval   = M_PI / 256;

const double LittleEpsilon = 3 * __FLT_EPSILON__;

const uint32_t MaxRecursionDepth = 10;

const float windowWidth  = 1000;
const float windowHeight = 1000; 

const float virtualWidth  = 10;
const float virtualHeight = 10;

const float displayDistance = 10;

const float specularN       = 3;
const MyColor ambient = {float(0.01), float(0.01), float(0.01)};

bool PollEvent(Window& window);

void ProcessRow(MyColor* curRow, const double row, const Vector3D& cameraCoords, 
                void* objectsVoid, const uint32_t objectsAmount);

MyColor ProcessRay(const BaseObject** objects, const uint32_t objectsAmount,
                  const Ray& curRay, const Vector3D& camCoords, const uint32_t curDepth);

Ray FindClosestObject(const BaseObject** objects, const uint32_t objectsAmount, const Ray& curRay, uint32_t* objectNumber = nullptr);

double CalcLambert(const Vector3D& normal, const Ray& lightRay);
double CalcDiffuse(const BaseObject* object, const Ray& ray, const Ray& lightRay, const Vector3D& camCoords);

MyColor CalcColor(const BaseObject** objects, const uint32_t objectsAmount,
                  const Ray& leastRay, const uint32_t leastObject, const Vector3D& camCoords);




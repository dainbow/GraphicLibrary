#pragma once

#include <iostream>
#include <unistd.h>

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

const uint32_t maxReflections = 3;

const float windowWidth  = 1000;
const float windowHeight = 1000; 

const float virtualWidth  = 10;
const float virtualHeight = 10;

const float displayDistance = 10;

const float specularN       = 3;
const MyColor ambient = {float(0.01), float(0.01), float(0.01)};

void PollEvent(Window& window);

Ray FindClosestObject(BaseObject** objects, const uint32_t objectsAmount, const Ray& curRay, uint32_t* objectNumber = nullptr);

double CalcLambert(const BaseObject* object, const Ray& ray, const Ray& lightRay);
double CalcDiffuse(const BaseObject* object, const Ray& ray, const Ray& lightRay, const Vector3D& camCoords);

MyColor CalcColor(LightSource** lights, const uint32_t lightsAmount, BaseObject** objects, const uint32_t objectsAmount,
                  const Ray& leastRay, const uint32_t leastObject, const Vector3D& camCoords);




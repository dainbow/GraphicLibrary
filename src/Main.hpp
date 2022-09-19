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

const float windowWidth  = 600;
const float windowHeight = 600; 

const float virtualWidth  = 10;
const float virtualHeight = 10;

const float displayDistance = 10;

const float specularN       = 3;
const MyColor ambient = {float(0.01), float(0.01), float(0.01)};

void PollEvent(Window& window);

MyColor CalcColor(const Ray& ray, const BaseObject* object, const Vector3D camCoords, const LightSource* light);


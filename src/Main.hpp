#pragma once

#include <iostream>
#include <unistd.h>

#include "Vector.hpp"
#include "CoordSystem.hpp"
#include "Window.hpp"
#include "Vector3d.hpp"
#include "Image.hpp"

#include "Matrix.hpp"
#include "Rays.hpp"

const float RotateCoeft = float(0.0001);
const float ArrowLength = 10;

void SphereGame();
void ArrowsGame(Window* newWindow);

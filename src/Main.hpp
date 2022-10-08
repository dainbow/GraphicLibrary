#pragma once

#include <cassert>
#include <thread>
#include <iostream>
#include <unistd.h>

#include <array>
#include <string>

#include "Widget.hpp"
#include "Window.hpp"
#include "Button.hpp"

#include "LightSource.hpp"

#include "Ray.hpp"
#include "Plane.hpp"
#include "Raytracer.hpp"

class ControlGraphics {
    public:
        Raytracer* tracer_;

        CustomButton<ControlGraphics>* lowButton_;
        CustomButton<ControlGraphics>* midButton_;
        CustomButton<ControlGraphics>* highButton_;

        ControlGraphics(const ControlGraphics& graphics)            = default;
        ControlGraphics& operator=(const ControlGraphics& graphics) = default;
};

void LowGraphicsOnClick(CustomButton<ControlGraphics>* button, const Vector& vec);
void MidGraphicsOnClick(CustomButton<ControlGraphics>* button, const Vector& vec);
void HighGraphicsOnClick(CustomButton<ControlGraphics>* button, const Vector& vec);
void AddButtonToList(CustomButton<List>* button, const Vector& vec);

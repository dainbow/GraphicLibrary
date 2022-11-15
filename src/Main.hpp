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

struct ControlGraphics {
    Raytracer* tracer_;

    CustomButton<ControlGraphics>* lowButton_;
    CustomButton<ControlGraphics>* midButton_;
    CustomButton<ControlGraphics>* highButton_;
};

void LowGraphicsOnClick(CustomButton<ControlGraphics>* button, const CordsPair& vec);
void MidGraphicsOnClick(CustomButton<ControlGraphics>* button, const CordsPair& vec);
void HighGraphicsOnClick(CustomButton<ControlGraphics>* button, const CordsPair& vec);

void AddSphereToTracer(CustomButton<Raytracer>* button, const CordsPair& vec);
void AddPlaneToTracer(CustomButton<Raytracer>* button, const CordsPair& vec);

void SwapColors(Button* button, const CordsPair& vec);
void CloseWindow(CustomButton<RealWindow>* button, const CordsPair& vec);
void SelectColor(CustomButton<booba::ApplicationContext>* button, const CordsPair& vec);

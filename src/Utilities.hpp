#pragma once

#include <ctime>
#include <cassert>
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>

bool CmpDbl(const double num1, const double num2);
double GetRandomDouble(const double min = 0, const double max = 1);
int64_t GetTimeMiliseconds();

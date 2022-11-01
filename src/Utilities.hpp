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
double ConvertSysToSys(const double value, const double min1, const double max1, const double min2, const double max2);
void FPutNChars(FILE* stream, char ch, uint32_t amount);

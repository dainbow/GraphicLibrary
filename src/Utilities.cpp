#include "Utilities.hpp"

bool CmpDbl(const double num1, const double num2) {
    return fabsf64(num1 - num2) <= __FLT_EPSILON__;
}

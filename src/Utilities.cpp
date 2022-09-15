#include "Utilities.hpp"

bool CmpFloat(const float num1, const float num2) {
    return fabs(num1 - num2) <= __FLT_EPSILON__;
}

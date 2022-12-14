#include "Utilities.hpp"

bool CmpDbl(const double num1, const double num2) {
    return fabsf64(num1 - num2) <= __FLT_EPSILON__;
}

double GetRandomDouble(const double min, const double max) {
    return min + (double(std::rand()) * (max - min)) / (double(RAND_MAX));
}

int64_t GetTimeMiliseconds() {
    auto curTime = std::chrono::system_clock::now();
    auto sinceEpoch = curTime.time_since_epoch();

    auto miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(sinceEpoch);
    return miliseconds.count(); 
}

double ConvertSysToSys(const double value, const double min1, const double max1, const double min2, const double max2) {
    double result = (value - min1) / (max1 - min1);
    
    result *= (max2 - min2);
    result += min2;

    return result;
}

void FPutNChars(FILE* stream, char ch, uint32_t amount) {
    fputc('\n', stream);
    
    while(amount--) {
        fputc(ch, stream);
    }
}

#include "Color.hpp"

std::ostream& operator<<(std::ostream& outStream, const MyColor& color) {
    outStream << "(" << (uint16_t)color.red_ << " : " << (uint16_t)color.green_ << " : " << (uint16_t)color.blue_ << ")";

    return outStream;
}

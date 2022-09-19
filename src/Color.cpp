#include "Color.hpp"

std::ostream& operator<<(std::ostream& outStream, const MyColor& color) {
    outStream << "(" << color.fRed_ << " : " << color.fGreen_ << " : " << color.fBlue_ << ")";

    return outStream;
}

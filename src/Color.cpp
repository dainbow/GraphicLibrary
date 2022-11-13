#include "Color.hpp"

std::ostream& operator<<(std::ostream& outStream, const MyColor& color) {
    outStream << "(" << (uint16_t)color.red_ << " : " << (uint16_t)color.green_ << " : " << (uint16_t)color.blue_ << ")";

    return outStream;
}

MyColor ConvertHSVToRGB(const MyHSV& hsv) {
    double H = hsv.h_;
    double S = hsv.s_;
    double V = hsv.v_;

    if ((H > 360) || (H < 0) || (S > 100) || (S < 0) || (V > 100) || (V < 0)){
        std::cerr <<"The givem HSV values are not in valid range"<< std::endl; 
        return 0;
    }

    double s = S / 100;
    double v = V / 100;
    double C = s * v;
    double X = C * (1 - std::abs(std::fmod(H / 60.0, 2) - 1));
    double m = v - C;
    double r = 0, g = 0, b = 0;

    if ((H >= 0) && (H < 60)) {
        r = C, g = X, b = 0;
    }
    else if ((H >= 60) && (H < 120)) {
        r = X, g = C, b = 0;
    }
    else if ((H >= 120) && (H < 180)) {
        r = 0, g = C, b = X;
    }
    else if ((H >= 180) && (H < 240)) {
        r = 0, g = X, b = C;
    }
    else if ((H >= 240) && (H < 300)) {
        r = X, g = 0, b = C;
    }
    else {
        r = C, g = 0, b = X;
    }
    uint8_t R = uint8_t((r + m) * 255);
    uint8_t G = uint8_t((g + m) * 255);
    uint8_t B = uint8_t((b + m) * 255);

    MyColor result;
    result.red_   = R;
    result.green_ = G;
    result.blue_  = B;

    return result;  
}

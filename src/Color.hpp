#pragma once

#include <cstdint>

class MyColor {
    public:
        uint8_t red_;
        uint8_t green_;
        uint8_t blue_;

        float fRed_;
        float fGreen_;
        float fBlue_;

        MyColor() : red_(0xFF), green_(0xFF), blue_(0xFF),
        fRed_(1), fGreen_(1), fBlue_(1) {}

        MyColor(uint8_t red, uint8_t green, uint8_t blue) : 
        red_(red), green_(green), blue_(blue), 
        fRed_(float(red) / float(0xFF)), fGreen_(float(green) / float(0xFF)), fBlue_(float(blue) / float(0xFF)) {}

        MyColor(float red, float green, float blue, float) :
        red_(uint8_t(red * float(0xFF))), green_(uint8_t(green * float(0xFF))), blue_(uint8_t(blue * float(0xFF))),
        fRed_(red), fGreen_(green), fBlue_(blue) {}

        void ClampFloats() {
            fRed_   = float(red_) / 0xff;
            fGreen_ = float(green_) / 0xff;
            fBlue_  = float(blue_) / 0xff;
        }

        void ClampUints() {
            if (fRed_ > 1)
                fRed_ = 1;

            if (fGreen_ > 1)
                fGreen_ = 1;

            if (fBlue_ > 1)
                fBlue_ = 1;

            red_   = uint8_t(fRed_   * 0xff);
            green_ = uint8_t(fGreen_ * 0xff);
            blue_  = uint8_t(fBlue_  * 0xff);
        }

        void operator*=(MyColor mulColor) {
            fRed_   *= mulColor.fRed_;
            fGreen_ *= mulColor.fGreen_;
            fBlue_  *= mulColor.fBlue_;

            ClampUints();
        }

        void operator+=(MyColor mulColor) {
            fRed_   += mulColor.fRed_;
            fGreen_ += mulColor.fGreen_;
            fBlue_  += mulColor.fBlue_;

            ClampUints();
        }

        MyColor operator*(float scalar) {
            MyColor result = *this;

            result.fRed_   *= scalar;
            result.fGreen_ *= scalar;
            result.fBlue_  *= scalar;

            result.ClampUints();
            return result;
        }

        MyColor operator*(MyColor mulColor) {
            MyColor result = *this;
            result *= mulColor;

            result.ClampUints();
            return result;
        }

        MyColor operator+(MyColor mulColor) {
            MyColor result = *this;
            result += mulColor;

            result.ClampUints();
            return result;
        }
};

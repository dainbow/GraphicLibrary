#pragma once

#include <iostream>
#include <cstdint>
#include <cmath>

class MyColor {
    public:
        uint8_t red_;
        uint8_t green_;
        uint8_t blue_;

        double fRed_;
        double fGreen_;
        double fBlue_;

        MyColor() : red_(0xFF), green_(0xFF), blue_(0xFF),
        fRed_(1), fGreen_(1), fBlue_(1) {}

        MyColor(uint32_t color) : 
        red_(uint8_t((color & 0xff000000) >> 24)), green_(uint8_t((color & 0x00ff0000) >> 16)), blue_(uint8_t((color & 0x0000ff00) >> 8)), 
        fRed_(double(red_) / double(0xFF)), fGreen_(double(green_) / double(0xFF)), fBlue_(double(blue_) / double(0xFF)) {}

        MyColor(double red, double green, double blue) :
        red_(0), green_(0), blue_(0),
        fRed_(0), fGreen_(0), fBlue_(0) 
        {
            fRed_   = (fabs(red)   > 1) ? 1 : red;
            fGreen_ = (fabs(green) > 1) ? 1 : green; 
            fBlue_  = (fabs(blue)  > 1) ? 1 : blue;

            red_   = uint8_t(red   * double(0xFF));
            green_ = uint8_t(green * double(0xFF));
            blue_  = uint8_t(blue  * double(0xFF));
        }

        void ClampFloats() {
            fRed_   = double(red_) / 0xff;
            fGreen_ = double(green_) / 0xff;
            fBlue_  = double(blue_) / 0xff;
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

        bool operator==(const MyColor& cmpColor) const {
            return (red_ == cmpColor.red_) && (green_ == cmpColor.green_) && (blue_ == cmpColor.blue_);
        }

        void operator*=(const MyColor& mulColor) {
            fRed_   *= mulColor.fRed_;
            fGreen_ *= mulColor.fGreen_;
            fBlue_  *= mulColor.fBlue_;

            ClampUints();
        }

        void operator*=(const double scalar) {
            fRed_   *= scalar;
            fGreen_ *= scalar;
            fBlue_  *= scalar;

            ClampUints();
        }

        void operator+=(const MyColor& mulColor) {
            fRed_   += mulColor.fRed_;
            fGreen_ += mulColor.fGreen_;
            fBlue_  += mulColor.fBlue_;

            ClampUints();
        }

        MyColor operator*(double scalar) const {
            MyColor result = *this;

            result.fRed_   *= scalar;
            result.fGreen_ *= scalar;
            result.fBlue_  *= scalar;

            result.ClampUints();
            return result;
        }

        MyColor operator*(const MyColor& mulColor) const {
            MyColor result = *this;
            result *= mulColor;

            return result;
        }

        MyColor operator+(MyColor mulColor) const {
            MyColor result = *this;
            result += mulColor;

            result.ClampUints();
            return result;
        }
};

std::ostream& operator<<(std::ostream& outStream, const MyColor& color);

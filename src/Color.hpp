#pragma once

#include <iostream>
#include <cstdint>
#include <cmath>

#include "Utilities.hpp"
#include "Serialize.hpp"

class MyColor : public Serializeable {
    public:
        uint8_t red_;
        uint8_t green_;
        uint8_t blue_;
        uint8_t alpha_;

        double fRed_;
        double fGreen_;
        double fBlue_;
        double fAlpha_;

        MyColor() : red_(0xFF), green_(0xFF), blue_(0xFF), alpha_(0xFF),
        fRed_(1), fGreen_(1), fBlue_(1), fAlpha_(1) {}

        MyColor(uint32_t color) : 
        red_(uint8_t((color & 0xff000000) >> 24)), green_(uint8_t((color & 0x00ff0000) >> 16)), blue_(uint8_t((color & 0x0000ff00) >> 8)), 
        alpha_(uint8_t((color & 0x000000ff))),
        fRed_(double(red_) / double(0xFF)), fGreen_(double(green_) / double(0xFF)), fBlue_(double(blue_) / double(0xFF)),
        fAlpha_(double(alpha_) / double (0xFF))
        {}

        MyColor(double red, double green, double blue, double alpha = 1) :
        red_(0), green_(0), blue_(0), alpha_(0),
        fRed_(0), fGreen_(0), fBlue_(0), fAlpha_(0)
        {
            fRed_   = (fabs(red)   > 1) ? 1 : red;
            fGreen_ = (fabs(green) > 1) ? 1 : green; 
            fBlue_  = (fabs(blue)  > 1) ? 1 : blue;
            fAlpha_ = (fabs(alpha) > 1) ? 1 : alpha;

            red_   = uint8_t(red   * double(0xFF));
            green_ = uint8_t(green * double(0xFF));
            blue_  = uint8_t(blue  * double(0xFF));
            alpha_ = uint8_t(alpha * double(0xFF));
        }

        virtual void Serialize(FILE* outStream, uint32_t depth) const override {
            FPutNChars(outStream, ' ', depth);

            fprintf(outStream, "{MCLR, %d, %d, %d}\n", red_, green_, blue_);
        }

        void ClampFloats() {
            fRed_   = double(red_) / 0xff;
            fGreen_ = double(green_) / 0xff;
            fBlue_  = double(blue_) / 0xff;
            fAlpha_ = double(alpha_) / 0xff;
        }

        void ClampUints() {
            if (fRed_ > 1)
                fRed_ = 1;

            if (fGreen_ > 1)
                fGreen_ = 1;

            if (fBlue_ > 1)
                fBlue_ = 1;

            if (fAlpha_ > 1)
                fAlpha_ = 1;

            red_   = uint8_t(fRed_   * 0xff);
            green_ = uint8_t(fGreen_ * 0xff);
            blue_  = uint8_t(fBlue_  * 0xff);
            alpha_ = uint8_t(fAlpha_ * 0xff);
        }

        operator uint32_t() const {
            return (uint32_t(red_) << 24u) + (uint32_t(green_) << 16u) + (uint32_t(blue_) << 8u) + uint32_t(alpha_);
        }

        bool operator==(const MyColor& cmpColor) const {
            return (red_ == cmpColor.red_) && (green_ == cmpColor.green_) && (blue_ == cmpColor.blue_) && (alpha_ == cmpColor.alpha_);
        }

        void operator*=(const MyColor& mulColor) {
            fRed_   *= mulColor.fRed_;
            fGreen_ *= mulColor.fGreen_;
            fBlue_  *= mulColor.fBlue_;
            fAlpha_ *= mulColor.fAlpha_;

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
            fAlpha_  = std::max(fAlpha_, mulColor.fAlpha_);

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

class MyHSV {
    public:
        double h_;
        double s_;
        double v_;
};

class Gradienter {
    private:
        double start_;
        double end_;
        double size_;

        double inc_;
    public:
        Gradienter(double start, double end, double size) :
        start_(start), end_(end), size_(size), inc_((end - start) / size)
        {}

        double GetValue() {
            return start_;
        }

        Gradienter operator++(int) {
            start_ += inc_;

            return *this;
        }

        Gradienter operator--(int) {
            start_ -= inc_;

            return *this;
        }
};

MyColor ConvertHSVToRGB(const MyHSV& hsv);
std::ostream& operator<<(std::ostream& outStream, const MyColor& color);

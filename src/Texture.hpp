#pragma once

#include "Color.hpp"
#include "Vector3d.hpp"

struct Texture {
    virtual MyColor GetColor(const Vector3D& point) const = 0;

    virtual ~Texture() {};
};

struct ConstColor : public Texture {
    MyColor color_;

    ConstColor(const MyColor& color):
    color_(color)
    {};

    MyColor GetColor([[maybe_unused]] const Vector3D& point) const override {
        return color_;
    };
};

struct Squares : public Texture {
    Squares(const Texture* oddText, const Texture* evenText, const double radius) :
    oddTexture_(oddText), evenTexture_(evenText), radius_(radius)
    {}

    Squares(const Squares& sqr) :
    oddTexture_(sqr.oddTexture_), evenTexture_(sqr.evenTexture_), radius_(sqr.radius_)
    {}

    Squares& operator=(const Squares& sqr) {
        oddTexture_  = sqr.oddTexture_;
        evenTexture_ = sqr.evenTexture_;
        radius_      = sqr.radius_;

        return *this;
    }

    MyColor GetColor(const Vector3D& point) const override {
        const double sinus = sin(radius_ * point.x_) * sin(radius_ * point.y_) * sin(radius_ * point.z_);

        if (sinus < 0) {
            return oddTexture_->GetColor(point);
        }
        else {
            return evenTexture_->GetColor(point);
        }
    };

    private:
        const Texture* oddTexture_;
        const Texture* evenTexture_;

        double radius_;
};

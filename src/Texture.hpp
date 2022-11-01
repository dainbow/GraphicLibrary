#pragma once

#include "Serialize.hpp"

#include "Color.hpp"
#include "Vector3d.hpp"

struct Texture : public Serializeable {
    virtual MyColor GetColor(const Vector3D& point) const = 0;

    virtual ~Texture() {};
};

struct ConstColor : public Texture {
    MyColor color_;

    ConstColor(const MyColor& color):
    color_(color)
    {};

    MyColor GetColor([[maybe_unused]] const Vector3D& point) const override {
        (const_cast<MyColor*>(&color_))->ClampUints();

        return color_;
    };

    virtual void Serialize(FILE* outStream, uint32_t depth) const override {
        FPutNChars(outStream, ' ', depth);

        fprintf(outStream, "{CCLR, ");

        color_.Serialize(outStream, depth + 1);

        fprintf(outStream, "}\n");
    }
};

struct Squares : public Texture {
    Squares(const Texture* oddText, const Texture* evenText, const double radius) :
    oddTexture_(oddText), evenTexture_(evenText), radius_(radius)
    {}

    Squares(const Squares& sqr) = default;
    Squares& operator=(const Squares& sqr) = default;

    virtual void Serialize(FILE* outStream, uint32_t depth) const override {
        FPutNChars(outStream, ' ', depth);
        
        fprintf(outStream, "{SQRS, "
                           "%lg, ", radius_);

        oddTexture_->Serialize(outStream, depth + 1);
        evenTexture_->Serialize(outStream, depth + 1);

        fprintf(outStream, "}\n");
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

    public:
        const Texture* oddTexture_;
        const Texture* evenTexture_;

        double radius_;
};

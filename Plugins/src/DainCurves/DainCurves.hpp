#pragma once

#include <iostream>
#include <cmath>
#include <cassert>

#include <string>

#include <algorithm>
#include <vector>

#include "../tools.hpp"

const std::string soPath        = "./Icons/";

const std::string CurvesTexture = soPath + "Curves.png";

struct CordsPair {
    double x;
    double y;

    bool operator<(const CordsPair& p2) const {
        return x < p2.x;
    }

    CordsPair operator*(const double mul) const {
        return {x * mul, y * mul};
    }

    CordsPair operator+(const CordsPair p2) const {
        return {x + p2.x, y + p2.y};
    }
};

struct HSV {
    double H;
    double S;
    double V;
};

HSV ConvertRGBToHSV(const uint32_t rgb);
uint32_t ConvertHSVToRGB(const HSV& hsv);

class AbstractTool : public booba::Tool {
    protected:
        std::string toolImage_;
        uint32_t toolIdx_;

    public:
        AbstractTool() :
        toolImage_(),
        toolIdx_(0)
        {}

        virtual ~AbstractTool() {

        }

        AbstractTool(const AbstractTool& tool) = default;
        AbstractTool& operator=(const AbstractTool& tool) = default;

        uint32_t GetIdx() {
            return toolIdx_;
        }

        virtual const char* getTexture() override {
            return toolImage_.data();
        }

        virtual void buildSetupWidget() override {}
};

class CurvesTool : public AbstractTool {
        static constexpr uint64_t PointsAmount = 200;
        static constexpr double Alpha = 0.5;

        static constexpr uint64_t CanvasWidth  = 250;
        static constexpr uint64_t CanvasHeight = 250; 
    private:
        uint64_t canvasIdx_;
        uint64_t resetButtonIdx_;
        uint64_t verticalGradCanvas_;
        uint64_t horisontalGradCanvas_;

        std::vector<CordsPair> basePoints_;
        std::vector<CordsPair> interPoints_;

        void DrawDefaultCanvas();
        double CalcNextT(const double lastT, const CordsPair& lastP, const CordsPair& nextP);

        void DoCatmullRomSpline(const CordsPair& p0, const CordsPair& p1, const CordsPair& p2, const CordsPair& p3);

        void DrawGradients();
        void ReDraw();

        void ResetBasePoints() {
            basePoints_.clear();

            basePoints_.push_back({-1, CanvasHeight + 1});
            basePoints_.push_back({CanvasWidth + 1, -1});
            basePoints_.push_back({0, CanvasHeight});
            basePoints_.push_back({CanvasWidth, 0});
        }

        double ConvertV(const double V);
    public:
        CurvesTool() : 
        AbstractTool(),
        canvasIdx_(0), resetButtonIdx_(0),
        basePoints_(), interPoints_()
        {
            toolImage_ = CurvesTexture;

            ResetBasePoints();
        }

        virtual ~CurvesTool() {}

        virtual void buildSetupWidget() override;
        virtual void apply(booba::Image* image, const booba::Event* event) override;
};

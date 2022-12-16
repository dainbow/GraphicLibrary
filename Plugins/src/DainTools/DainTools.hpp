#pragma once

#include <cmath>
#include <stack>
#include <chrono>
#include <cstdio>

#include <string>
#include <set>

#define ELPIDIFOR_STANDART_EXTENDED
#define ELPIDIFOR_HIDDEN_LAYER

#include "../tools.hpp"

const std::string soPath        = "./Icons/dain/";

const std::string LineTexture   = soPath + "Line.png";
const std::string BrushTexture  = soPath + "Brush.png";
const std::string EraserTexture = soPath + "Eraser.png";
const std::string RectTexture   = soPath + "Rect.png";
const std::string EllipsTexture = soPath + "Ellips.png";
const std::string BucketTexture = soPath + "Bucket.png";
const std::string CurvesTexture = soPath + "Curves.png";

struct CordsPair {
    int32_t x;
    int32_t y;

    bool operator<(const CordsPair& p2) {
        return x < p2.x;
    }
};

int64_t GetTimeMiliseconds();

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

class LineTool : public AbstractTool {
    protected:
        CordsPair firstClick_;

        uint64_t sizeScrollIdx_;
        int32_t lineSize_;
    public:
        LineTool() :
        AbstractTool(),
        firstClick_({-1, -1}),
        sizeScrollIdx_(0),
        lineSize_(1)
        {
            toolImage_ = LineTexture;
        }

        virtual ~LineTool() {}

        virtual void apply(booba::Image* image, const booba::Event* event) override;

        static void DrawBigLine(const CordsPair& start, const CordsPair& end, int32_t radius, booba::Image* image, const uint32_t color) {
            for (int32_t curX = std::max(0, start.x - radius); curX < std::min(int32_t(image->getW()), start.x + radius); curX++) {
                for (int32_t curY = std::max(0, start.y - radius); curY < std::min(int32_t(image->getH()), start.y + radius); curY++) {
                    if ((std::pow(curX - start.x, 2) + std::pow(curY - start.y, 2)) <= std::pow(radius, 2)) {
                        if (((end.x + curX - start.x) >= 0) && ((end.x + curX - start.x) < int32_t(image->getW())) &&
                            ((end.y + curY - start.y) >= 0) && ((end.y + curY - start .y) < int32_t(image->getH()))) {
                            DrawLine(image, {curX, curY}, {end.x + curX - start.x, end.y + curY - start.y}, color);
                        }
                    }
                }
            }
        }

        static void DrawLine(booba::Image* curImage, CordsPair first, CordsPair second, const uint32_t lineColor) {
            bool trans = 0;
            if (std::abs(first.x - second.x) < std::abs(first.y - second.y)) {
                std::swap(first.x, first.y);
                std::swap(second.x, second.y);

                trans = 1;
            }

            if (first.x > second.x) {
                std::swap(first, second);
            }

            int32_t xDiff = second.x - first.x;
            int32_t yDiff = second.y - first.y;

            int32_t dError = std::abs(yDiff) * 2;
            int32_t error  = 0;
            
            int32_t curY = first.y;
            for (int32_t curX = first.x; curX < second.x; curX++) {
                if (trans) {
                    curImage->setPixel(curY, curX, lineColor);
                }
                else {
                    curImage->setPixel(curX, curY, lineColor);
                }
                
                error += dError;
                if (error > xDiff) {
                    curY += (second.y > first.y) ? 1 : -1;
                    
                    error -= 2 * xDiff;
                }
            }
        }

        virtual void buildSetupWidget() override;
};

class BrushTool : public AbstractTool {
    protected:
        CordsPair lastPoint_;
        bool isClicked_;

        int64_t lastTime_;
        int64_t delay_;

        uint64_t sizeScrollIdx_;
        int32_t brushSize_;

        uint64_t transScrollIdx_;
        double transparency_;
    public:
        BrushTool() :
        AbstractTool(),
        lastPoint_({-1, -1}),
        isClicked_(0),
        lastTime_(0),
        delay_(10),
        sizeScrollIdx_(0),
        brushSize_(1),
        transScrollIdx_(0), transparency_(1)
        {
            toolImage_ = BrushTexture;
        }

        virtual ~BrushTool() {}

        static void DrawCircle(booba::Image* layer, const CordsPair& center, const int32_t radius, const uint32_t color1, const uint32_t color2);

        virtual void apply(booba::Image* image, const booba::Event* event) override;
        virtual void buildSetupWidget() override;
}; 

class EraserTool : public AbstractTool {
    protected:
        CordsPair lastPoint_;
        bool isClicked_;

        int64_t lastTime_;
        int64_t delay_;
    public:
        EraserTool() :
        AbstractTool(),
        lastPoint_({-1, -1}),
        isClicked_(0),
        lastTime_(0),
        delay_(10)
        {
            toolImage_ = EraserTexture;
        }

        virtual ~EraserTool() {}

        virtual void apply(booba::Image* image, const booba::Event* event) override;
}; 

class RectTool : public AbstractTool {
    protected:
        CordsPair firstClick_;

    public:
        RectTool() :
        AbstractTool(),
        firstClick_({-1, -1})
        {
            toolImage_ = RectTexture;
        }

        virtual ~RectTool() {}

        virtual void apply(booba::Image* image, const booba::Event* event) override;
};

class EllipsTool : public AbstractTool {
    protected:
        CordsPair firstClick_;

    public:
        EllipsTool() :
        AbstractTool(),
        firstClick_({-1, -1})
        {
            toolImage_ = EllipsTexture;
        }

        virtual ~EllipsTool() {}

        virtual void apply(booba::Image* image, const booba::Event* event) override;

        static void DrawEllips(booba::Image* curImage, CordsPair first, CordsPair second, const uint32_t lineColor) {
            int32_t xCenter = (first.x + second.x) / 2;
            int32_t yCenter = (first.y + second.y) / 2;

            int32_t xRadius = std::max(first.x, second.x) - xCenter;
            int32_t yRadius = std::max(first.y, second.y) - yCenter;

            int32_t x = 0, y = yRadius;

            float d1 = float((yRadius * yRadius) - (xRadius * xRadius * yRadius)) + (0.25f * float(xRadius * xRadius));
            float dX = float(2 * yRadius * yRadius * x);
            float dY = float(2 * xRadius * xRadius * y);

            while (dX < dY) {
                curImage->setPixel( x + xCenter,  y + yCenter, lineColor);
                curImage->setPixel(-x + xCenter,  y + yCenter, lineColor);
                curImage->setPixel( x + xCenter, -y + yCenter, lineColor);
                curImage->setPixel(-x + xCenter, -y + yCenter, lineColor);

                if (d1 < 0) {
                    x++;

                    dX += float(2 * yRadius * yRadius);
                    d1 += dX + float(yRadius * yRadius);
                }
                else {
                    x++;
                    y--;

                    dX += float(2 * yRadius * yRadius);
                    dY -= float(2 * xRadius * xRadius);
                    d1 += dX - dY + float(yRadius * yRadius);
                }
            }

            float d2 = (float(yRadius * yRadius) * ((float(x) + 0.5f) * (float(x) + 0.5f))) +
                        float((xRadius * xRadius) * ((y - 1) * (y - 1))) -
                        float(xRadius * xRadius * yRadius * yRadius);
            
            while (y >= 0) {
                curImage->setPixel( x + xCenter,  y + yCenter, lineColor);
                curImage->setPixel(-x + xCenter,  y + yCenter, lineColor);
                curImage->setPixel( x + xCenter, -y + yCenter, lineColor);
                curImage->setPixel(-x + xCenter, -y + yCenter, lineColor);

                if (d2 > 0) {
                    y--;

                    dY -= float(2 * xRadius * xRadius);
                    d2 += float(xRadius * xRadius) - dY;
                }
                else {
                    y--;
                    x++;

                    dX += float(2 * yRadius * yRadius);
                    dY -= float(2 * xRadius * xRadius);
                    d2 += dX - dY + float(xRadius * xRadius);
                }
            }
            
        }
};

class BucketTool : public AbstractTool {
    public:
        BucketTool() :
        AbstractTool()
        {
            toolImage_ = BucketTexture;
        }

        virtual ~BucketTool() {}

        virtual void apply(booba::Image* image, const booba::Event* event) override;

        static void Scan(int32_t leftX, int32_t rightX, int32_t curY, const uint32_t colorToErase, booba::Image* curImage, std::stack<CordsPair>& stack) {
            bool isAdded = 0;

            for (int32_t curX = leftX; curX < rightX; curX++) {
                if (!CompareColors(curImage->getPixel(curX, curY), colorToErase)) {
                    isAdded = 0;
                }
                else if (!isAdded) {
                    stack.push({curX, curY});
                    isAdded = 1;
                }
            }
        }

        static bool CompareColors(const uint32_t color1, const uint32_t color2) {
            uint8_t red1 = (color1 >> 8) & 0xff;
            uint8_t red2 = (color2 >> 8) & 0xff;

            uint8_t green1 = (color1 >> 16) & 0xff;
            uint8_t green2 = (color2 >> 16) & 0xff;

            uint8_t blue1 = (color1 >> 24) & 0xff;
            uint8_t blue2 = (color2 >> 24) & 0xff;

            const int32_t compareConst = 5;
            if ((std::abs(red1 - red2) < compareConst) && (std::abs(green1 - green2) < compareConst) && (std::abs(blue1 - blue2) < compareConst)) {
                return 1;
            }

            return 0;
        }

        static void Fill(booba::Image* curImage, const CordsPair& startPoint, const uint32_t fillColor) {
            uint32_t colorToErase = curImage->getPixel(startPoint.x, startPoint.y);
            if (fillColor == colorToErase)
                return;

            std::stack<CordsPair> stack;
            stack.push(startPoint);

            while (!stack.empty()) {
                CordsPair curPoint = stack.top();
                stack.pop();

                int32_t leftX = curPoint.x;

                while ((leftX - 1) && CompareColors(curImage->getPixel(leftX - 1, curPoint.y), colorToErase)) {
                    curImage->setPixel(leftX - 1, curPoint.y, fillColor);
                    leftX--;
                }

                while ((curPoint.x < int32_t(curImage->getW())) && CompareColors(curImage->getPixel(curPoint.x, curPoint.y), colorToErase)) {
                    curImage->setPixel(curPoint.x, curPoint.y, fillColor);

                    curPoint.x++;
                }

                if ((curPoint.y + 1) < int32_t(curImage->getH()))
                    Scan(leftX, curPoint.x, curPoint.y + 1, colorToErase, curImage, stack);

                if (curPoint.y - 1)
                    Scan(leftX, curPoint.x, curPoint.y - 1, colorToErase, curImage, stack);
            }
        }
};

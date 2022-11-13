#pragma once

#include <cmath>
#include <stack>
#include <chrono>
#include <cstdio>

#include "../tools.hpp"

const char LineTexture[]   = "./Skins/dain/Line.png";
const char BrushTexture[]  = "./Skins/dain/Brush.png";
const char EraserTexture[] = "./Skins/dain/Eraser.png";
const char RectTexture[]   = "./Skins/dain/Rect.png";
const char EllipsTexture[] = "./Skins/dain/Ellips.png";
const char BucketTexture[] = "./Skins/dain/Bucket.png";

struct CordsPair {
    int32_t x;
    int32_t y;
};

int64_t GetTimeMiliseconds();

class AbstractTool : public booba::Tool {
    protected:
        const char* toolImage_;
        uint32_t toolIdx_;

    public:
        AbstractTool() :
        toolImage_(nullptr),
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
            return toolImage_;
        }

        virtual void buildSetupWidget() override {}
};

class LineTool : public AbstractTool {
    protected:
        CordsPair firstClick_;

    public:
        LineTool() :
        AbstractTool(),
        firstClick_({-1, -1})
        {
            toolImage_ = LineTexture;
        }

        virtual ~LineTool() {}

        virtual void apply(booba::Image* image, const booba::Event* event) override;

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
                    curImage->putPixel(curY, curX, lineColor);
                }
                else {
                    curImage->putPixel(curX, curY, lineColor);
                }
                
                error += dError;
                if (error > xDiff) {
                    curY += (second.y > first.y) ? 1 : -1;
                    
                    error -= 2 * xDiff;
                }
            }
        }
};

class BrushTool : public AbstractTool {
    protected:
        CordsPair lastPoint_;
        bool isClicked_;

        int64_t lastTime_;
        int64_t delay_;
    public:
        BrushTool() :
        AbstractTool(),
        lastPoint_({-1, -1}),
        isClicked_(0),
        lastTime_(0),
        delay_(10)
        {
            toolImage_ = BrushTexture;
        }

        virtual ~BrushTool() {}

        virtual void apply(booba::Image* image, const booba::Event* event) override;
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
                curImage->putPixel( x + xCenter,  y + yCenter, lineColor);
                curImage->putPixel(-x + xCenter,  y + yCenter, lineColor);
                curImage->putPixel( x + xCenter, -y + yCenter, lineColor);
                curImage->putPixel(-x + xCenter, -y + yCenter, lineColor);

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
                curImage->putPixel( x + xCenter,  y + yCenter, lineColor);
                curImage->putPixel(-x + xCenter,  y + yCenter, lineColor);
                curImage->putPixel( x + xCenter, -y + yCenter, lineColor);
                curImage->putPixel(-x + xCenter, -y + yCenter, lineColor);

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
                if (!(curImage->getPixel(curX, curY) == colorToErase)) {
                    isAdded = 0;
                }
                else if (!isAdded) {
                    stack.push({curX, curY});
                    isAdded = 1;
                }
            }
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

                while ((leftX - 1) && (curImage->getPixel(leftX - 1, curPoint.y) == colorToErase)) {
                    curImage->putPixel(leftX - 1, curPoint.y, fillColor);
                    leftX--;
                }

                while ((curPoint.x < int32_t(curImage->getX())) && (curImage->getPixel(curPoint.x, curPoint.y) == colorToErase)) {
                    curImage->putPixel(curPoint.x, curPoint.y, fillColor);

                    curPoint.x++;
                }

                if ((curPoint.y + 1) < int32_t(curImage->getH()))
                    Scan(leftX, curPoint.x, curPoint.y + 1, colorToErase, curImage, stack);

                if (curPoint.y - 1)
                    Scan(leftX, curPoint.x, curPoint.y - 1, colorToErase, curImage, stack);
            }
        }
};

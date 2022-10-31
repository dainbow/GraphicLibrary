#pragma once

#include <SFML/Graphics.hpp>

#include <iostream>
#include <stack>

#include "Window.hpp"
#include "Button.hpp"

class ImageWindow : public Window {
    public:
        Image image_;

        ImageWindow(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        Window(x, y, width, height),
        image_(width, height)
        {

        }

        virtual void ReDraw() override {
            image_.rotation_ = 0;
            image_.Draw(widgetContainer_, {0, 0}, Vector(0, 0), uint32_t(GetWidth()), uint32_t(GetHeight()));
        }
};

class FlexImageWindow : public ImageWindow {
    private:
        double imageXMin_ = 0;
        double imageYMin_ = 0;
        
        double imageXMax_;
        double imageYMax_;

        double imageX_ = 0;
        double imageY_ = 0;

        uint32_t imageWidth_;
        uint32_t imageHeight_;

        ScrollBar* yScroll_;
        ScrollBar* xScroll_;

    public:
        FlexImageWindow(const FlexImageWindow& image) = default;
        FlexImageWindow& operator=(const FlexImageWindow& image) = default;

        FlexImageWindow(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        ImageWindow(x, y, width, height),
        imageXMin_(0), imageYMin_(0),
        imageXMax_(0), imageYMax_(0),
        imageX_(0), imageY_(0),
        imageWidth_(width), imageHeight_(height),
        yScroll_(new ScrollBar(width - defaultBarWidth, 0, defaultBarWidth, height, &imageY_, &imageYMin_, &imageYMax_)),
        xScroll_(new ScrollBar(0, height - defaultBarWidth, defaultBarWidth, height, &imageX_, &imageXMin_, &imageXMax_))
        {
            *this += yScroll_;
            
            xScroll_->SetRotation(-float(M_PI_2));
            *this += xScroll_;
        }

        double GetImageXShift() {
            return imageX_;
        }

        double GetImageYShift() {
            return imageY_;
        }

        virtual void SetParent(Widget* newParent) override {
            Window::SetParent(newParent);
            
            imageWidth_  = std::min(uint32_t(newParent->GetWidth())  - GetShiftX() - defaultBarWidth, imageWidth_);
            imageXMax_   = double(GetWidth() - imageWidth_);

            imageHeight_ = std::min(uint32_t(newParent->GetHeight()) - GetShiftY() - defaultBarWidth, imageHeight_);
            imageYMax_   = double(GetHeight() - imageHeight_);
            
            yScroll_->SetHeight(imageHeight_);
            xScroll_->SetHeight(imageWidth_);

            yScroll_->SetShifts(imageWidth_, GetShiftY());
            xScroll_->SetShifts(GetShiftX(), imageHeight_ + defaultBarWidth);
        }

        virtual void ReDraw() override {
            image_.rotation_ = 0;
            image_.Draw(widgetContainer_, {int32_t(GetShiftX()), int32_t(GetShiftY())}, Vector(imageX_, imageY_), imageWidth_, imageHeight_);
        }
};

const uint32_t defaultPromoWidth  = 100;
const uint32_t defaultPromoHeight = 100;

const uint32_t promoAmount = 6;
const int64_t defaultDelay = 100;
const float defaultPromoRotation = float((M_PI / 180.0) * 4.0);

class Promotion : public ImageWindow {
    private:
        uint32_t maxX_;
        uint32_t maxY_;

        Vector moveVector_;

        int64_t delay_;
        int64_t curTime_;

        double fltShiftX_;
        double fltShiftY_;

        Vector GetRandomAtRectangle() {
            uint32_t x = rand() % maxX_;
            uint32_t y = rand() % maxY_;

            bool choice = rand() % 2;

            if (choice) {
                if (x < (maxX_ / 2)) {
                    x = 0;
                }
                else {
                    x = maxX_;
                }
            }
            else {
                if (y < (maxY_ / 2)) {
                y = 0;
                }
                else {
                    y = maxY_;
                }
            }

            return {double(x), double(y)};
        }

        void UpdatePromo() {
            Vector startPoint = GetRandomAtRectangle();
            SetShifts(uint32_t(startPoint.x_), uint32_t(startPoint.y_));

            fltShiftX_ = startPoint.x_;
            fltShiftY_ = startPoint.y_;

            Vector endPoint   = GetRandomAtRectangle(); 

            moveVector_ = endPoint - startPoint;
            moveVector_ *= (0.1 / moveVector_.Length());

            uint32_t curPromo = rand() % promoAmount;

            std::stringstream myStream;

            myStream << "./Promos/" << curPromo << ".png";
            image_.LoadFromFile(myStream.str());
        }

        void ClickPopping(const CordsPair& coords) {
            if (IsClicked(coords)) {
                UpdatePromo();
                delay_ = GetTimeMiliseconds() + 3 * defaultDelay;
            }
        }

    public:
        Promotion() :
        ImageWindow(0, 0, defaultPromoWidth, defaultPromoHeight),
        maxX_(0), maxY_(0),
        moveVector_(0, 0),
        delay_(0),
        curTime_(0),
        fltShiftX_(0), fltShiftY_(0) 
        {
            SetRotation(defaultPromoRotation);
        }

        virtual void OnClick(const Event& curEvent) override {
            ClickPopping({curEvent.mouseButton.x, curEvent.mouseButton.y});

            ImageWindow::OnClick(curEvent);
        }

        virtual void SetParent(Widget* newParent) override {
            Window::SetParent(newParent);

            maxX_ = uint32_t(newParent->GetWidth());
            maxY_ = uint32_t(newParent->GetHeight());

            UpdatePromo();
        }

        void PromoTick() {
            curTime_ = GetTimeMiliseconds();
            if (curTime_ > delay_) {
                SetChanged();
            }
        }

        virtual void OnTick(const Event& curEvent) override {
            PromoTick();

            ImageWindow::OnTick(curEvent);
        }

        virtual void ReDraw() override {
            fltShiftX_ += moveVector_.x_;
            fltShiftY_ += moveVector_.y_;

            SetShifts(uint32_t(fltShiftX_), uint32_t(fltShiftY_));
            SetRotation(-GetRotation());

            if ((GetShiftX() > maxX_) || (GetShiftY() > maxY_)) {
                UpdatePromo();
                delay_ = curTime_ + defaultDelay;
            }

            ImageWindow::ReDraw();
        }
};

enum class ToolType {
    NoTool     = 0,
    BrushTool  = 1,
    EraserTool = 2,
    LineTool   = 3,
    RectTool   = 4,
    EllipsTool = 5,
    BucketTool = 6,
};

class Canvas;

class AbstractTool {
    protected:
        SkinIdxs toolImage_;
        ToolType toolType_;

        Canvas* parent_;
    public:
        AbstractTool(Canvas* parentPtr) :
        toolImage_(SkinIdxs::NoSkin),
        toolType_(ToolType::NoTool),
        parent_(parentPtr)
        {}

        virtual ~AbstractTool() {}

        AbstractTool(const AbstractTool& tool) = default;
        AbstractTool& operator=(const AbstractTool& tool) = default;

        virtual void Apply(const Event& curEvent) = 0;
        
        ToolType GetType() {
            return toolType_;
        }

        sf::Texture* GetToolImage() {
            return SkinManager::GetInstance("default").GetTexture(toolImage_);
        }
};

class LineTool : public AbstractTool {
    protected:
        CordsPair firstClick_;

    public:
        LineTool(Canvas* parentPtr) :
        AbstractTool(parentPtr),
        firstClick_({-1, -1})
        {
            toolImage_ = SkinIdxs::LineTool;
            toolType_  = ToolType::LineTool;
        }

        virtual ~LineTool() {}

        virtual void Apply(const Event& curTool) override;

        static void DrawLine(Image& curImage, CordsPair first, CordsPair second, const MyColor& lineColor) {
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
                    curImage.SetPixel(curY, curX, lineColor);
                }
                else {
                    curImage.SetPixel(curX, curY, lineColor);
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
        BrushTool(Canvas* parentPtr) :
        AbstractTool(parentPtr),
        lastPoint_({-1, -1}),
        isClicked_(0),
        lastTime_(0),
        delay_(10)
        {
            toolImage_ = SkinIdxs::BrushTool;
            toolType_  = ToolType::BrushTool;
        }

        virtual ~BrushTool() {}

        virtual void Apply(const Event& curTool) override;
}; 

class EraserTool : public AbstractTool {
    protected:
        CordsPair lastPoint_;
        bool isClicked_;

        int64_t lastTime_;
        int64_t delay_;
    public:
        EraserTool(Canvas* parentPtr) :
        AbstractTool(parentPtr),
        lastPoint_({-1, -1}),
        isClicked_(0),
        lastTime_(0),
        delay_(10)
        {
            toolImage_ = SkinIdxs::EraserTool;
            toolType_  = ToolType::EraserTool;
        }

        virtual ~EraserTool() {}

        virtual void Apply(const Event& curTool) override;
}; 

class RectTool : public AbstractTool {
    protected:
        CordsPair firstClick_;

    public:
        RectTool(Canvas* parentPtr) :
        AbstractTool(parentPtr),
        firstClick_({-1, -1})
        {
            toolImage_ = SkinIdxs::RectTool;
            toolType_  = ToolType::RectTool;
        }

        virtual ~RectTool() {}

        virtual void Apply(const Event& curTool) override;
};

class EllipsTool : public AbstractTool {
    protected:
        CordsPair firstClick_;

    public:
        EllipsTool(Canvas* parentPtr) :
        AbstractTool(parentPtr),
        firstClick_({-1, -1})
        {
            toolImage_ = SkinIdxs::EllipsTool;
            toolType_  = ToolType::EllipsTool;
        }

        virtual ~EllipsTool() {}

        virtual void Apply(const Event& curTool) override;

        static void DrawEllips(Image& curImage, CordsPair first, CordsPair second, const MyColor& lineColor) {
            int32_t xCenter = (first.x + second.x) / 2;
            int32_t yCenter = (first.y + second.y) / 2;

            int32_t xRadius = std::max(first.x, second.x) - xCenter;
            int32_t yRadius = std::max(first.y, second.y) - yCenter;

            int32_t x = 0, y = yRadius;

            float d1 = float((yRadius * yRadius) - (xRadius * xRadius * yRadius)) + (0.25f * float(xRadius * xRadius));
            float dX = float(2 * yRadius * yRadius * x);
            float dY = float(2 * xRadius * xRadius * y);

            while (dX < dY) {
                curImage.SetPixel( x + xCenter,  y + yCenter, lineColor);
                curImage.SetPixel(-x + xCenter,  y + yCenter, lineColor);
                curImage.SetPixel( x + xCenter, -y + yCenter, lineColor);
                curImage.SetPixel(-x + xCenter, -y + yCenter, lineColor);

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
                curImage.SetPixel( x + xCenter,  y + yCenter, lineColor);
                curImage.SetPixel(-x + xCenter,  y + yCenter, lineColor);
                curImage.SetPixel( x + xCenter, -y + yCenter, lineColor);
                curImage.SetPixel(-x + xCenter, -y + yCenter, lineColor);

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
        BucketTool(Canvas* parentPtr) :
        AbstractTool(parentPtr)
        {
            toolImage_ = SkinIdxs::BucketTool;
            toolType_  = ToolType::BucketTool;
        }

        virtual ~BucketTool() {}

        virtual void Apply(const Event& curTool) override;

        static void Scan(int32_t leftX, int32_t rightX, int32_t curY, const MyColor& colorToErase, Image& curImage, std::stack<CordsPair>& stack) {
            bool isAdded = 0;

            for (int32_t curX = leftX; curX < rightX; curX++) {
                if (!(curImage.GetPixel(curX, curY) == colorToErase)) {
                    isAdded = 0;
                }
                else if (!isAdded) {
                    stack.push({curX, curY});
                    isAdded = 1;
                }
            }
        }

        static void Fill(Image& curImage, const CordsPair& startPoint, const MyColor& fillColor) {
            MyColor colorToErase = curImage.GetPixel(startPoint.x, startPoint.y);
            if (fillColor == colorToErase)
                return;

            std::stack<CordsPair> stack;
            stack.push(startPoint);

            while (!stack.empty()) {
                CordsPair curPoint = stack.top();
                stack.pop();

                int32_t leftX = curPoint.x;

                while ((leftX - 1) && (curImage.GetPixel(leftX - 1, curPoint.y) == colorToErase)) {
                    curImage.SetPixel(leftX - 1, curPoint.y, fillColor);
                    leftX--;
                }

                while ((curPoint.x < int32_t(curImage.width_)) && (curImage.GetPixel(curPoint.x, curPoint.y) == colorToErase)) {
                    curImage.SetPixel(curPoint.x, curPoint.y, fillColor);

                    curPoint.x++;
                }

                if ((curPoint.y + 1) < int32_t(curImage.height_))
                    Scan(leftX, curPoint.x, curPoint.y + 1, colorToErase, curImage, stack);

                if (curPoint.y - 1)
                    Scan(leftX, curPoint.x, curPoint.y - 1, colorToErase, curImage, stack);
            }
        }
};

class ToolButton;

class ToolPalette : public DynamicWindow {
    private:
        ToolButton* curActive_;

    public:
        ToolPalette(uint32_t x, uint32_t y) :
        DynamicWindow(x, y),
        curActive_(nullptr)
        {}

        ToolPalette(const ToolPalette& palette)            = default;
        ToolPalette& operator=(const ToolPalette& palette) = default;

        void SetTool(ToolButton* newTool);
};

class ToolManager {
    private:
        LineTool   myLine_;
        BrushTool  myBrush_;
        EraserTool myEraser_;
        RectTool   myRect_;
        EllipsTool myElps_;
        BucketTool myBuck_;

        ToolPalette* palette_;
    public:
        ToolManager(Canvas* canvas, ToolPalette* palette);

        ToolManager(const ToolManager& manager)            = default;
        ToolManager& operator=(const ToolManager& manager) = default;

        AbstractTool* GetTool(const ToolType newTool) {
            switch (newTool) {
                case ToolType::NoTool: {
                    return nullptr;

                    break;
                }

                case ToolType::LineTool: {
                    return &myLine_;

                    break;
                }

                case ToolType::BrushTool: {
                    return &myBrush_;

                    break;
                }

                case ToolType::EraserTool: {
                    return &myEraser_;

                    break;
                }

                case ToolType::RectTool: {
                    return &myRect_;

                    break;
                }

                case ToolType::EllipsTool: {
                    return &myElps_;

                    break;
                }

                case ToolType::BucketTool: {
                    return &myBuck_;

                    break;
                }

                default:
                    assert(0 && "PIZDA");

                    break;
            }

            return nullptr;
        }
};

class Canvas : public FlexImageWindow {
    private:
        ToolType curTool_;
        AbstractTool* curToolPtr_;

        ToolManager toolManager_;
    public:
        Canvas(uint32_t x, uint32_t y, uint32_t width, uint32_t height, ToolPalette* palette) :
        FlexImageWindow(x, y, width, height),
        curTool_(ToolType::NoTool), curToolPtr_(nullptr),
        toolManager_(this, palette)
        {}

        Canvas(const Canvas& canvas) = default;
        Canvas& operator=(const Canvas& canvas) = default;

        void SelectTool(const ToolType& newTool) {
            curTool_    = newTool;
            curToolPtr_ = toolManager_.GetTool(newTool);
        }

        virtual void OnClick(const Event& curEvent) override {
            Window::OnClick(curEvent);

            if (curToolPtr_ && IsClicked({curEvent.mouseButton.x, curEvent.mouseButton.y})) {
                curToolPtr_->Apply(curEvent);
            }
        }

        virtual void OnMove(const Event& curEvent) override {
            Window::OnMove(curEvent);

            if (curToolPtr_ && IsClicked({curEvent.mouseMove.x, curEvent.mouseMove.y})) {
                curToolPtr_->Apply(curEvent);
            }
        }

        virtual void OnRelease(const Event& curEvent) override {
            Window::OnRelease(curEvent);

            if (curToolPtr_) {
                curToolPtr_->Apply(curEvent);
            }
        }

        ~Canvas() {}
    
};

const uint32_t DefaultToolButtonWidth  = 50;
const uint32_t DefaultToolButtonHeight = 50;

class ToolButton : public Button {
    private:
        Canvas*         canvas_;
        AbstractTool*   tool_;

    public:
        ToolButton(Canvas* curCanvas, AbstractTool* curTool) :
        Button(0, 0, DefaultToolButtonWidth, DefaultToolButtonHeight),
        canvas_(curCanvas), tool_(curTool)
        {
            clickAction_ = new MethodCaller(this, &ToolButton::SetMeToCanvas);
        }

        ToolButton(const ToolButton& toolB)            = default;
        ToolButton& operator=(const ToolButton& toolB) = default;

        virtual void FlagClicked([[maybe_unused]] const CordsPair& cords) override {}
        virtual void FlagReleased() override {}

        void SetMeToCanvas(const CordsPair& cords) {
            if (!IsClicked(cords)) {
                return;
            }

            canvas_->SelectTool(tool_->GetType());
            ((ToolPalette*)parent_)->SetTool(this);
        }

        virtual void ReDraw() override {
            Button::ReDraw();

            Rectangle iconRect({DefaultToolButtonWidth, DefaultToolButtonHeight, 0, 0});
            iconRect.Draw(widgetContainer_, tool_->GetToolImage());
        }
}; 

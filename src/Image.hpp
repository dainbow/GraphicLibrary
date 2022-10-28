#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

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

        virtual void OnTick(const Event& curEvent) {
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


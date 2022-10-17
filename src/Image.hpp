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

        virtual void Draw([[maybe_unused]] const int64_t& time) override {
            image_.rotation_ = GetRotation();
            image_.Draw(ptrToRealWdw_, ConvertXY(0, 0), Vector(0, 0), uint32_t(width_), uint32_t(height_));
        }
};

class FlexImageWindow : public ImageWindow {
    public:
        FlexImageWindow(const FlexImageWindow& image) = default;
        FlexImageWindow& operator= (const FlexImageWindow& image) = default;

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
            
            xScroll_->Rotate(-float(M_PI_2));
            *this += xScroll_;
        }

        virtual void SetParent(Widget* newParent) override {
            Window::SetParent(newParent);
            
            imageWidth_  = std::min(uint32_t(newParent->width_)  - shiftX_, imageWidth_);
            imageXMax_   = double(width_ - imageWidth_);

            imageHeight_ = std::min(uint32_t(newParent->height_) - shiftY_, imageHeight_);
            imageYMax_   = double(height_ - imageHeight_);
            
            std::cout << "New width and height are " << imageWidth_ << " " << imageHeight_ << std::endl;

            yScroll_->SetHeight(imageHeight_);
            xScroll_->SetHeight(imageWidth_);

            yScroll_->shiftX_ = imageWidth_;
            xScroll_->shiftY_ = imageHeight_ + defaultBarWidth;
        }

        virtual void Draw([[maybe_unused]] const int64_t& time) override {
            image_.rotation_ = GetRotation();

            image_.Draw(ptrToRealWdw_, ConvertXY(0, 0), Vector(imageX_, imageY_), imageWidth_, imageHeight_);
        }

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

            shiftX_ = uint32_t(startPoint.x_);
            shiftY_ = uint32_t(startPoint.y_);

            fltShiftX_ = double(shiftX_);
            fltShiftY_ = double(shiftY_);

            Vector endPoint   = GetRandomAtRectangle(); 

            moveVector_ = endPoint - startPoint;
            moveVector_ *= (3.0 / moveVector_.Length());

            uint32_t curPromo = rand() % promoAmount;

            std::stringstream myStream;

            myStream << "./Promos/" << curPromo << ".png";
            image_.LoadFromFile(myStream.str());
        }

        void ClickPopping(const Vector& coords) {
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
            rotation_ = defaultPromoRotation;
            onClick_.PushFront(new MethodCaller<Promotion, Vector>(this, &Promotion::ClickPopping));
        }

        virtual void SetParent(Widget* newParent) override {
            Window::SetParent(newParent);

            maxX_ = uint32_t(newParent->width_);
            maxY_ = uint32_t(newParent->height_);

            UpdatePromo();
        }

        virtual void Draw (const int64_t& time) override {
            if (time < delay_) {
                return;
            }

            fltShiftX_ += moveVector_.x_;
            fltShiftY_ += moveVector_.y_;
            shiftX_ = uint32_t(fltShiftX_);
            shiftY_ = uint32_t(fltShiftY_);

            rotation_ = -rotation_;

            if ((shiftX_ > maxX_) || (shiftY_ > maxY_)) {
                UpdatePromo();
                delay_ = time + defaultDelay;
            }

            ImageWindow::Draw(time);
        }
};


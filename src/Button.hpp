#pragma once

#include "Window.hpp"

const int64_t XTextShift = 2;
const int64_t YTextShift = 2;

class Button : public Window {
    protected:
        MyColor gradColor_;
        MyColor clickColor_;

        int64_t enterTime_;

        Text text_;

    public:
        void SetText(const sf::String& newString = "", const MyColor& newColor = 0) {
            text_ = {newString, newColor};
        }

        void ControlEnterTime(const Vector& vec) {  
            if ((!enterTime_) && IsClicked(vec)) {
                enterTime_ = GetTimeMiliseconds();
            }
            else {
                enterTime_ = 0;
            }
        }

        Button(const Button& button) = default;
        Button& operator=(const Button& button) = default;

        Button(uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
               const MyColor& backColor, const MyColor& gradColor, const MyColor& clickColor) :
        Window(x, y, width, height, backColor),
        gradColor_(gradColor), clickColor_(clickColor),
        enterTime_(0),
        text_()
        {   
            onMove_    += new MethodCaller<Button, Vector> (this, &Button::ControlEnterTime);
        }

        virtual void Draw ([[maybe_unused]] const int64_t& time) override {
            Vector newXY = ConvertXY(0, 0);

            Rectangle realWindowRect({width_, height_, int64_t(newXY.x_), int64_t(newXY.y_)});

            MyColor curColor = 0;
            if (isClicked_) {
                curColor = clickColor_;
            }
            else {
                curColor = backColor_;
            }

            realWindowRect.Draw(ptrToRealWdw_, curColor);  

            if (text_.GetSize()) {
                Vector textXY = ConvertXY(XTextShift, YTextShift);
                text_.Draw(ptrToRealWdw_, textXY, width_, height_);
            }
        }
};

class DropList : public Button {
    protected:
        virtual void FlagReleased([[maybe_unused]] const int64_t& time) override {
            isHolded_ = 0;
        }

        virtual void FlagClicked(const Vector& coords) override {
            if (IsClicked(coords)) {
                isClicked_ ^= 1;
            }
            else if ((!list_->IsClicked(coords)) && isClicked_) {
                isClicked_ = 0;
            }
        }

        void ConditionalClick(const Vector& vec) {
            if (isClicked_) {
                list_->onClick_(vec);
            }
        }

        void ConditionalTick(const int64_t& time) {
            if (isClicked_) {
                list_->onTick_(time);
            }
        }

        void ConditionalMove(const Vector& vec) {
            if (isClicked_) {
                list_->onMove_(vec);
            }
        }

        void ConditionalRelease(const int64_t& time) {
            if (isClicked_) {
                list_->onRelease_(time);
            }
        }

    public:
        DynamicWindow* list_;

        DropList(const DropList& dropList)            = default;
        DropList& operator=(const DropList& dropList) = default;

        DropList(uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
                 const MyColor& backColor, const MyColor& gradColor, const MyColor& clickColor) :
        Button(x, y, width, height, backColor, gradColor, clickColor),
        list_(new DynamicWindow(x, y + uint32_t(height_), clickColor))
        {
            onTick_    += new MethodCaller<DropList, int64_t>(this, &DropList::ConditionalTick);
            onMove_    += new MethodCaller<DropList, Vector>(this, &DropList::ConditionalMove);
            onClick_   += new MethodCaller<DropList, Vector>(this, &DropList::ConditionalClick);
            onRelease_ += new MethodCaller<DropList, int64_t>(this, &DropList::ConditionalRelease);
        }

        virtual void SetParent(Widget* parent) override {
            parent_ = parent;
            list_->SetParent(parent);
        }

        virtual void SetRealWindowPtr(sf::RenderWindow* realPtr) override {
            ptrToRealWdw_ = realPtr;
            list_->SetRealWindowPtr(realPtr);
        }

        void operator+=(Widget* windowAdd) override {
            *list_ += windowAdd;
        }
};

template <class T>
class CustomButton : public Button {
    protected:
        virtual void FlagReleased([[maybe_unused]] const int64_t& time) override {
            isHolded_ = 0;
        }
        virtual void FlagClicked([[maybe_unused]] const Vector& coords) override {

        }

        T* context_;
    public:
        CustomButton(const CustomButton<T>& anotherButton)               = default;
        CustomButton<T>& operator=(const CustomButton<T>& anotherButton) = default;

        CustomButton(uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
               const MyColor& backColor, const MyColor& gradColor, const MyColor& clickColor, T* context) :
        Button(x, y, width, height, backColor, gradColor, clickColor),
        context_(context)
        {}

        T* GetContext() {
            return context_;
        }
};

const uint32_t defaultArrowsHeight = 10;
const uint32_t defaultBarHeight    = 20;

class ScrollBar : public Window {
    private:
        double* valueToCtrl_;

        double* minValue_;
        double* maxValue_;

        double scale_;

        Button* arrowUp_;
        Button* valueBut_;
        Button* arrowDown_;

        void UpdateBarPosition([[maybe_unused]] const int64_t& time) {
            if (CmpDbl((*maxValue_ - *minValue_), 0)) {
                valueBut_->shiftY_ = defaultArrowsHeight;

                return;
            }

            double newShift = ConvertSysToSys(*valueToCtrl_, *minValue_, *maxValue_, double(defaultArrowsHeight), double(height_ - defaultArrowsHeight));
            valueBut_->shiftY_ = uint32_t(newShift);
        }

        void ArrowUp(const Vector& cords) {
            if (arrowUp_->IsClicked(cords)) {
                if (((*valueToCtrl_ - scale_) > *minValue_) || CmpDbl(*valueToCtrl_ - scale_, *minValue_)) {
                    *valueToCtrl_ -= scale_;
                }
                else {
                    *valueToCtrl_ = *minValue_;
                }
            }
        }

        void ArrowDown(const Vector& cords) {
            if (arrowDown_->IsClicked(cords)) {
                double realLength = ConvertSysToSys(double(defaultArrowsHeight + valueBut_->height_), double(defaultArrowsHeight), double(height_ - defaultArrowsHeight), *minValue_, *maxValue_);

                if (((*valueToCtrl_ + realLength + scale_) < *maxValue_) || CmpDbl(*valueToCtrl_ + scale_ + realLength, *maxValue_)) {
                    *valueToCtrl_ += scale_;
                }
                else {
                    *valueToCtrl_ = *maxValue_ - realLength;
                }
            }
        }

        void ClickAtBar(const Vector& cords) {
            if (IsClicked(cords) && !arrowDown_->IsClicked(cords) && !arrowUp_->IsClicked(cords) && !valueBut_->IsClicked(cords)) {
                Vector newCords = ConvertRealXY(int64_t(cords.x_), int64_t(cords.y_));

                double newValue = ConvertSysToSys(newCords.y_, double(defaultArrowsHeight), double(height_ - defaultArrowsHeight), *minValue_, *maxValue_);
                double realLength = ConvertSysToSys(double(defaultArrowsHeight + valueBut_->height_), double(defaultArrowsHeight), double(height_ - defaultArrowsHeight), *minValue_, *maxValue_);
                
                if ((newValue <= *valueToCtrl_ ) && (newValue >= *minValue_)) {
                    *valueToCtrl_ = newValue;
                }
                else if (((newValue - realLength) >= *valueToCtrl_) && (newValue <= *maxValue_)) {
                    *valueToCtrl_ = newValue - realLength;
                }
            }
        }

    public:
        void SetButHigh(const uint32_t value) {
            valueBut_->height_ = value;
        }

        int64_t GetButHigh() {
            return valueBut_->height_;
        }

        ScrollBar(const ScrollBar& bar) = default;
        ScrollBar& operator=(const ScrollBar& bar) = default;

        void RecalcScale() {
            scale_ = (*maxValue_ - *minValue_) / double(height_ - 2 * defaultArrowsHeight - valueBut_->height_);
        }

        ScrollBar(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const MyColor& color, double* valueToCtrl, double* minValue, double* maxValue) :
        Window(x, y, width, height, color),
        valueToCtrl_(valueToCtrl), 
        minValue_(minValue), maxValue_(maxValue),
        scale_(0),
        arrowUp_(new Button(0, 0, width, defaultArrowsHeight, 0, 0, 0x00FF0000)),
        valueBut_(new Button(0, defaultArrowsHeight, width, defaultBarHeight, 0, 0, 0xFF000000)),
        arrowDown_(new Button(0, height - defaultArrowsHeight, width, defaultArrowsHeight, 0, 0, 0x00FF0000))
        {
            onClick_ += new MethodCaller<ScrollBar, Vector>(this, &ScrollBar::ClickAtBar);

            arrowUp_->onClick_ += new MethodCaller<ScrollBar, Vector>(this, &ScrollBar::ArrowUp);
            manager_ += arrowUp_;

            valueBut_->onTick_.PushFront(new MethodCaller<ScrollBar, int64_t>(this, &ScrollBar::UpdateBarPosition));
            manager_ += valueBut_;

            arrowDown_->onClick_ += new MethodCaller<ScrollBar, Vector>(this, &ScrollBar::ArrowDown);
            manager_ += arrowDown_;
        }

        virtual void SetParent(Widget* parent) override {
            parent_ = parent;
            
            arrowUp_->SetParent(this);
            valueBut_->SetParent(this);
            arrowDown_->SetParent(this);
        }

        virtual void SetRealWindowPtr(sf::RenderWindow* realPtr) override {
            ptrToRealWdw_ = realPtr;
            
            arrowUp_->SetRealWindowPtr(realPtr);
            valueBut_->SetRealWindowPtr(realPtr);
            arrowDown_->SetRealWindowPtr(realPtr);
        }
};

const uint32_t defaultBarWidth = 20;

class List : public LimitedWindow {
    private:
        ScrollBar* scrollBar_;

        void TickScroll(const int64_t& time) {
            scrollBar_->onTick_(time);
        }

        void ClickScroll(const Vector& vec) {
            scrollBar_->onClick_(vec);
        }

        void MoveScroll(const Vector& vec) {
            scrollBar_->onMove_(vec);
        }

        void ReleaseScroll(const int64_t& time) {
            scrollBar_->onRelease_(time);
        }

    public: 
        List(const List& list) = default;
        List& operator=(const List& list) = default;

        List(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const MyColor& color, const MyColor& barColor) :
        LimitedWindow(x, y, width, height, color),
        scrollBar_(new ScrollBar(width - defaultBarWidth, 0, defaultBarWidth, height, barColor, &curPointer_, &minHeight_, &maxHeight_))
        {
            onTick_    += new MethodCaller<List, int64_t>(this, &List::TickScroll);
            onClick_   += new MethodCaller<List, Vector>(this, &List::ClickScroll);
            onMove_    += new MethodCaller<List, Vector>(this, &List::MoveScroll);
            onRelease_ += new MethodCaller<List, int64_t>(this, &List::ReleaseScroll);
        }

        virtual void operator+=(Widget* newWidget) override {
            LimitedWindow::operator+=(newWidget);

            scrollBar_->RecalcScale();
            
            double visibleCoef = double(height_) / double(maxHeight_);
            visibleCoef = (visibleCoef > 1) ? 1 : visibleCoef;

            scrollBar_->SetButHigh(uint32_t(visibleCoef * double(height_ - 2 * defaultArrowsHeight)));
            if ((int64_t(curPointer_) + scrollBar_->GetButHigh()) > (height_ - int64_t(defaultArrowsHeight))) {
                curPointer_ = height_ - defaultArrowsHeight - scrollBar_->GetButHigh();
            }
        }

        virtual void SetParent(Widget* parent) override {
            parent_ = parent;
            
            scrollBar_->SetParent(this);
        }

        virtual void SetRealWindowPtr(sf::RenderWindow* realPtr) override {
            ptrToRealWdw_ = realPtr;
            
            scrollBar_->SetRealWindowPtr(realPtr);
        }
};

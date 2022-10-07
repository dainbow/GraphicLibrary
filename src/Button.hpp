#pragma once

#include "Window.hpp"

const int64_t XTextShift = 2;
const int64_t YTextShift = 2;

class Button : public Window {
    protected:
        MyColor gradColor_;
        MyColor clickColor_;

        int64_t enterTime_;
        bool isPressed_;

        Text text_;

        virtual void MouseRelease([[maybe_unused]] const int64_t& time) {
            isPressed_ = 0;
        }

        virtual void MouseClick(const Vector& coords) {
            if (IsClicked(coords)) {
                isPressed_ = 1;
            }
        }

    public:
        void SetText(const sf::String& newString = "", const MyColor& newColor = 0) {
            text_ = {newString, newColor};
        }

        void SetPressed(bool state) {
            isPressed_ = state;
        }

        bool GetPressed() {
            return isPressed_;
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
        enterTime_(0), isPressed_(0),
        text_()
        {   
            onClick_   += new MethodCaller<Button, Vector> (this, &Button::MouseClick);
            onRelease_ += new MethodCaller<Button, int64_t>(this, &Button::MouseRelease);
            onMove_    += new MethodCaller<Button, Vector> (this, &Button::ControlEnterTime);
        }

        virtual void Draw ([[maybe_unused]] const int64_t& time) override {
            Vector newXY = ConvertXY(0, 0);

            Rectangle realWindowRect({width_, height_, int64_t(newXY.x_), int64_t(newXY.y_)});

            MyColor curColor = 0;
            if (isPressed_) {
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
        virtual void MouseRelease([[maybe_unused]] const int64_t& time) override {}

        virtual void MouseClick(const Vector& coords) override {
            if (IsClicked(coords)) {
                isPressed_ ^= 1;
            }
            else if ((!list_->IsClicked(coords)) && isPressed_) {
                isPressed_ = 0;
            }
        }

        void ConditionalClick(const Vector& vec) {
            if (isPressed_) {
                list_->onClick_(vec);
            }
        }

        void ConditionalTick(const int64_t& time) {
            if (isPressed_) {
                list_->onTick_(time);
            }
        }

        void ConditionalMove(const Vector& vec) {
            if (isPressed_) {
                list_->onMove_(vec);
            }
        }

        void ConditionalRelease(const int64_t& time) {
            if (isPressed_) {
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

        void operator+=(Window* windowAdd) {
            *list_ += windowAdd;
        }
};

template <class T>
class CustomButton : public Button {
    protected:
        virtual void MouseRelease([[maybe_unused]] const int64_t& time) override {}
        virtual void MouseClick([[maybe_unused]] const Vector& coords) override {}

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

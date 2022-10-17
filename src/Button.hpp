#pragma once

#include <sstream>

#include "Window.hpp"

const int64_t XTextShift = 2;
const int64_t YTextShift = 2;

class Button : public Window {
    protected:
        bool isHovered_;

        Text text_;

        virtual void MoveHover(const Vector& cords) {
            if (IsClicked(cords)) {
                isHovered_ = 1;
            }
            else {
                isHovered_ = 0;
            }
        }

    public:
        SkinIdxs hoveredSkin_;
        SkinIdxs clickedSkin_;

        void SetText(const sf::String& newString = "", const MyColor& newColor = 0) {
            text_ = {newString, newColor};
        }

        Button(const Button& button) = default;
        Button& operator=(const Button& button) = default;

        Button(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        Window(x, y, width, height),
        isHovered_(0),
        text_(),
        hoveredSkin_(SkinIdxs::ButtonHovering), clickedSkin_(SkinIdxs::ButtonPressed)
        {   
            widgetSkin_ = SkinIdxs::ButtonNotPressed;
            onMove_    += new MethodCaller<Button, Vector> (this, &Button::MoveHover);
        }

        virtual void Draw ([[maybe_unused]] const int64_t& time) override {
            Vector newXY = ConvertXY(0, 0);

            Rectangle realWindowRect({width_, height_, int64_t(newXY.x_), int64_t(newXY.y_), GetRotation()});

            sf::Texture* curTexture = nullptr;
            if (isClicked_) {
                curTexture = skinManager_->GetTexture(clickedSkin_);
            }
            else {
                if (isHovered_) {
                    curTexture = skinManager_->GetTexture(hoveredSkin_);
                }
                else {
                    curTexture = skinManager_->GetTexture(widgetSkin_);
                }
            }

            realWindowRect.Draw(ptrToRealWdw_, curTexture);  

            if (text_.GetSize()) {
                Vector textXY = ConvertXY(XTextShift, YTextShift);
                text_.rotation_ = GetRotation();

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

        void ConditionalKey (const sf::Keyboard::Key& key) {
            if (isClicked_) {
                list_->onKeyboard_(key);
            }
        }

    public:
        DynamicWindow* list_;

        DropList(const DropList& dropList)            = default;
        DropList& operator=(const DropList& dropList) = default;

        DropList(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        Button(x, y, width, height),
        list_(new DynamicWindow(x, y + uint32_t(height_)))
        {
            onTick_     += new MethodCaller<DropList, int64_t>(this, &DropList::ConditionalTick);
            onMove_     += new MethodCaller<DropList, Vector>(this, &DropList::ConditionalMove);
            onClick_    += new MethodCaller<DropList, Vector>(this, &DropList::ConditionalClick);
            onRelease_  += new MethodCaller<DropList, int64_t>(this, &DropList::ConditionalRelease);
            onKeyboard_ += new MethodCaller<DropList, sf::Keyboard::Key>(this, &DropList::ConditionalKey);
        }

        virtual void SetParent(Widget* parent) override {
            parent_ = parent;
            list_->SetParent(parent);
        }

        virtual void SetRealWindowPtr(sf::RenderWindow* realPtr) override {
            ptrToRealWdw_ = realPtr;
            list_->SetRealWindowPtr(realPtr);
        }

        virtual void SetSkinManager(SkinManager* skinManager) override {
            skinManager_ = skinManager;
            list_->SetSkinManager(skinManager);
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

        CustomButton(uint32_t x, uint32_t y, uint32_t width, uint32_t height, T* context) :
        Button(x, y, width, height),
        context_(context)
        {
            hoveredSkin_ = SkinIdxs::ButtonNotPressed;
        }

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
            RecalcScale();

            if (arrowUp_->IsClicked(cords)) {
                if (((*valueToCtrl_ - scale_) > *minValue_) || CmpDbl(*valueToCtrl_ - scale_, *minValue_)) {
                    std::cout << scale_ << std::endl;
                    *valueToCtrl_ -= scale_;

                    std::cout << *valueToCtrl_ << std::endl;
                }
                else {
                    *valueToCtrl_ = *minValue_;
                }
            }
        }

        void ArrowDown(const Vector& cords) {
            RecalcScale();

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

        void MoveTo(const Vector& cords) {
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

        void ClickAtBar(const Vector& cords) {
            if (IsClicked(cords) && !arrowDown_->IsClicked(cords) && !arrowUp_->IsClicked(cords) && !valueBut_->IsClicked(cords)) {
                MoveTo(cords);
            }
        }

        void MoveAtBar(const Vector& cords) {
            if (valueBut_->isClicked_) {
                MoveTo(cords);
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

        ScrollBar(uint32_t x, uint32_t y, uint32_t width, uint32_t height, double* valueToCtrl, double* minValue, double* maxValue) :
        Window(x, y, width, height),
        valueToCtrl_(valueToCtrl), 
        minValue_(minValue), maxValue_(maxValue),
        scale_(0),
        arrowUp_(new Button(0, 0, width, defaultArrowsHeight)),
        valueBut_(new Button(0, defaultArrowsHeight, width, defaultBarHeight)),
        arrowDown_(new Button(0, height - defaultArrowsHeight, width, defaultArrowsHeight))
        {
            onMove_ += new MethodCaller<ScrollBar, Vector>(this, &ScrollBar::MoveAtBar);

            widgetSkin_ = SkinIdxs::ScrollBackground;
            onClick_ += new MethodCaller<ScrollBar, Vector>(this, &ScrollBar::ClickAtBar);

            arrowUp_->onClick_ += new MethodCaller<ScrollBar, Vector>(this, &ScrollBar::ArrowUp);
            arrowUp_->widgetSkin_ = SkinIdxs::ScrollUp;
            manager_ += arrowUp_;

            valueBut_->onTick_.PushFront(new MethodCaller<ScrollBar, int64_t>(this, &ScrollBar::UpdateBarPosition));
            valueBut_->widgetSkin_ = SkinIdxs::ScrollMiddle;
            manager_ += valueBut_;

            arrowDown_->onClick_ += new MethodCaller<ScrollBar, Vector>(this, &ScrollBar::ArrowDown);
            arrowDown_->widgetSkin_ = SkinIdxs::ScrollDown;
            manager_ += arrowDown_;
        }

        void SetHeight(const uint32_t height) {
            height_ = height;
            arrowDown_->shiftY_ = height - defaultArrowsHeight;
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

        virtual void SetSkinManager(SkinManager* skinManager) override {
            skinManager_ = skinManager;

            arrowUp_->SetSkinManager(skinManager);
            valueBut_->SetSkinManager(skinManager);
            arrowDown_->SetSkinManager(skinManager);
        }
};

const uint32_t defaultBarWidth = 20;

class List : public LimitedWindow {
    public:
        ScrollBar* scrollBar_;

    private:
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

        void KeyScroll(const sf::Keyboard::Key& key) {
            scrollBar_->onKeyboard_(key);
        }

    public: 
        List(const List& list) = default;
        List& operator=(const List& list) = default;

        List(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        LimitedWindow(x, y, width, height),
        scrollBar_(new ScrollBar(width - defaultBarWidth, 0, defaultBarWidth, height, &curPointer_, &minHeight_, &maxHeight_))
        {
            onTick_     += new MethodCaller<List, int64_t>(this, &List::TickScroll);
            onClick_    += new MethodCaller<List, Vector>(this, &List::ClickScroll);
            onMove_     += new MethodCaller<List, Vector>(this, &List::MoveScroll);
            onRelease_  += new MethodCaller<List, int64_t>(this, &List::ReleaseScroll);
            onKeyboard_ += new MethodCaller<List, sf::Keyboard::Key>(this, &List::KeyScroll);
        }

        virtual void operator+=(Widget* newWidget) override {
            LimitedWindow::operator+=(newWidget);

            scrollBar_->RecalcScale();
            
            double visibleCoef = double(height_) / double(maxHeight_);
            visibleCoef = (visibleCoef > 1) ? 1 : visibleCoef;

            scrollBar_->SetButHigh(uint32_t(visibleCoef * double(height_ - 2 * defaultArrowsHeight)));
            if ((int64_t(curPointer_) + scrollBar_->GetButHigh()) > (height_ - int64_t(defaultArrowsHeight))) {
                curPointer_ = double(height_ - int64_t(defaultArrowsHeight) - scrollBar_->GetButHigh());
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

        virtual void SetSkinManager(SkinManager* skinManager) override {
            skinManager_ = skinManager;

            scrollBar_->SetSkinManager(skinManager);
        }
};

class TextField : public Button {
    private:
        virtual void FlagReleased([[maybe_unused]] const int64_t& time) override {
            isHolded_ = 0;
        }

        virtual void FlagClicked(const Vector& coords) override {
            if (IsClicked(coords)) {
                isClicked_ ^= 1;
            }
            else if (isClicked_) {
                isClicked_ = 0;
            }
        }

        virtual void MoveHover([[maybe_unused]] const Vector& coords) override {

        }

        virtual void TextKeyPressed(const sf::Keyboard::Key& key) {
            if (isClicked_) {
                std::string tempString = *text_.GetRealString();

                if (key == sf::Keyboard::Backspace) {
                    if (tempString.size() > 0) {
                        tempString.pop_back();
                    }
                }
                else if ((key >= sf::Keyboard::A) && (key <= sf::Keyboard::Z)) {
                    tempString += char('a' + (key - sf::Keyboard::A));
                }
                else if ((key >= sf::Keyboard::Num0) && (key <= sf::Keyboard::Num9)) {
                    tempString += char('0' + (key - sf::Keyboard::Num0));
                }
                else if (key == sf::Keyboard::Space) {
                    tempString += ' ';
                }
                else if (key == sf::Keyboard::Hyphen) {
                    tempString += '-';
                }
                else if (key == sf::Keyboard::Period) {
                    tempString += '.';
                }

                SetText(tempString, 0xffffff00);
            }
        }

    public:
        TextField(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        Button(x, y, width, height)
        {
            SetText("", 0xffffff00);

            widgetSkin_  = SkinIdxs::InactiveText;
            clickedSkin_ = SkinIdxs::ActiveText;

            onKeyboard_ += new MethodCaller<TextField, sf::Keyboard::Key>(this, &TextField::TextKeyPressed);
        }
};

template <class T>
class CtrlTextField : public TextField {
    private:
        T* ctrlThing_;
        bool* isChanged_;

        virtual void FlagClicked(const Vector& coords) override {
            bool isClickedBefore = isClicked_;

            if (IsClicked(coords)) {
                isClicked_ ^= 1;
            }
            else if (isClicked_) {
                isClicked_ = 0;
            }

            if (isClickedBefore && !isClicked_ && ctrlThing_) {
                std::stringstream myStream(*text_.GetRealString());

                myStream >> *ctrlThing_;
                if (isChanged_) {
                    *isChanged_ = 1;
                }
            }
        }

    public:
        CtrlTextField(const CtrlTextField& field) = default;
        CtrlTextField& operator=(const CtrlTextField& field) = default;

        CtrlTextField(uint32_t x, uint32_t y, uint32_t width, uint32_t height, T* ctrlThing, bool* changedFlag):
        TextField(x, y, width, height),
        ctrlThing_(ctrlThing), isChanged_(changedFlag)
        {
            if (ctrlThing_) {
                std::stringstream mySteam;
                mySteam.precision(2);
                mySteam << *ctrlThing;

                *text_.GetRealString() = mySteam.str();
            }
        }
};

class ProgressBar : public CustomButton<double> {
    private:
        double minValue_;
        double maxValue_;

    public:
        ProgressBar(uint32_t x, uint32_t y, uint32_t width, uint32_t height, double* valueToCtrl, double minValue, double maxValue) :
        CustomButton(x, y, width, height, valueToCtrl),
        minValue_(minValue), maxValue_(maxValue)
        {
            widgetSkin_  = SkinIdxs::BarBackground;
            clickedSkin_ = SkinIdxs::BarProgress;
        }

        virtual void Draw([[maybe_unused]] const int64_t& time) override {
            Window::Draw(time);

            uint32_t percent = uint32_t((*context_ - minValue_) / (maxValue_ - minValue_) * 100);

            Vector newXY = ConvertXY(0, 0);
            Rectangle realWindowRect({width_ * int64_t(percent) / 100, height_, int64_t(newXY.x_), int64_t(newXY.y_), GetRotation()});

            realWindowRect.Draw(ptrToRealWdw_, skinManager_->GetTexture(clickedSkin_));  

            std::stringstream stream;
            stream << percent << "%";

            SetText(stream.str());
            if (text_.GetSize()) {
                Vector textXY = ConvertXY(XTextShift, YTextShift);
                text_.rotation_ = GetRotation();

                text_.Draw(ptrToRealWdw_, textXY, width_, height_);
            }
        }
};

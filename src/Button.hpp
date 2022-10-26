#pragma once

#include <sstream>

#include "Window.hpp"

const int64_t XTextShift = 2;
const int64_t YTextShift = 2;

class Button : public Window {
    protected:
        BaseHandler<CordsPair>* clickAction_;

        SkinIdxs hoveredSkin_;
        SkinIdxs clickedSkin_;

        Text text_;

        bool isHovered_;

    public:
        Button(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        Window(x, y, width, height),
        isHovered_(0),
        text_(),
        hoveredSkin_(SkinIdxs::ButtonHovering), clickedSkin_(SkinIdxs::ButtonPressed)
        {   
            widgetSkin_ = SkinIdxs::ButtonNotPressed;
        }

        Button& operator=(const Button& button) = default;
        Button(const Button& button)            = default;

        virtual void MoveHover(const CordsPair& cords) {
            if (IsClicked(cords)) {
                isHovered_ = 1;
            }
            else {
                isHovered_ = 0;
            }
        }  

        virtual void OnMove(const Event& curEvent) override {
            Window::OnMove(curEvent);

            MoveHover({uint32_t(curEvent.mouseButton.x), uint32_t(curEvent.mouseButton.y)});
        }

        void SetHandler(BaseHandler<CordsPair>* newHandler) {
            if (clickAction_) {
                delete clickAction_;
            }

            clickAction_ = newHandler;
        }

        virtual void OnClick(const Event& curEvent) override {
            Window::OnClick(curEvent);

            if (clickAction_) {
                clickAction_->Call({uint32_t(curEvent.mouseButton.x), uint32_t(curEvent.mouseButton.y)});
            }
        }

        void SetText(const sf::String& newString = "", const MyColor& newColor = 0) {
            text_ = {newString, newColor};
        } 

        virtual void Draw() override {
            CordsPair newXY = ConvertXY({0, 0});
            Rectangle realWindowRect({width_, height_, newXY.x, newXY.y, GetRotation()});

            sf::Texture* curTexture = nullptr;
            if (isClicked_) {
                curTexture = skinManager_->GetTexture(clickedSkin_);
            }
            else if (isHovered_) {
                curTexture = skinManager_->GetTexture(hoveredSkin_);
            }
            else {
                curTexture = skinManager_->GetTexture(widgetSkin_);
            }

            realWindowRect.Draw(ptrToRealWdw_, curTexture);  

            if (text_.GetSize()) {
                CordsPair textXY = ConvertXY({XTextShift, YTextShift});
                text_.rotation_ = GetRotation();

                text_.Draw(ptrToRealWdw_, textXY, width_, height_);
            }
        }
};

class DropList : public Button {
    public:
        DynamicWindow* list_;

        DropList(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        Button(x, y, width, height),
        list_(new DynamicWindow(x, y + uint32_t(height_)))
        {}

        DropList(const DropList& dropList)            = default;
        DropList& operator=(const DropList& dropList) = default;

        virtual void OnMove(const Event& curEvent) override {
            Button::OnMove(curEvent);

            ConditionalMove(curEvent);
        }

        virtual void OnClick(const Event& curEvent) override {
            Button::OnClick(curEvent);

            ConditionalClick(curEvent);
        }

        virtual void OnTick(const Event& curEvent) override {
            Button::OnTick(curEvent);

            ConditionalTick(curEvent);
        }

        virtual void OnRelease(const Event& curEvent) override {
            Button::OnRelease(curEvent);

            ConditionalRelease(curEvent);
        }

        virtual void OnKeyboard(const Event& curEvent) override {
            Button::OnKeyboard(curEvent);

            ConditionalKey(curEvent);
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

    protected:
        virtual void FlagReleased() override {
            isHolded_ = 0;
        }

        virtual void FlagClicked(const CordsPair& coords) override {
            if (IsClicked(coords)) {
                isClicked_ ^= 1;
            }
            else if ((!list_->IsClicked(coords)) && isClicked_) {
                isClicked_ = 0;
            }
        }

        void ConditionalListAction(void (DynamicWindow::* method)(const Event& curEvent), const Event& curEvent) {
            if (isClicked_) {
                (list_->*method)(curEvent);
            }
        }

        void ConditionalClick(const Event& curEvent) {
            ConditionalListAction(&DynamicWindow::OnClick, curEvent);
        }

        void ConditionalTick(const Event& curEvent) {
            ConditionalListAction(&DynamicWindow::OnTick, curEvent);
        }

        void ConditionalMove(const Event& curEvent) {
            ConditionalListAction(&DynamicWindow::OnMove, curEvent);
        }

        void ConditionalRelease(const Event& curEvent) {
            ConditionalListAction(&DynamicWindow::OnRelease, curEvent);
        }

        void ConditionalKey (const Event& curEvent) {
            ConditionalListAction(&DynamicWindow::OnKeyboard, curEvent);
        }  
};

template <class T>
class CustomButton : public Button {
    protected: 
        void (*actionWithContext_)(T* context, const Event& curEvent);
        T* context;

    public:
        CustomButton(uint32_t x, uint32_t y, uint32_t width, uint32_t height, T* context) :
        Button(x, y, width, height),
        context_(context)
        {
            hoveredSkin_ = SkinIdxs::ButtonNotPressed;
        }

        CustomButton(const CustomButton<T>& anotherButton)               = default;
        CustomButton<T>& operator=(const CustomButton<T>& anotherButton) = default;

        virtual void OnClick(const Event& curEvent) {
            Button::OnClick(curEvent);

            if (context && actionWithContext_) {
                (*actionWithContext_)(context, curEvent);
            }
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

    public:
        ScrollBar(uint32_t x, uint32_t y, uint32_t width, uint32_t height, double* valueToCtrl, double* minValue, double* maxValue) :
        Window(x, y, width, height),
        valueToCtrl_(valueToCtrl), 
        minValue_(minValue), maxValue_(maxValue),
        scale_(0),
        arrowUp_(new Button(0, 0, width, defaultArrowsHeight)),
        valueBut_(new Button(0, defaultArrowsHeight, width, defaultBarHeight)),
        arrowDown_(new Button(0, height - defaultArrowsHeight, width, defaultArrowsHeight))
        {
            widgetSkin_ = SkinIdxs::ScrollBackground;

            arrowUp_->SetSkin(SkinIdxs::ScrollUp);
            arrowUp_->SetHandler(new MethodCaller<ScrollBar, CordsPair>(this, &ScrollBar::ArrowUp));
            manager_ += arrowUp_;

            valueBut_->SetSkin(SkinIdxs::ScrollMiddle);
            manager_ += valueBut_;

            arrowDown_->SetHandler(new MethodCaller<ScrollBar, CordsPair>(this, &ScrollBar::ArrowDown));
            arrowDown_->SetSkin(SkinIdxs::ScrollDown);
            manager_ += arrowDown_;
        }

        virtual void OnMove(const Event& curEvent) override {
            Window::OnMove(curEvent);

            MoveAtBar({uint32_t(curEvent.mouseButton.x), uint32_t(curEvent.mouseButton.y)});
        }

        virtual void OnClick(const Event& curEvent) override {
            Window::OnClick(curEvent);

            ClickAtBar({uint32_t(curEvent.mouseButton.x), uint32_t(curEvent.mouseButton.y)});
        }

        virtual void OnTick(const Event& curEvent) override {
            UpdateBarPosition();

            Window::OnTick(curEvent);
        }

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

    private:
        void UpdateBarPosition() {
            if (CmpDbl((*maxValue_ - *minValue_), 0)) {
                valueBut_->shiftY_ = defaultArrowsHeight;

                return;
            }

            double newShift = ConvertSysToSys(*valueToCtrl_, *minValue_, *maxValue_, double(defaultArrowsHeight), double(height_ - defaultArrowsHeight));
            valueBut_->shiftY_ = uint32_t(newShift);
        }

        void ArrowUp(const CordsPair& cords) {
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

        void ArrowDown(const CordsPair& cords) {
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

        void MoveTo(const CordsPair& cords) {
            CordsPair newCords = ConvertRealXY(cords);

            double newValue = ConvertSysToSys(double(newCords.y), double(defaultArrowsHeight), double(height_ - defaultArrowsHeight), *minValue_, *maxValue_);
            double realLength = ConvertSysToSys(double(defaultArrowsHeight + valueBut_->height_), double(defaultArrowsHeight), double(height_ - defaultArrowsHeight), *minValue_, *maxValue_);
            
            if ((newValue <= *valueToCtrl_ ) && (newValue >= *minValue_)) {
                *valueToCtrl_ = newValue;
            }
            else if (((newValue - realLength) >= *valueToCtrl_) && (newValue <= *maxValue_)) {
                *valueToCtrl_ = newValue - realLength;
            }
        }

        void ClickAtBar(const CordsPair& cords) {
            if (IsClicked(cords) && !arrowDown_->IsClicked(cords) && !arrowUp_->IsClicked(cords) && !valueBut_->IsClicked(cords)) {
                MoveTo(cords);
            }
        }

        void MoveAtBar(const CordsPair& cords) {
            if (valueBut_->GetClickedState()) {
                MoveTo(cords);
            }
        }
};

const uint32_t defaultBarWidth = 20;

class List : public LimitedWindow {
    protected:
        ScrollBar* scrollBar_;
    public:
        List(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        LimitedWindow(x, y, width, height),
        scrollBar_(new ScrollBar(width - defaultBarWidth, 0, defaultBarWidth, height, &curPointer_, &minHeight_, &maxHeight_))
        {}

        List(const List& list) = default;
        List& operator=(const List& list) = default;

        virtual void OnMove(const Event& curEvent) override {
            LimitedWindow::OnMove(curEvent);

            MoveScroll(curEvent);
        }

        virtual void OnTick(const Event& curEvent) override {
            LimitedWindow::OnTick(curEvent);

            TickScroll(curEvent);
        }

        virtual void OnClick(const Event& curEvent) override {
            LimitedWindow::OnClick(curEvent);

            ClickScroll(curEvent);
        }

        virtual void OnRelease(const Event& curEvent) override {
            LimitedWindow::OnRelease(curEvent);

            ReleaseScroll(curEvent);
        }

        virtual void OnKeyboard(const Event& curEvent) override {
            LimitedWindow::OnKeyboard(curEvent);

            KeyScroll(curEvent);
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

    private:
        void TickScroll(const Event& curEvent) {
            scrollBar_->OnTick(curEvent);
        }

        void ClickScroll(const Event& curEvent) {
            scrollBar_->OnClick(curEvent);
        }

        void MoveScroll(const Event& curEvent) {
            scrollBar_->OnMove(curEvent);
        }

        void ReleaseScroll(const Event& curEvent) {
            scrollBar_->OnRelease(curEvent);
        }

        void KeyScroll(const Event& curEvent) {
            scrollBar_->OnKeyboard(curEvent);
        }
};

class TextField : public Button {
    public:
        TextField(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        Button(x, y, width, height)
        {
            SetText("", 0xffffff00);

            widgetSkin_  = SkinIdxs::InactiveText;
            clickedSkin_ = SkinIdxs::ActiveText;
        }

        virtual void OnKeyboard(const Event& curEvent) override {
            Button::OnKeyboard(curEvent);

            TextKeyPressed(curEvent.key.code);
        }

    private:
        virtual void FlagReleased() override {
            isHolded_ = 0;
        }

        virtual void FlagClicked(const CordsPair& coords) override {
            if (IsClicked(coords)) {
                isClicked_ ^= 1;
            }
            else if (isClicked_) {
                isClicked_ = 0;
            }
        }

        virtual void MoveHover([[maybe_unused]] const CordsPair& coords) override {}

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

        virtual void Draw() override {
            Window::Draw();

            uint32_t percent = uint32_t((*context - minValue_) / (maxValue_ - minValue_) * 100);

            CordsPair newXY = ConvertXY({0, 0});
            Rectangle realWindowRect({width_ * int64_t(percent) / 100, height_, newXY.x, newXY.y, GetRotation()});

            realWindowRect.Draw(ptrToRealWdw_, skinManager_->GetTexture(clickedSkin_));  

            std::stringstream stream;
            stream << percent << "%";

            SetText(stream.str());
            if (text_.GetSize()) {
                CordsPair textXY = ConvertXY({uint32_t(XTextShift), uint32_t(YTextShift)});
                text_.rotation_ = GetRotation();

                text_.Draw(ptrToRealWdw_, textXY, width_, height_);
            }
        }
};

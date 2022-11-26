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
        clickAction_(nullptr),
        hoveredSkin_(SkinIdxs::ButtonHovering), clickedSkin_(SkinIdxs::ButtonPressed),
        text_(),
        isHovered_(0)
        {   
            widgetSkin_ = SkinIdxs::ButtonNotPressed;
        }

        Button& operator=(const Button& button) = default;
        Button(const Button& button)            = default;

        virtual void MoveHover(const CordsPair& cords) {
            if (IsClicked(cords)) {
                isHovered_ = 1;

                SetChanged();
            }
            else if (isHovered_) {
                isHovered_ = 0;

                SetChanged();
            }
        }  

        virtual void OnMove(const Event& curEvent) override {
            Window::OnMove(curEvent);

            MoveHover({curEvent.Oleg_.motion.x, curEvent.Oleg_.motion.y});
        }

        void SetHandler(BaseHandler<CordsPair>* newHandler) {
            if (clickAction_) {
                delete clickAction_;
            }

            clickAction_ = newHandler;
        }

        virtual void FlagClicked(const CordsPair& cords) override {
            if (IsClicked(cords)) {
                isClicked_ = 1;

                SetChanged();
            }
        }

        virtual void OnClick(const Event& curEvent) override {
            Window::OnClick(curEvent);

            if (clickAction_) {
                clickAction_->Call({curEvent.Oleg_.mbedata.x, curEvent.Oleg_.mbedata.y});
            }
        }

        void SetText(const sf::String& newString = "", const MyColor& newColor = 0) {
            text_ = {newString, newColor};
        } 

        void SetHoveredSkin(const SkinIdxs newSkin) {
            hoveredSkin_ = newSkin;
        }

        virtual void ReDraw() override {
            Rectangle realWindowRect({GetWidth(), GetHeight(), 0, 0, 0});

            sf::Texture* curTexture = nullptr;
            if (isClicked_) {
                curTexture = SkinManager::GetInstance("default").GetTexture(clickedSkin_);
            }
            else if (isHovered_) {
                curTexture = SkinManager::GetInstance("default").GetTexture(hoveredSkin_);
            }
            else {
                curTexture = SkinManager::GetInstance("default").GetTexture(widgetSkin_);
            }

            realWindowRect.Draw(widgetContainer_, curTexture);  

            if (text_.GetSize()) {
                CordsPair textXY = {XTextShift, YTextShift};
                text_.rotation_ = GetRotation();

                text_.Draw(widgetContainer_, textXY, GetWidth(), GetHeight());
            }
        }
};

class DropList : public Button {
    public:
        DynamicWindow* list_;

        DropList(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        Button(x, y, width, height),
        list_(new DynamicWindow(x, y + uint32_t(GetHeight())))
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
            ProcessRedraw();
            manager_.TriggerTick(curEvent);
            PlaceTexture();


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

                SetChanged();
            }
            else if ((!list_->IsClicked(coords)) && isClicked_) {
                isClicked_ = 0;

                SetChanged();
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
        T* context_;

    public:
        CustomButton(uint32_t x, uint32_t y, uint32_t width, uint32_t height, T* context) :
        Button(x, y, width, height),
        actionWithContext_(nullptr),
        context_(context)
        {
            hoveredSkin_ = SkinIdxs::ButtonNotPressed;
        }

        CustomButton(const CustomButton<T>& anotherButton)               = default;
        CustomButton<T>& operator=(const CustomButton<T>& anotherButton) = default;

        virtual void OnClick(const Event& curEvent) override {
            Button::OnClick(curEvent);

            if (context_ && actionWithContext_) {
                (*actionWithContext_)(context_, curEvent);
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
        double savedValue;
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
        savedValue(valueToCtrl ? *valueToCtrl : 0), valueToCtrl_(valueToCtrl), 
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

            MoveAtBar({curEvent.Oleg_.motion.x, curEvent.Oleg_.motion.y});
        }

        virtual void OnClick(const Event& curEvent) override {
            Window::OnClick(curEvent);

            ClickAtBar({curEvent.Oleg_.mbedata.x, curEvent.Oleg_.mbedata.y});
        }

        virtual void OnTick(const Event& curEvent) override {
            UpdateBarPosition();

            Window::OnTick(curEvent);
        }

        void SetButHigh(const uint32_t value) {
            valueBut_->SetSizes(valueBut_->GetWidth(), value);
        }

        int64_t GetButHigh() {
            return valueBut_->GetHeight();
        }

        ScrollBar(const ScrollBar& bar) = default;
        ScrollBar& operator=(const ScrollBar& bar) = default;

        void RecalcScale() {
            scale_ = (*maxValue_ - *minValue_) / double(GetHeight() - 2 * defaultArrowsHeight - valueBut_->GetHeight());
        }

        void SetHeight(const uint32_t height) {
            SetSizes(GetWidth(), height);
            arrowDown_->SetShifts(arrowDown_->GetShiftX(), height - defaultArrowsHeight);
        }

        virtual void SetParent(Widget* parent) override {
            parent_ = parent;
            
            arrowUp_->SetParent(this);
            valueBut_->SetParent(this);
            arrowDown_->SetParent(this);
        }
    protected:
        bool UpdateBarPosition() {
            if (CmpDbl(*valueToCtrl_, savedValue))
                return 0;

            if (CmpDbl((*maxValue_ - *minValue_), 0)) {
                valueBut_->SetShifts(valueBut_->GetShiftX(), defaultArrowsHeight);

                return 1;
            }

            double newShift = ConvertSysToSys(*valueToCtrl_, *minValue_, *maxValue_, double(defaultArrowsHeight), double(GetHeight() - defaultArrowsHeight));
            
            valueBut_->SetShifts(valueBut_->GetShiftX(), uint32_t(newShift));
            savedValue = *valueToCtrl_;

            return 1;
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
                double realLength = ConvertSysToSys(double(defaultArrowsHeight + valueBut_->GetHeight()), double(defaultArrowsHeight), double(GetHeight() - defaultArrowsHeight), *minValue_, *maxValue_);

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

            double newValue = ConvertSysToSys(double(newCords.y), double(defaultArrowsHeight), double(GetHeight() - defaultArrowsHeight), *minValue_, *maxValue_);
            double realLength = ConvertSysToSys(double(defaultArrowsHeight + valueBut_->GetHeight()), double(defaultArrowsHeight), double(GetHeight() - defaultArrowsHeight), *minValue_, *maxValue_);
            
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

class List : public LimitedWindow, public Serializeable {
    protected:
        ScrollBar* scrollBar_;
    public:
        List(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        LimitedWindow(x, y, width, height),
        scrollBar_(new ScrollBar(width - defaultBarWidth, 0, defaultBarWidth, height, &curPointer_, &minHeight_, &maxHeight_))
        {}

        List(const List& list) = default;
        List& operator=(const List& list) = default;

        virtual void Serialize(FILE* outStream, uint32_t depth) const override {
            FPutNChars(outStream, ' ', depth);
            
            fprintf(outStream, "{LIST, %u, %u, %ld, %ld}\n", GetShiftX(), GetShiftY(), GetWidth(), GetHeight());
        }

        virtual void OnMove(const Event& curEvent) override {
            LimitedWindow::OnMove(curEvent);

            MoveScroll(curEvent);
        }

        virtual void OnTick(const Event& curEvent) override {
            ProcessRedraw();

            TickVisible(curEvent);
            TickScroll(curEvent);

            PlaceTexture();
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

        virtual void operator+=(Widget* newWidget) override {
            LimitedWindow::operator+=(newWidget);

            scrollBar_->RecalcScale();
            
            double visibleCoef = double(GetHeight()) / double(maxHeight_);
            visibleCoef = (visibleCoef > 1) ? 1 : visibleCoef;

            scrollBar_->SetButHigh(uint32_t(visibleCoef * double(GetHeight() - 2 * defaultArrowsHeight)));
            if ((int64_t(curPointer_) + scrollBar_->GetButHigh()) > (GetHeight() - int64_t(defaultArrowsHeight))) {
                curPointer_ = double(GetHeight() - int64_t(defaultArrowsHeight) - scrollBar_->GetButHigh());
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
            SetText("", 0xffffffff);

            widgetSkin_  = SkinIdxs::InactiveText;
            clickedSkin_ = SkinIdxs::ActiveText;
        }

        virtual void OnKeyboard(const Event& curEvent) override {
            Button::OnKeyboard(curEvent);

            TextKeyPressed(curEvent.Oleg_.kpedata.code);
        }

    private:
        virtual void FlagReleased() override {
            isHolded_ = 0;
        }

        virtual void FlagClicked(const CordsPair& coords) override {
            if (IsClicked(coords)) {
                isClicked_ ^= 1;

                SetChanged();
            }
            else if (isClicked_) {
                isClicked_ = 0;

                SetChanged();
            }
        }

        virtual void MoveHover([[maybe_unused]] const CordsPair& coords) override {}

        virtual void TextKeyPressed(const Key& key) {
            if (isClicked_) {
                std::string tempString = *text_.GetRealString();

                if (key == Key::Backspace) {
                    if (tempString.size() > 0) {
                        tempString.pop_back();
                    }
                }
                else if ((key >= Key::A) && (key <= Key::Z)) {
                    tempString += char('a' + (key - Key::A));
                }
                else if ((key >= Key::Num0) && (key <= Key::Num9)) {
                    tempString += char('0' + (key - Key::Num0));
                }
                else if (key == Key::Space) {
                    tempString += ' ';
                }
                else if (key == Key::Hyphen) {
                    tempString += '-';
                }
                else if (key == Key::Period) {
                    tempString += '.';
                }

                SetText(tempString, 0xffffffff);
                SetChanged();
            }
        }
};

template <class T>
class CtrlTextField : public TextField {
    private:
        T* ctrlThing_;
        bool* isChanged_;

        virtual void FlagClicked(const CordsPair& coords) override {
            bool isClickedBefore = isClicked_;

            if (IsClicked(coords)) {
                isClicked_ ^= 1;

                SetChanged();
            }
            else if (isClicked_) {
                isClicked_ = 0;

                SetChanged();
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
        double savedValue_;
        double minValue_;
        double maxValue_;

    public:
        ProgressBar(uint32_t x, uint32_t y, uint32_t width, uint32_t height, double* valueToCtrl, double minValue, double maxValue) :
        CustomButton(x, y, width, height, valueToCtrl),
        savedValue_(valueToCtrl ? *valueToCtrl : 0),
        minValue_(minValue), maxValue_(maxValue)
        {
            widgetSkin_  = SkinIdxs::BarBackground;
            clickedSkin_ = SkinIdxs::BarProgress;
        }

        void UpdateProgress() {
            if (CmpDbl(savedValue_, *context_)) {
                return;
            }

            SetChanged();
            savedValue_ = *context_;
        }

        virtual void OnTick(const Event& curEvent) override {
            UpdateProgress();

            Window::OnTick(curEvent);
        }

        virtual void ReDraw() override {
            Window::ReDraw();

            uint32_t percent = uint32_t((*context_ - minValue_) / (maxValue_ - minValue_) * 100);

            Rectangle realWindowRect({GetWidth() * int64_t(percent) / 100, GetHeight(), 0, 0, 0});

            realWindowRect.Draw(widgetContainer_, SkinManager::GetInstance("default").GetTexture(clickedSkin_));  

            std::stringstream stream;
            stream << percent << "%";

            SetText(stream.str());
            if (text_.GetSize()) {
                CordsPair textXY = {uint32_t(XTextShift), uint32_t(YTextShift)};
                text_.rotation_ = 0;

                text_.Draw(widgetContainer_, textXY, GetWidth(), GetHeight());
            }
        }
};

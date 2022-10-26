#pragma once

#include <list>
#include <chrono>

#include "Event.hpp"
#include "SkinManager.hpp"
#include "Color.hpp"
#include "Primitives.hpp"

class Window;
using Event = sf::Event;

struct CordsPair {
    uint32_t x;
    uint32_t y;
};

class Widget {
    protected:
        Widget* parent_;
        sf::RenderWindow* ptrToRealWdw_;
        SkinManager*      skinManager_;

        SkinIdxs widgetSkin_;
    public:
        int64_t width_;
        int64_t height_;

        uint32_t shiftX_;
        uint32_t shiftY_;

        float rotation_;
    protected:   
        bool isClicked_;
        bool isHolded_;
    public:
        Widget(uint32_t shiftX, uint32_t shiftY, int64_t width, int64_t height) :
        parent_(nullptr), ptrToRealWdw_(nullptr),
        skinManager_(nullptr),
        widgetSkin_(SkinIdxs::NoSkin),
        width_(width), height_(height),
        shiftX_(shiftX), shiftY_(shiftY),
        rotation_(0),
        isClicked_(0), isHolded_(0)
        {}

        virtual ~Widget() {}

        Widget& operator=([[maybe_unused]] const Widget& widgToCpy) = default;
        Widget(const Widget& widgToCpy) = default;

        virtual void OnMove(const Event& curEvent) {

        }

        virtual void FlagClicked(const CordsPair& cords) {
            if (IsClicked(cords)) {
                isClicked_ = 1;
            }
        }

        virtual bool IsClicked(const CordsPair& cords) = 0;

        bool GetClickedState() {
            return isClicked_;
        }

        void SetClickedState(const bool newState) {
            isClicked_ = newState;
        }

        virtual void FlagHolded(const CordsPair& cords) {
            if (isClicked_ && IsClicked(cords)) {
                isHolded_ = 1;
            }
        }

        virtual void OnClick(const Event& curEvent) {
            CordsPair curCords = {uint32_t(curEvent.mouseButton.x), uint32_t(curEvent.mouseButton.y)};

            FlagClicked(curCords);
            FlagHolded(curCords);
        }

        virtual void Draw() = 0;

        virtual void OnTick(const Event& curEvent) {
            Draw();
        }

        virtual void FlagReleased() {
            isClicked_ = 0;
            isHolded_  = 0;
        }

        virtual void OnRelease(const Event& curEvent) {
            FlagReleased();
        }

        virtual void OnKeyboard(const Event& curEvent) {

        }

        SkinIdxs GetSkin() {
            return widgetSkin_;
        }

        void SetSkin(const SkinIdxs& newSkin) {
            widgetSkin_ = newSkin;
        }

        virtual Widget* GetParent() {
            return parent_;
        }

        virtual void SetParent(Widget* newParent) {
            parent_ = newParent;
        }

        virtual sf::RenderWindow* GetRealWindowPtr() {
            return ptrToRealWdw_;
        }

        virtual void SetRealWindowPtr(sf::RenderWindow* newPtr) {
            ptrToRealWdw_ = newPtr;
        }

        virtual void SetSkinManager(SkinManager* skinManager) {
            skinManager_ = skinManager;
        }

        void Rotate(float rotation) {
            rotation_ += rotation;
        }

        float GetRotation() {
            float result = 0;
            Widget* curWidget = this;

            while(curWidget->parent_) {
                result += curWidget->rotation_;

                curWidget = curWidget->parent_;
            }

            return result;
        }

        CordsPair ConvertXY(const CordsPair& cords) {
            Widget* curWidg = this;

            int64_t x = cords.x;
            int64_t y = cords.y;

            while (curWidg->parent_) {
                int64_t oldX = x;
                int64_t oldY = y;

                x  = int64_t(float(oldX) * cosf(curWidg->rotation_) - float(oldY) * sinf(curWidg->rotation_));
                y  = int64_t(float(oldX) * sinf(curWidg->rotation_) + float(oldY) * cosf(curWidg->rotation_));

                x += curWidg->shiftX_;
                y += curWidg->shiftY_;

                curWidg = curWidg->parent_;
            }

            return {x, y};
        }

        CordsPair ConvertRealXY(const CordsPair& cords) {
            std::list<Widget*> parentsList = {};

            for (Widget* curWidg = this; curWidg->parent_; curWidg = curWidg->parent_) {
                parentsList.push_front(curWidg);
            }

            int64_t x = cords.x;
            int64_t y = cords.y;

            for (auto& curWidg : parentsList) {
                x -= curWidg->shiftX_;
                y -= curWidg->shiftY_;

                int64_t oldX = x;
                int64_t oldY = y;

                x  = int64_t(float(oldX) * cosf(-curWidg->rotation_) - float(oldY) * sinf(-curWidg->rotation_));
                y  = int64_t(float(oldX) * sinf(-curWidg->rotation_) + float(oldY) * cosf(-curWidg->rotation_));
            }

            return {x, y};
        }
};

class ChildrenManager {
    private:
        std::list<Widget*> widgets_;

    public:
        ChildrenManager() :
        widgets_()
        {}

        ~ChildrenManager() {
            for (auto& curWidget : widgets_) {
                delete curWidget;
            }
        }

        void Clear() {
            for (auto& curWidget : widgets_) {
                delete curWidget;
            }

            widgets_.clear();
        }

        std::list<Widget*>* GetWidgetsList() {
            return &widgets_;
        }

        void operator+=(Widget* newWidget) {
            widgets_.push_back(newWidget);
        }

        void operator-=(Widget* widgetToClose) {
            for (auto it = widgets_.begin(); it != widgets_.end(); it++) {
                if (*it == widgetToClose) {
                    widgets_.erase(it);
                }
            }  
        }

        void TriggerMove(const Event& curEvent) {
            for (auto& curWidget : widgets_) {
                curWidget->OnMove(curEvent);
            }
        }

        void TriggerClick(const Event& curEvent) {
            for (auto& curWidget : widgets_) {
                curWidget->OnClick(curEvent);
            }
        }

        void TriggerTick(const Event& curEvent) {
            for (auto& curWidget : widgets_) {
                curWidget->OnTick(curEvent);
            }
        }

        void TriggerRelease(const Event& curEvent) {
            for (auto& curWidget : widgets_) {
                curWidget->OnRelease(curEvent);
            }
        }

        void TriggetKeyPressed(const Event& curEvent) {
            for (auto& curWidget : widgets_) {
                curWidget->OnKeyboard(curEvent);
            }
        }
};

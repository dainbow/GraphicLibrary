#pragma once

#include "../Plugins/src/tools.hpp"

#include "SkinManager.hpp"
#include "Image.hpp"

const uint32_t DefaultPropertyWindowWidth  = 500;
const uint32_t DefaultPropertyWindowHeight = 500;

class Canvas;

class ToolButton;

class ToolManager {
    private:
        static ToolManager* instance_;
        booba::Tool* activeTool_;

        std::vector<booba::Tool*> tools_;
        std::vector<sf::Texture*> textures_; 

        uint64_t curInitTool_;
        std::vector<Window*>      toolWindows_;

        ToolManager();
    public:
        ToolManager(const ToolManager& manager)            = delete;
        ToolManager& operator=(const ToolManager& manager) = delete;

        static ToolManager& GetInstance() {
            if (!instance_) {
                instance_ = new ToolManager();
            }

            std::cout << "RETURNING INSTANCE\n";
            return *instance_;
        }

        void ApplyActive(booba::Image* image, const booba::Event* event) {
            if (activeTool_) {
                activeTool_->apply(image, event);
            }
        }

        void SelectTool(booba::Tool* newTool) {
            activeTool_ = newTool;
        }

        void AddTool(booba::Tool* newTool) {
            tools_.push_back(newTool);

            sf::Texture* newTexture = new sf::Texture();
            newTexture->loadFromFile(newTool->getTexture());

            textures_.push_back(newTexture);

            toolWindows_.push_back(new Window(0, 0, DefaultPropertyWindowWidth, DefaultPropertyWindowHeight));
            newTool->buildSetupWidget();
            curInitTool_++;
        }

        uint64_t GetToolIdx(booba::Tool* tool) {
            for (uint32_t toolIdx = 0; toolIdx < tools_.size(); toolIdx++) {
                if (tools_[toolIdx] == tool) {
                    return toolIdx;
                }
            }

            return UINT64_MAX;
        }

        uint64_t GetToolSize() {
            return tools_.size();
        }

        uint64_t GetCurInitTool() {
            return curInitTool_;
        }

        Window* GetToolWindow(const uint64_t toolIdx) {
            return toolWindows_[toolIdx];
        }

        booba::Tool* GetTool(const uint64_t toolIdx) {
            return tools_[toolIdx];
        }

        sf::Texture* GetToolTexture(const uint64_t toolIdx) {
            return textures_[toolIdx];
        }
};

class ToolWindow : public RealWindow {
    public:
        ToolWindow() :
        RealWindow(DefaultPropertyWindowWidth, DefaultPropertyWindowHeight)
        {}

        virtual void PollEvent() override {
            sf::Event curEvent;
            if (realWindow_.pollEvent(curEvent) == 0) {
                OnTick(Event(curEvent));

                return;
            }

            Event myEvent(curEvent);

            switch (myEvent.type_) {
                case EventType::Closed: {
                    Hide();

                    break;
                }

                case EventType::MouseMoved: {
                    if ((curEvent.mouseMove.y < 0) || (curEvent.mouseMove.y > GetHeight())) {
                        break;
                    }

                    OnMove(myEvent);

                    break;
                }

                case EventType::MousePressed: {
                    if ((GetTimeMiliseconds() - lastPressedTime_) < TimeBetweenClicks)
                        break;

                    OnClick(myEvent);

                    lastPressedTime_ = GetTimeMiliseconds();
                    break;
                }

                case EventType::MouseReleased: {
                    OnRelease(myEvent);

                    break;
                }

                case EventType::KeyPressed: {
                    OnKeyboard(myEvent);

                    break;
                }

                case EventType::NoEvent:
                case EventType::ButtonClicked:
                case EventType::ScrollbarMoved:
                case EventType::CanvasMPressed:
                case EventType::CanvasMReleased:
                case EventType::CanvasMMoved:
                default:
                    break;
            }

            OnTick(myEvent);
        }

        void Pop() {
            if (manager_.GetWidgetsList()->size())
                manager_.GetWidgetsList()->pop_back();
        }

        void Push(Window* newActiveToolWindow) {
            manager_.GetWidgetsList()->push_back(newActiveToolWindow);
        }
};

class ButtonForTool : public Button {
    private:
        booba::Tool* bindedTool_;

    public:
        ButtonForTool(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        Button(x, y, width, height),
        bindedTool_(ToolManager::GetInstance().GetTool(ToolManager::GetInstance().GetCurInitTool())) 
        {}

        ButtonForTool(const ButtonForTool& otherButt)                  = delete;
        const ButtonForTool& operator=(const ButtonForTool& otherButt) = delete;

        booba::Event ButtonClickEvent() {
            booba::Event newEvent;

            newEvent.type = booba::EventType::ButtonClicked;
            newEvent.Oleg.bcedata.id = reinterpret_cast<uint64_t>(this);

            return newEvent;
        }

        virtual void OnClick(const Event& curEvent) override {
            Window::OnClick(curEvent);

            if (IsClicked({curEvent.Oleg_.mbedata.x, curEvent.Oleg_.mbedata.y})) {
                booba::Event standartEvent = ButtonClickEvent();
                bindedTool_->apply(nullptr, &standartEvent);
            }
        }
};

class ScrollBarForTool : public ScrollBar {
    private:
        booba::Tool* bindedTool_;

        double minValue_;
        double maxValue_;

        double curValue_;
    public:
        ScrollBarForTool(uint32_t x, uint32_t y, uint32_t width, uint32_t height, double maxValue, double startValue) :
        ScrollBar(x, y, width, height, &curValue_, &minValue_, &maxValue_),
        bindedTool_(ToolManager::GetInstance().GetTool(ToolManager::GetInstance().GetCurInitTool())),
        minValue_(0), maxValue_(maxValue), curValue_(startValue)
        {}

        ScrollBarForTool(const ScrollBarForTool& otherScroll)            = delete;
        ScrollBarForTool& operator=(const ScrollBarForTool& otherScroll) = delete;

        booba::Event ScrollMoveEvent() {
            booba::Event newEvent;

            newEvent.type = booba::EventType::ScrollbarMoved;

            newEvent.Oleg.smedata.id = reinterpret_cast<uint64_t>(this);
            newEvent.Oleg.smedata.value = static_cast<int32_t>(curValue_);

            return newEvent;
        }

        virtual void OnTick(const Event& curEvent) override {
            if (UpdateBarPosition()) {
                booba::Event stEvent = ScrollMoveEvent();
                bindedTool_->apply(nullptr, &stEvent);
            }

            Window::OnTick(curEvent);
        }
};

class CanvasForTool : public ImageWindow {
    private:
        booba::Tool* bindedTool_;
    public:
        CanvasForTool(uint32_t x, uint32_t y, uint32_t width, uint32_t height) :
        ImageWindow(x, y, width, height),
        bindedTool_(ToolManager::GetInstance().GetTool(ToolManager::GetInstance().GetCurInitTool()))
        {}
        
        CanvasForTool(const CanvasForTool& otherCanvas)            = delete;
        CanvasForTool& operator=(const CanvasForTool& otherCanvas) = delete;

        booba::Event CanvasEvent(const Event& curEvent) {
            booba::Event newEvent;

            if (curEvent.type_ == EventType::MouseMoved) {
                newEvent.type = booba::EventType::CanvasMMoved;
            }
            else if (curEvent.type_ == EventType::MousePressed) {
                newEvent.type = booba::EventType::CanvasMPressed;
            }
            else if (curEvent.type_ == EventType::MouseReleased) {
                newEvent.type = booba::EventType::CanvasMReleased;
            }

            newEvent.Oleg.cedata.id = reinterpret_cast<uint64_t>(this);

            CordsPair convertedCords = ConvertRealXY({curEvent.Oleg_.mbedata.x, curEvent.Oleg_.mbedata.y});
            newEvent.Oleg.cedata.x = convertedCords.x;
            newEvent.Oleg.cedata.y = convertedCords.y;

            return newEvent;
        }

        virtual void OnMove(const Event& curEvent) override {
            Window::OnMove(curEvent);

            if (IsClicked({curEvent.Oleg_.mbedata.x, curEvent.Oleg_.mbedata.y})) {
                booba::Event stEvent = CanvasEvent(curEvent);
                bindedTool_->apply(nullptr, &stEvent);
            }
        }

        virtual void OnClick(const Event& curEvent) override {
            Window::OnClick(curEvent);

            if (IsClicked({curEvent.Oleg_.mbedata.x, curEvent.Oleg_.mbedata.y})) {
                booba::Event stEvent = CanvasEvent(curEvent);
                bindedTool_->apply(nullptr, &stEvent);
            }
        }

        virtual void OnRelease(const Event& curEvent) override {
            Window::OnRelease(curEvent);

            if (IsClicked({curEvent.Oleg_.mbedata.x, curEvent.Oleg_.mbedata.y})) {
                booba::Event stEvent = CanvasEvent(curEvent);
                bindedTool_->apply(nullptr, &stEvent);
            }
        }
    };

    class ToolPalette : public DynamicWindow, public Serializeable {
        private:
            ToolButton* curActive_;

        public:
            ToolPalette(uint32_t x, uint32_t y) :
            DynamicWindow(x, y),
            curActive_(nullptr)
            {}

            ToolPalette(const ToolPalette& palette)            = default;
            ToolPalette& operator=(const ToolPalette& palette) = default;

            virtual void Serialize(FILE* outStream, uint32_t depth) const override {
                FPutNChars(outStream, ' ', depth);
                
                fprintf(outStream, "{TLPT, %u, %u}\n", GetShiftX(), GetShiftY());
            }

            void AddTools(Canvas* canvas, ToolManager* manager, const uint64_t amount);
            void SetTool(ToolButton* newTool);
    };

    class Canvas : public FlexImageWindow {
        private:
            uint32_t curToolIdx_;
        public:
            ToolManager& toolManager_;
            ToolPalette* toolPalette_;
            ToolWindow*  toolWindow_;

            Canvas(uint32_t x, uint32_t y, uint32_t width, uint32_t height, ToolPalette* palette) :
            FlexImageWindow(x, y, width, height),
            curToolIdx_(0),
            toolManager_(ToolManager::GetInstance()),
            toolPalette_(palette),
            toolWindow_(new ToolWindow())
            {
                App::GetInstance() += toolWindow_;

                std::string dlPath = "./Plugins";

                for (const auto& curFile : std::filesystem::directory_iterator(dlPath)) {
                    if (curFile.is_directory()) {
                        continue;
                    }

                    void* dlHandler = dlopen(curFile.path().c_str(), RTLD_LAZY);
                    
                    if (dlHandler) {
                        void (*initFunc)()   = nullptr; 
                        *((void**)&initFunc) = dlsym(dlHandler, "init_module");

                        (*initFunc)();
                    }
                    else {
                        fprintf(stderr, "Unable to open lib: %s\n", dlerror());
                    }
                }
                std::cout << "TOOLS AMOUNT IS " << toolManager_.GetToolSize() << std::endl;
                toolPalette_->AddTools(this, &toolManager_, toolManager_.GetToolSize());
            }

            Canvas(const Canvas& canvas) = default;
            Canvas& operator=(const Canvas& canvas) = default;

            booba::Event ConvertToStandartEvent(const Event& event) {
                Event standartEvent = event;

                if (event.type_ == EventType::MousePressed) {
                    CordsPair convertedCords = ConvertRealXY({event.Oleg_.mbedata.x, event.Oleg_.mbedata.y});
                    std::cout << convertedCords.x << " " << convertedCords.y << std::endl;

                    standartEvent.Oleg_.mbedata.x = convertedCords.x;
                    standartEvent.Oleg_.mbedata.y = convertedCords.y;

                    standartEvent.Oleg_.mbedata.x += int32_t(GetImageXShift());
                    standartEvent.Oleg_.mbedata.y += int32_t(GetImageYShift());
                }
                else if (event.type_ == EventType::MouseMoved) {
                    CordsPair convertedCords = ConvertRealXY({event.Oleg_.motion.x, event.Oleg_.motion.y});

                    standartEvent.Oleg_.motion.x = convertedCords.x;
                    standartEvent.Oleg_.motion.y = convertedCords.y;

                    standartEvent.Oleg_.motion.x += int32_t(GetImageXShift());
                    standartEvent.Oleg_.motion.y += int32_t(GetImageYShift());
                }

                return *reinterpret_cast<booba::Event*>(&standartEvent);
            }

            virtual void OnClick(const Event& curEvent) override {
                Window::OnClick(curEvent);

                if (IsClicked({curEvent.Oleg_.mbedata.x, curEvent.Oleg_.mbedata.y})) {
                    booba::Event stEvent = ConvertToStandartEvent(curEvent);

                    toolManager_.ApplyActive(&image_, &stEvent);
                    SetChanged();
                }
            }

            virtual void OnMove(const Event& curEvent) override {
                Window::OnMove(curEvent);

                if (IsClicked({curEvent.Oleg_.mbedata.x, curEvent.Oleg_.mbedata.y})) {
                    booba::Event stEvent = ConvertToStandartEvent(curEvent);
                    
                    toolManager_.ApplyActive(&image_, &stEvent);
                    SetChanged();
                }
            }

            virtual void OnRelease(const Event& curEvent) override {
                Window::OnRelease(curEvent);

                booba::Event stEvent = ConvertToStandartEvent(curEvent);
                toolManager_.ApplyActive(&image_, &stEvent);
                SetChanged();
            }

            ~Canvas() {}
        
    };

    const uint32_t DefaultToolButtonWidth  = 50;
    const uint32_t DefaultToolButtonHeight = 50;

    class ToolButton : public Button {
        private:
            Canvas*       canvas_;

            booba::Tool*  tool_;
            sf::Texture*  toolTexture_;

        public:
            ToolButton(Canvas* curCanvas, booba::Tool* curTool) :
            Button(0, 0, DefaultToolButtonWidth, DefaultToolButtonHeight),
            canvas_(curCanvas), tool_(curTool), toolTexture_(nullptr)
            {
                clickAction_ = new MethodCaller(this, &ToolButton::SetMeToCanvas);
                toolTexture_ = canvas_->toolManager_.GetToolTexture(ToolManager::GetInstance().GetToolIdx(curTool));
            }

            ToolButton(const ToolButton& toolB)            = default;
            ToolButton& operator=(const ToolButton& toolB) = default;

            virtual void FlagClicked([[maybe_unused]] const CordsPair& cords) override {}
            virtual void FlagReleased() override {}

            void SetMeToCanvas(const CordsPair& cords) {
                if (!IsClicked(cords)) {
                    return;
                }

                canvas_->toolManager_.SelectTool(tool_);
                
                canvas_->toolWindow_->Pop();

                Window* newActiveWindow = ToolManager::GetInstance().GetToolWindow(ToolManager::GetInstance().GetToolIdx(tool_));
                newActiveWindow->SetParent(canvas_->toolWindow_);

                canvas_->toolWindow_->Push(newActiveWindow);
                canvas_->toolWindow_->Show();

                ((ToolPalette*)parent_)->SetTool(this);
            }

            virtual void ReDraw() override {
                Button::ReDraw();

                sf::Texture texture;

                Rectangle iconRect({DefaultToolButtonWidth, DefaultToolButtonHeight, 0, 0});
                iconRect.Draw(widgetContainer_, toolTexture_);
            }
    }; 

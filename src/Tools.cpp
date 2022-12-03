#include "Tools.hpp"

ToolManager* ToolManager::instance_ = nullptr;

void booba::addTool(booba::Tool* tool) {
    ToolManager::GetInstance().AddTool(tool);
}

void booba::addFilter(booba::Tool* tool) {
    addTool(tool);
}

ToolManager::ToolManager() :
secondLayer_(nullptr),
activeTool_(nullptr),
plugins_(),
tools_(),
textures_(),
curInitTool_(0), toolWindows_()
{
    
}

void ToolPalette::AddTools(Canvas* canvas, ToolManager* manager, const uint64_t amount) {
    for (uint64_t curTool = (manager->GetToolSize() - amount); curTool < manager->GetToolSize(); curTool++) {
        *this += new ToolButton(canvas, manager->GetTool(curTool));
    }
}

void ToolPalette::SetTool(ToolButton* newTool) {
    if (newTool == curActive_)
        return;

    if (curActive_) {
        curActive_->SetClickedState(0);
        curActive_->SetChanged();
    }

    newTool->SetClickedState(1);
    newTool->SetChanged();

    curActive_ = newTool;   
}

uint64_t booba::createButton(size_t x, size_t y, size_t w, size_t h, const char* text) {
    ButtonForTool* newButton = new ButtonForTool(x, y, w, h);
    if (text) {
        newButton->SetText(text, 0xffffffff);
    }

    *ToolManager::GetInstance().GetToolWindow(ToolManager::GetInstance().GetCurInitTool()) += newButton;

    return reinterpret_cast<uint64_t>(newButton);
}

uint64_t booba::createSlider(size_t x, size_t y, size_t w, size_t h, int64_t minValue, int64_t maxValue, int64_t startValue) {
    ScrollBarForTool* newScroll = new ScrollBarForTool(x, y, h, w, double(minValue), double(maxValue), double(startValue));
    newScroll->SetRotation(-float(M_PI_2));

    *ToolManager::GetInstance().GetToolWindow(ToolManager::GetInstance().GetCurInitTool()) += newScroll;

    return reinterpret_cast<uint64_t>(newScroll);
}

uint64_t booba::createLabel(size_t x, size_t y, size_t w, size_t h, const char* text) {
    CustomButton<int32_t>* newLabel = new CustomButton<int32_t>(uint32_t(x), uint32_t(y), uint32_t(w), uint32_t(h), nullptr);
    if (text) {
        newLabel->SetText(text, 0xffffffff);
    }

    *ToolManager::GetInstance().GetToolWindow(ToolManager::GetInstance().GetCurInitTool()) += newLabel;

    return reinterpret_cast<uint64_t>(newLabel);
}

void booba::putPixel(uint64_t canvas, size_t x, size_t y, uint32_t color) {
    CanvasForTool* curCanvas = reinterpret_cast<CanvasForTool*>(canvas);

    curCanvas->image_.setPixel(x, y, color);
    curCanvas->SetChanged();
}

uint64_t booba::createCanvas(size_t x, size_t y, size_t w, size_t h) {
    CanvasForTool* newCanvas = new CanvasForTool(x, y, w, h);

    *ToolManager::GetInstance().GetToolWindow(ToolManager::GetInstance().GetCurInitTool()) += newCanvas;

    return reinterpret_cast<uint64_t>(newCanvas);
}

void booba::putSprite(uint64_t canvas, size_t x, size_t y, size_t w, size_t h, const char* texture) {
    assert(texture);
    
    CanvasForTool* curCanvas = reinterpret_cast<CanvasForTool*>(canvas);

    ::Image imageToCopy{uint32_t(w), uint32_t(h)};
    imageToCopy.LoadFromFile(texture);

    curCanvas->image_.Copy(imageToCopy, uint32_t(x), uint32_t(y));
}

void booba::cleanCanvas(uint64_t canvasId, uint32_t color) {
    CanvasForTool* curCanvas = reinterpret_cast<CanvasForTool*>(canvasId);

    curCanvas->image_.Clear(color);
}

void* booba::getLibSymbol(GUID guid, const char* name) {
    void* handler = nullptr;

    if ((handler = ToolManager::GetInstance().GetGUIDHandler(guid)) == nullptr) {
        return nullptr;
    }

    return dlsym(handler, name);
}

booba::Image* booba::getHiddenLayerID() {
    return ToolManager::GetInstance().GetSecondLayer();
}


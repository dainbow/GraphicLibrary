#include "Tools.hpp"

ToolManager* ToolManager::instance_ = nullptr;

void booba::addTool(booba::Tool* tool) {
    ToolManager::GetInstance().AddTool(tool);
}

ToolManager::ToolManager() :
activeTool_(nullptr),
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

uint64_t booba::createButton(int32_t x, int32_t y, uint32_t w, uint32_t h, const char* text) {
    ButtonForTool* newButton = new ButtonForTool(x, y, w, h);
    if (text) {
        newButton->SetText(text);
    }

    *ToolManager::GetInstance().GetToolWindow(ToolManager::GetInstance().GetCurInitTool()) += newButton;

    return reinterpret_cast<uint64_t>(newButton);
}

uint64_t booba::createScrollbar(int32_t x, int32_t y, uint32_t w, uint32_t h) {
    ScrollBarForTool* newScroll = new ScrollBarForTool(x, y, h, w);
    newScroll->SetRotation(-float(M_PI_2));

    *ToolManager::GetInstance().GetToolWindow(ToolManager::GetInstance().GetCurInitTool()) += newScroll;

    return reinterpret_cast<uint64_t>(newScroll);
}

uint64_t booba::createLabel(int32_t x, int32_t y, uint32_t w, uint32_t h, const char* text) {
    CustomButton<int32_t>* newLabel = new CustomButton<int32_t>(x, y, w, h, nullptr);
    if (text) {
        newLabel->SetText(text, 0xffffff00);
    }

    *ToolManager::GetInstance().GetToolWindow(ToolManager::GetInstance().GetCurInitTool()) += newLabel;

    return reinterpret_cast<uint64_t>(newLabel);
}

uint64_t booba::putPixel(uint64_t canvas, int32_t x, int32_t y, uint32_t color) {
    CanvasForTool* curCanvas = reinterpret_cast<CanvasForTool*>(canvas);

    curCanvas->image_.SetPixel(x, y, color);

    return canvas;
}

uint64_t booba::putSprite(uint64_t canvas, int32_t x, int32_t y, uint32_t w, uint32_t h, const char* texture) {
    assert(texture);
    
    CanvasForTool* curCanvas = reinterpret_cast<CanvasForTool*>(canvas);

    ::Image imageToCopy(w, h);
    imageToCopy.LoadFromFile(texture);

    curCanvas->image_.Copy(imageToCopy, x, y);

    return canvas;
}

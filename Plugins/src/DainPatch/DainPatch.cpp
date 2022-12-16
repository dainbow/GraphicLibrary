#include "DainPatch.hpp" 

booba::GUID booba::getGUID() {
    booba::GUID dainToolsGUID = {"DAIN-PATCH"};
    
    return dainToolsGUID;
}

void PatchTool::DrawCircle(booba::Image* layer, const CordsPair& center, const int32_t radius, const uint32_t color1, const uint32_t color2) {
    for (int32_t curX = std::max(0, center.x - radius); curX < std::min(int32_t(layer->getW()), center.x + radius); curX++) {
        for (int32_t curY = std::max(0, center.y - radius); curY < std::min(int32_t(layer->getH()), center.y + radius); curY++) {
            if (((std::pow(curX - center.x, 2) + std::pow(curY - center.y, 2)) <= (std::pow(radius, 2))) &&
                ((std::pow(curX - center.x, 2) + std::pow(curY - center.y, 2)) >= (std::pow(radius - 3, 2)))) {
                if (std::sin(curX) * std::sin(curY) > 0) {
                    layer->setPixel(curX, curY, color1);
                }
                else {
                    layer->setPixel(curX, curY, color2);
                }
            }
        }
    }
}

void PatchTool::apply(booba::Image* image, const booba::Event* event) {
    if (image == nullptr) {
        if (event->type == booba::EventType::SliderMoved) {
            if (event->Oleg.smedata.id == sizeScrollIdx_) {
                brushSize_ = std::max(1l, event->Oleg.smedata.value / 2);
            }
            else if (event->Oleg.smedata.id == transScrollIdx_) {
                transparency_ = double(event->Oleg.smedata.value) / 50.0;
            }
        }

        return;
    }

    if (event->type == booba::EventType::MousePressed) {
        isClicked_ = 1;

        if (event->Oleg.mbedata.alt) {  
            anchorPoint_ = {int32_t(event->Oleg.mbedata.x), int32_t(event->Oleg.mbedata.y)};
        }

        return;
    }

    if (event->type == booba::EventType::MouseReleased) {
        isClicked_ = 0;

        colored_.clear();
        virtualPoint_ = {-1, -1};

        return;
    }

    CordsPair moveCords = {int32_t(event->Oleg.motion.x), int32_t(event->Oleg.motion.y)};
    
    booba::Image* secondLayer = booba::getHiddenLayerID();

    static CordsPair lastMove = {-1, -1};
    static int32_t lastBrushSize = 0;

    if ((lastMove.x != -1) && (lastMove.y != -1)) {
        DrawCircle(secondLayer, lastMove, lastBrushSize, 0, 0);
    }
    DrawCircle(secondLayer, moveCords, brushSize_, 0x000000ff, 0xffffffff);

    lastMove = moveCords;
    lastBrushSize = brushSize_;

    if (((anchorPoint_.x == -1) && (anchorPoint_.y == -1))) {
        return;
    }

    int32_t deltaX = 0;
    int32_t deltaY = 0;

    if ((virtualPoint_.x != -1) && (virtualPoint_.y != -1)) {
        deltaX = moveCords.x - virtualPoint_.x;
        deltaY = moveCords.y - virtualPoint_.y;
    }

    CordsPair whereNow = {anchorPoint_.x + deltaX, anchorPoint_.y + deltaY};
    static CordsPair lastWhere = {-1, -1};

    if ((lastWhere.x != -1) && (lastWhere.y != -1)) {
        DrawCircle(secondLayer, lastWhere, lastBrushSize, 0, 0);
    }
    DrawCircle(secondLayer, whereNow, brushSize_, 0x000000ff, 0xffffffff);

    lastWhere = whereNow;

    if (isClicked_ == 0) {
        return;
    }

    if ((virtualPoint_.x == -1) && (virtualPoint_.y == -1)) {
        virtualPoint_ = moveCords;
    }

    for (int32_t curX = std::max(0, moveCords.x - brushSize_); curX < std::min(int32_t(image->getW()), moveCords.x + brushSize_); curX++) {
        for (int32_t curY = std::max(0, moveCords.y - brushSize_); curY < std::min(int32_t(image->getH()), moveCords.y + brushSize_); curY++) {
            if (colored_.find(uint64_t(moveCords)) == colored_.end()) {
                if ((std::pow(curX - moveCords.x, 2) + std::pow(curY - moveCords.y, 2)) <= (std::pow(brushSize_, 2))) {
                    deltaX = curX - virtualPoint_.x;
                    deltaY = curY - virtualPoint_.y;
                    
                    if (((anchorPoint_.x + deltaX) >= 0) && ((anchorPoint_.x + deltaX) < int32_t(image->getW())) &&
                        ((anchorPoint_.y + deltaY) >= 0) && ((anchorPoint_.y + deltaY) < int32_t(image->getH()))) {
                        uint32_t anchorColor = image->getPixel(anchorPoint_.x + deltaX, anchorPoint_.y + deltaY);
                        anchorColor = (anchorColor & 0xffffff00) | uint8_t(transparency_ * 255.0);

                        // printf("Color trans is %u", uint32_t(anchorColor & 0x000000ff));
                        image->setPixel(curX, curY, anchorColor);
                    }
                }
            }
        }
    }
}

void PatchTool::buildSetupWidget() {
    booba::createLabel(430, 10, 50, 30, "Size");
    booba::createLabel(430, 60, 60, 30, "Trans");

    sizeScrollIdx_  = booba::createSlider(20, 40, 400, 30, 0, 400, 0);
    transScrollIdx_ = booba::createSlider(20, 90, 400, 30, 0, 50, 45);
}

void booba::init_module() {
    PatchTool* patch = new PatchTool();

    booba::addTool(patch);
}

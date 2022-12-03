#include "DainPatch.hpp" 

booba::GUID booba::getGUID() {
    booba::GUID dainToolsGUID = {"DAIN-PATCH"};
    
    return dainToolsGUID;
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

    for (int32_t curX = std::max(0, moveCords.x - brushSize_); curX < std::min(int32_t(secondLayer->getW()), moveCords.x + brushSize_); curX++) {
        for (int32_t curY = std::max(0, moveCords.y - brushSize_); curY < std::min(int32_t(secondLayer->getH()), moveCords.y + brushSize_); curY++) {
            if (((std::pow(curX - moveCords.x, 2) + std::pow(curY - moveCords.y, 2)) <= (std::pow(brushSize_, 2))) &&
                ((std::pow(curX - moveCords.x, 2) + std::pow(curY - moveCords.y, 2)) >= (std::pow(brushSize_ - 3, 2)))) {
                if (std::sin(curX) * std::sin(curY) > 0) {
                    secondLayer->setPixel(curX, curY, 0x000000ff);
                }
                else {
                    secondLayer->setPixel(curX, curY, 0xffffffff);
                }
            }
        }
    }

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

    for (int32_t curX = std::max(0, whereNow.x - brushSize_); curX < std::min(int32_t(secondLayer->getW()), whereNow.x + brushSize_); curX++) {
        for (int32_t curY = std::max(0, whereNow.y - brushSize_); curY < std::min(int32_t(secondLayer->getH()), whereNow.y + brushSize_); curY++) {
            if (((std::pow(curX - whereNow.x, 2) + std::pow(curY - whereNow.y, 2)) <= (std::pow(brushSize_, 2))) &&
                ((std::pow(curX - whereNow.x, 2) + std::pow(curY - whereNow.y, 2)) >= (std::pow(brushSize_ - 3, 2)))) {
                if (std::sin(curX) * std::sin(curY) > 0) {
                    secondLayer->setPixel(curX, curY, 0x000000ff);
                }
                else {
                    secondLayer->setPixel(curX, curY, 0xffffffff);
                }
            }
        }
    }

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

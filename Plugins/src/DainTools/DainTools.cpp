#include "DainTools.hpp" 

booba::GUID booba::getGUID() {
    booba::GUID dainToolsGUID = {"DAIN-TOOLS"};
    
    return dainToolsGUID;
}

int64_t GetTimeMiliseconds() {
    auto curTime = std::chrono::system_clock::now();
    auto sinceEpoch = curTime.time_since_epoch();

    auto miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(sinceEpoch);
    return miliseconds.count(); 
}

void LineTool::apply(booba::Image* image, const booba::Event* event) {
    if (image == nullptr) {
        if (event->type == booba::EventType::SliderMoved) {
            if (event->Oleg.smedata.id == sizeScrollIdx_) {
                lineSize_ = std::max(1l, event->Oleg.smedata.value / 2);
            }
        }

        return;
    }

    if (event->type != booba::EventType::MousePressed)
        return;

    CordsPair curClick = {int32_t(event->Oleg.mbedata.x), int32_t(event->Oleg.mbedata.y)};

    if ((firstClick_.x == -1) && (firstClick_.y == -1)) {
        firstClick_ = curClick;
    }
    else {
        CordsPair secondClick = curClick;

        DrawBigLine(firstClick_, secondClick, lineSize_, image, booba::APPCONTEXT->fgColor);

        firstClick_ = {-1, -1};
    }
}

void LineTool::buildSetupWidget() {
    booba::createLabel(20, 20, 300, 40, "Line size");

    sizeScrollIdx_ = booba::createSlider(20, 120, 400, 30, 0, 400, 0);
}

void BrushTool::DrawCircle(booba::Image* layer, const CordsPair& center, const int32_t radius, const uint32_t color1, const uint32_t color2) {
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

void BrushTool::apply(booba::Image* image, const booba::Event* event) {
    if (image == nullptr) {
        if (event->type == booba::EventType::SliderMoved) {
            if (event->Oleg.smedata.id == sizeScrollIdx_) {
                brushSize_ = std::max(1l, event->Oleg.smedata.value / 2);
            }
            else if (event->Oleg.smedata.id == transScrollIdx_) {
                transparency_ = double(event->Oleg.smedata.value) / 50.0;
                // printf("New trans is %lg\n", transparency_);
            }
        }

        return;
    }

    if (event->type == booba::EventType::MousePressed) {
        isClicked_ = 1;
        lastPoint_ = {-1, -1};

        return;
    }

    if (event->type == booba::EventType::MouseReleased) {
        isClicked_ = 0;
        lastPoint_ = {-1, -1};

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

    if (isClicked_ == 0) {
        return;
    }

    if (GetTimeMiliseconds() < (lastTime_ + delay_)) {
        return;
    }

    if ((lastPoint_.x != -1) && (lastPoint_.y != -1)) {
        // printf("Brush drawing with transparency %lx\n", booba::APPCONTEXT->fgColor & uint8_t(transparency_ * 0xFF));
        LineTool::DrawBigLine(lastPoint_, moveCords, brushSize_, image, (booba::APPCONTEXT->fgColor & 0xffffff00) | (booba::APPCONTEXT->fgColor & uint8_t(transparency_ * 0xFF)));
    }

    lastPoint_ = moveCords;
    lastTime_  = GetTimeMiliseconds();
}

void BrushTool::buildSetupWidget() {
    booba::createLabel(430, 10, 50, 30, "Size");
    booba::createLabel(430, 60, 60, 30, "Trans");

    sizeScrollIdx_  = booba::createSlider(20, 40, 400, 30, 0, 400, 0);
    transScrollIdx_ = booba::createSlider(20, 90, 400, 30, 0, 50, 45);
}

void EraserTool::apply(booba::Image* image, const booba::Event* event)  {
    if (event->type == booba::EventType::MousePressed) {
        isClicked_ = 1;
        lastPoint_ = {-1, -1};

        return;
    }

    if (event->type == booba::EventType::MouseReleased) {
        isClicked_ = 0;
        lastPoint_ = {-1, -1};

        return;
    }

    if (isClicked_ == 0) {
        return;
    }

    if (GetTimeMiliseconds() < (lastTime_ + delay_)) {
        return;
    }

    CordsPair moveCords = {int32_t(event->Oleg.motion.x), int32_t(event->Oleg.motion.y)};


    if ((lastPoint_.x != -1) && (lastPoint_.y != -1)) {
        LineTool::DrawLine(image, lastPoint_, moveCords, {0xffffffff});
    }

    lastPoint_ = moveCords;
    lastTime_  = GetTimeMiliseconds();
}

void RectTool::apply(booba::Image* image, const booba::Event* event)  {
    if (event->type != booba::EventType::MousePressed)
        return;

    CordsPair curClick = {int32_t(event->Oleg.mbedata.x), int32_t(event->Oleg.mbedata.y)};

    if ((firstClick_.x == -1) && (firstClick_.y == -1)) {
        firstClick_ = curClick;
    }
    else {
        CordsPair secondClick = curClick;

        LineTool::DrawLine(image, firstClick_, {firstClick_.x, secondClick.y}, booba::APPCONTEXT->fgColor);
        LineTool::DrawLine(image, {firstClick_.x, secondClick.y}, secondClick, booba::APPCONTEXT->fgColor);
        LineTool::DrawLine(image, secondClick, {secondClick.x, firstClick_.y}, booba::APPCONTEXT->fgColor);
        LineTool::DrawLine(image, {secondClick.x, firstClick_.y}, firstClick_, booba::APPCONTEXT->fgColor);

        firstClick_ = {-1, -1};
    }
}

void EllipsTool::apply(booba::Image* image, const booba::Event* event)  {
    if (event->type != booba::EventType::MousePressed)
        return;

    CordsPair curClick = {int32_t(event->Oleg.mbedata.x), int32_t(event->Oleg.mbedata.y)};

    if ((firstClick_.x == -1) && (firstClick_.y == -1)) {
        firstClick_ = curClick;
    }
    else {
        CordsPair secondClick = curClick;

        DrawEllips(image, firstClick_, secondClick, booba::APPCONTEXT->fgColor);

        firstClick_ = {-1, -1};
    }
}

void BucketTool::apply(booba::Image* image, const booba::Event* event)  {
    if (event->type != booba::EventType::MousePressed)
        return;

    CordsPair curClick = {int32_t(event->Oleg.mbedata.x), int32_t(event->Oleg.mbedata.y)};

    Fill(image, curClick, booba::APPCONTEXT->fgColor);
}

void booba::init_module() {
    LineTool* lineTool     = new LineTool();
    BrushTool* brushTool   = new BrushTool();
    EraserTool* eraserTool = new EraserTool();
    RectTool* rectTool     = new RectTool();
    EllipsTool* ellipsTool = new EllipsTool();
    BucketTool* bucketTool = new BucketTool();

    booba::addTool(lineTool);
    booba::addTool(brushTool);
    booba::addTool(eraserTool);
    booba::addTool(rectTool);
    booba::addTool(ellipsTool);
    booba::addTool(bucketTool);
}

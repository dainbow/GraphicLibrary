#include "DainTools.hpp" 

int64_t GetTimeMiliseconds() {
    auto curTime = std::chrono::system_clock::now();
    auto sinceEpoch = curTime.time_since_epoch();

    auto miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(sinceEpoch);
    return miliseconds.count(); 
}

void LineTool::apply(booba::Image* image, const booba::Event* event) {
    if (image == nullptr) {
        if (event->type == booba::EventType::ScrollbarMoved) {
            if (event->Oleg.smedata.id == sizeScrollIdx_) {
                lineSize_ = std::max(1, event->Oleg.smedata.value / 2);
            }
        }

        return;
    }

    if (event->type != booba::EventType::MousePressed)
        return;

    CordsPair curClick = {event->Oleg.mbedata.x, event->Oleg.mbedata.y};

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

    sizeScrollIdx_ = booba::createScrollbar(20, 120, 400, 30, 400, 0);
}

void BrushTool::apply(booba::Image* image, const booba::Event* event) {
    if (image == nullptr) {
        if (event->type == booba::EventType::ScrollbarMoved) {
            if (event->Oleg.smedata.id == sizeScrollIdx_) {
                brushSize_ = std::max(1, event->Oleg.smedata.value / 2);
            }
            else if (event->Oleg.smedata.id == transScrollIdx_) {
                transparency_ = double(event->Oleg.smedata.value) / 50.0;
                printf("New trans is %lg\n", transparency_);
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

    if (isClicked_ == 0) {
        return;
    }

    if (GetTimeMiliseconds() < (lastTime_ + delay_)) {
        return;
    }

    CordsPair moveCords = {event->Oleg.motion.x, event->Oleg.motion.y};

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

    sizeScrollIdx_  = booba::createScrollbar(20, 40, 400, 30, 400, 0);
    transScrollIdx_ = booba::createScrollbar(20, 90, 400, 30, 50, 45);
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

    CordsPair moveCords = {event->Oleg.motion.x, event->Oleg.motion.y};


    if ((lastPoint_.x != -1) && (lastPoint_.y != -1)) {
        LineTool::DrawLine(image, lastPoint_, moveCords, {0xffffffff});
    }

    lastPoint_ = moveCords;
    lastTime_  = GetTimeMiliseconds();
}

void RectTool::apply(booba::Image* image, const booba::Event* event)  {
    if (event->type != booba::EventType::MousePressed)
        return;

    CordsPair curClick = {event->Oleg.mbedata.x, event->Oleg.mbedata.y};

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

    CordsPair curClick = {event->Oleg.mbedata.x, event->Oleg.mbedata.y};

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

    CordsPair curClick = {event->Oleg.mbedata.x, event->Oleg.mbedata.y};

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

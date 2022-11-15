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

        DrawBigLine(firstClick_, secondClick, lineSize_, image);

        firstClick_ = {-1, -1};
    }
}

void LineTool::buildSetupWidget() {
    booba::createLabel(20, 20, 300, 40, "Line size");

    sizeScrollIdx_ = booba::createScrollbar(20, 120, 400, 30);
}

void BrushTool::apply(booba::Image* image, const booba::Event* event) {
    if (image == nullptr) {
        if (event->type == booba::EventType::ScrollbarMoved) {
            if (event->Oleg.smedata.id == sizeScrollIdx_) {
                brushSize_ = std::max(1, event->Oleg.smedata.value / 2);
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

    CordsPair moveCords = {event->Oleg.motion.rel_x, event->Oleg.motion.rel_y};

    if ((lastPoint_.x != -1) && (lastPoint_.y != -1)) {
        LineTool::DrawBigLine(lastPoint_, moveCords, brushSize_, image);
    }

    lastPoint_ = moveCords;
    lastTime_  = GetTimeMiliseconds();
}

void BrushTool::buildSetupWidget() {
    booba::createLabel(20, 20, 400, 40, "Brush size");

    sizeScrollIdx_ = booba::createScrollbar(20, 120, 400, 30);
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

    CordsPair moveCords = {event->Oleg.motion.rel_x, event->Oleg.motion.rel_y};


    if ((lastPoint_.x != -1) && (lastPoint_.y != -1)) {
        LineTool::DrawLine(image, lastPoint_, moveCords, {0xffffff00});
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
    printf("INITIALIZING\n");

    LineTool* lineTool     = new LineTool();
    BrushTool* brushTool   = new BrushTool();
    EraserTool* eraserTool = new EraserTool();
    RectTool* rectTool     = new RectTool();
    EllipsTool* ellipsTool = new EllipsTool();
    BucketTool* bucketTool = new BucketTool();

    printf("ADD\n");
    booba::addTool(lineTool);
    booba::addTool(brushTool);
    booba::addTool(eraserTool);
    booba::addTool(rectTool);
    booba::addTool(ellipsTool);
    booba::addTool(bucketTool);
}

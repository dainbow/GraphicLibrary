#include "Image.hpp"

ToolManager::ToolManager(Canvas* canvas, ToolPalette* palette) :
myLine_(canvas), myBrush_(canvas), myEraser_(canvas), myRect_(canvas), myElps_(canvas), myBuck_(canvas),
palette_(palette)
{
    *palette_ += new ToolButton(canvas, &myLine_);
    *palette_ += new ToolButton(canvas, &myBrush_);
    *palette  += new ToolButton(canvas, &myEraser_);
    *palette  += new ToolButton(canvas, &myRect_);
    *palette  += new ToolButton(canvas, &myElps_);
    *palette  += new ToolButton(canvas, &myBuck_);
}

void LineTool::Apply(const Event& curEvent) {
    if (curEvent.type != Event::MouseButtonPressed)

        return;

    CordsPair curClick = parent_->ConvertRealXY({curEvent.mouseButton.x, curEvent.mouseButton.y});
    curClick.x += int32_t(parent_->GetImageXShift());
    curClick.y += int32_t(parent_->GetImageYShift());

    if ((firstClick_.x == -1) && (firstClick_.y == -1)) {
        firstClick_ = curClick;
    }
    else {
        CordsPair secondClick = curClick;

        DrawLine(parent_->image_, firstClick_, secondClick, {0x15171900});

        firstClick_ = {-1, -1};
    }
}

void BrushTool::Apply(const Event& curEvent) {
    if (curEvent.type == Event::MouseButtonPressed) {
        isClicked_ = 1;
        lastPoint_ = {-1, -1};

        return;
    }

    if (curEvent.type == Event::MouseButtonReleased) {
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

    CordsPair moveCords = parent_->ConvertRealXY({curEvent.mouseMove.x, curEvent.mouseMove.y});
    moveCords.x += int32_t(parent_->GetImageXShift());
    moveCords.y += int32_t(parent_->GetImageYShift());

    if ((lastPoint_.x != -1) && (lastPoint_.y != -1)) {
        LineTool::DrawLine(parent_->image_, lastPoint_, moveCords, {0x15171900});
    }

    parent_->SetChanged();
    lastPoint_ = moveCords;
    lastTime_  = GetTimeMiliseconds();
}

void EraserTool::Apply(const Event& curEvent) {
    if (curEvent.type == Event::MouseButtonPressed) {
        isClicked_ = 1;
        lastPoint_ = {-1, -1};

        return;
    }

    if (curEvent.type == Event::MouseButtonReleased) {
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

    CordsPair moveCords = parent_->ConvertRealXY({curEvent.mouseMove.x, curEvent.mouseMove.y});
    moveCords.x += int32_t(parent_->GetImageXShift());
    moveCords.y += int32_t(parent_->GetImageYShift());

    if ((lastPoint_.x != -1) && (lastPoint_.y != -1)) {
        LineTool::DrawLine(parent_->image_, lastPoint_, moveCords, {0xffffff00});
    }

    parent_->SetChanged();
    lastPoint_ = moveCords;
    lastTime_  = GetTimeMiliseconds();
}

void RectTool::Apply(const Event& curEvent) {
    if (curEvent.type != Event::MouseButtonPressed)
        return;

    CordsPair curClick = parent_->ConvertRealXY({curEvent.mouseButton.x, curEvent.mouseButton.y});
    curClick.x += int32_t(parent_->GetImageXShift());
    curClick.y += int32_t(parent_->GetImageYShift());

    if ((firstClick_.x == -1) && (firstClick_.y == -1)) {
        firstClick_ = curClick;
    }
    else {
        CordsPair secondClick = curClick;

        LineTool::DrawLine(parent_->image_, firstClick_, {firstClick_.x, secondClick.y}, {0x15171900});
        LineTool::DrawLine(parent_->image_, {firstClick_.x, secondClick.y}, secondClick, {0x15171900});
        LineTool::DrawLine(parent_->image_, secondClick, {secondClick.x, firstClick_.y}, {0x15171900});
        LineTool::DrawLine(parent_->image_, {secondClick.x, firstClick_.y}, firstClick_, {0x15171900});

        firstClick_ = {-1, -1};
    }
}

void EllipsTool::Apply(const Event& curEvent) {
    if (curEvent.type != Event::MouseButtonPressed)
        return;

    CordsPair curClick = parent_->ConvertRealXY({curEvent.mouseButton.x, curEvent.mouseButton.y});
    curClick.x += int32_t(parent_->GetImageXShift());
    curClick.y += int32_t(parent_->GetImageYShift());

    if ((firstClick_.x == -1) && (firstClick_.y == -1)) {
        firstClick_ = curClick;
    }
    else {
        CordsPair secondClick = curClick;

        DrawEllips(parent_->image_, firstClick_, secondClick, {0x15171900});

        firstClick_ = {-1, -1};
    }
}

void BucketTool::Apply(const Event& curEvent) {
    if (curEvent.type != Event::MouseButtonPressed)
        return;

    CordsPair curClick = parent_->ConvertRealXY({curEvent.mouseButton.x, curEvent.mouseButton.y});
    curClick.x += int32_t(parent_->GetImageXShift());
    curClick.y += int32_t(parent_->GetImageYShift());

    Fill(parent_->image_, curClick, {0x15171900});
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


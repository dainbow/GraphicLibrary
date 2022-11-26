#include "DainCurves.hpp"

HSV ConvertRGBToHSV(const uint32_t rgb) {
    double r = ((rgb & (0xFF << 24u)) >> 24u) / 255.0;
    double g = ((rgb & (0xFF << 16u)) >> 16u) / 255.0;
    double b = ((rgb & (0xFF << 8u))  >> 8u) / 255.0;

    double cmax = std::max(r, std::max(g, b)); // maximum of r, g, b
    double cmin = std::min(r, std::min(g, b)); // minimum of r, g, b
    double diff = cmax - cmin; // diff of cmax and cmin.
    double h = -1, s = -1;
 
    // if cmax and cmax are equal then h = 0
    if (cmax == cmin)
        h = 0;
 
    // if cmax equal r then compute h
    else if (cmax == r)
        h = fmod(60 * ((g - b) / diff) + 360, 360);
 
    // if cmax equal g then compute h
    else if (cmax == g)
        h = fmod(60 * ((b - r) / diff) + 120, 360);
 
    // if cmax equal b then compute h
    else if (cmax == b)
        h = fmod(60 * ((r - g) / diff) + 240, 360);
 
    // if cmax equal zero
    if (cmax == 0)
        s = 0;
    else
        s = (diff / cmax) * 100;
 
    // compute v
    double v = cmax * 100;
    return {h, s, v};
}

uint32_t ConvertHSVToRGB(const HSV& hsv) {
    double H = hsv.H;
    double S = hsv.S;
    double V = hsv.V;

    if ((H > 360) || (H < 0) || (S > 100) || (S < 0)){
        std::cerr <<"The givem HSV values are not in valid range"<< std::endl; 
        return 0;
    }

    double s = S / 100;
    double v = V / 100;
    double C = s * v;
    double X = C * (1 - std::abs(std::fmod(H / 60.0, 2) - 1));
    double m = v - C;
    double r = 0, g = 0, b = 0;

    if ((H >= 0) && (H < 60)) {
        r = C, g = X, b = 0;
    }
    else if ((H >= 60) && (H < 120)) {
        r = X, g = C, b = 0;
    }
    else if ((H >= 120) && (H < 180)) {
        r = 0, g = C, b = X;
    }
    else if ((H >= 180) && (H < 240)) {
        r = 0, g = X, b = C;
    }
    else if ((H >= 240) && (H < 300)) {
        r = X, g = 0, b = C;
    }
    else {
        r = C, g = 0, b = X;
    }
    uint8_t R = uint8_t((r + m) * 255);
    uint8_t G = uint8_t((g + m) * 255);
    uint8_t B = uint8_t((b + m) * 255);

    uint32_t result = (R << 24u) + (G << 16u) + (B << 8u) + 0xff;

    return result;  
}

void CurvesTool::buildSetupWidget() {
    canvasIdx_ = booba::createCanvas(50, 20, CanvasWidth + 1, CanvasHeight + 1);
    resetButtonIdx_ = booba::createButton(50 + CanvasWidth + 20, 20, 70, 30, "Reset");

    verticalGradCanvas_ = booba::createCanvas(20, 20, 30, CanvasHeight + 1);
    horisontalGradCanvas_ = booba::createCanvas(50, 20 + CanvasHeight + 1, CanvasWidth + 1, 30);

    DrawGradients();
    ReDraw();
}

double CurvesTool::CalcNextT(const double lastT, const CordsPair& lastP, const CordsPair& nextP) {
    double dx = nextP.x - lastP.x;
    double dy = nextP.y - lastP.y;

    double l = std::sqrt(dx * dx + dy * dy);
    return lastT + std::pow(l, Alpha);
}

void CurvesTool::DoCatmullRomSpline(const CordsPair& p0, const CordsPair& p1, const CordsPair& p2, const CordsPair& p3) {
    double t0 = 0;
    double t1 = CalcNextT(t0, p0, p1);
    double t2 = CalcNextT(t1, p1, p2);
    double t3 = CalcNextT(t2, p2, p3);

    double timeBuffer = t1;
    double timeStep = (t2 - t1) / double(PointsAmount - 1);

    for (uint64_t curIdx = 0; curIdx < PointsAmount; curIdx++) {
        CordsPair a1 = p0 * ((t1 - timeBuffer) / (t1 - t0)) + p1 * ((timeBuffer - t0) / (t1 - t0));
        CordsPair a2 = p1 * ((t2 - timeBuffer) / (t2 - t1)) + p2 * ((timeBuffer - t1) / (t2 - t1));
        CordsPair a3 = p2 * ((t3 - timeBuffer) / (t3 - t2)) + p3 * ((timeBuffer - t2) / (t3 - t2));
        CordsPair b1 = a1 * ((t2 - timeBuffer) / (t2 - t0)) + a2 * ((timeBuffer - t0) / (t2 - t0));
        CordsPair b2 = a2 * ((t3 - timeBuffer) / (t3 - t1)) + a3 * ((timeBuffer - t1) / (t3 - t1));

        CordsPair point = b1 * ((t2 - timeBuffer) / (t2 - t1)) + b2 * ((timeBuffer - t1) / (t2 - t1)); 
        interPoints_.push_back({point.x, point.y});

        timeBuffer += timeStep;
    }
}

void CurvesTool::DrawDefaultCanvas() {
    for (uint64_t curX = 0; curX < CanvasWidth; curX++) {
        for (uint64_t curY = 0; curY < CanvasHeight; curY++) {
            if (((curX % 50) == 0) || ((curY % 50) == 0)) {
                booba::putPixel(canvasIdx_, curX, curY, 0xffffffff);
            }
            else {
                booba::putPixel(canvasIdx_, curX, curY, 0x000000ff);
            }
        }
    }
}

void CurvesTool::ReDraw() {
    interPoints_.clear();
    std::sort(basePoints_.begin(), basePoints_.end());

    CordsPair points[4] = {basePoints_[0], basePoints_[1], basePoints_[2]};

    for (uint64_t curPoint = 0; curPoint < (basePoints_.size() - 3); curPoint++) {
        points[(curPoint + 3) % 4] = basePoints_[curPoint + 3];

        DoCatmullRomSpline(points[curPoint % 4], points[(curPoint + 1) % 4], points[(curPoint + 2) % 4], points[(curPoint + 3) % 4]);
    }

    DrawDefaultCanvas();
    for (auto& curPoint : interPoints_) {
        booba::putPixel(canvasIdx_, curPoint.x, curPoint.y, 0xffffffff);
    }

    for (auto& curPoint : basePoints_) {
        for (int64_t curXDiff = -5; curXDiff < 6; curXDiff++) {
            for (int64_t curYDiff = -5; curYDiff < 6; curYDiff++) {
                if ((curYDiff < 0) && (curPoint.y > (-curYDiff))) {
                    if ((curXDiff < 0) && (curPoint.x > (-curXDiff))) {
                        booba::putPixel(canvasIdx_, curPoint.x + curXDiff, curPoint.y + curYDiff, 0xffffffff);
                    }
                    else if ((curXDiff > 0) && ((curPoint.x + curXDiff) < CanvasWidth)) {
                        booba::putPixel(canvasIdx_, curPoint.x + curXDiff, curPoint.y + curYDiff, 0xffffffff);
                    }
                }
                else if ((curYDiff > 0) && ((curPoint.y + curYDiff) < CanvasHeight)) {
                    if ((curXDiff < 0) && (curPoint.x > (-curXDiff))) {
                        booba::putPixel(canvasIdx_, curPoint.x + curXDiff, curPoint.y + curYDiff, 0xffffffff);
                    }
                    else if ((curXDiff > 0) && ((curPoint.x + curXDiff) < CanvasWidth)) {
                        booba::putPixel(canvasIdx_, curPoint.x + curXDiff, curPoint.y + curYDiff, 0xffffffff);
                    }
                }            
            }
        }
    }
}

void CurvesTool::DrawGradients() {
    for (uint64_t curY = 0; curY < (CanvasHeight + 1); curY++) {
        uint32_t curColor = ConvertHSVToRGB({0, 0, 100.0 - (double(curY) / double(CanvasHeight + 1) * 100.0)});

        for (uint64_t curX = 0; curX < 30; curX++) {
            booba::putPixel(verticalGradCanvas_, curX, curY, curColor);
        }
    }

    for (uint64_t curX = 0; curX < (CanvasWidth + 1); curX++) {
        uint32_t curColor = ConvertHSVToRGB({0, 0, double(curX) / double(CanvasWidth + 1) * 100.0});

        for (uint64_t curY = 0; curY < 30; curY++) {
            booba::putPixel(horisontalGradCanvas_, curX, curY, curColor);
        }
    }
}

double CurvesTool::ConvertV(const double V) {
    const double convertedV = (V / 100.0) * double(CanvasWidth);

    uint32_t left  = 0;
    uint32_t right = interPoints_.size();

    while (left < right) {
        uint32_t middle = (left + right) / 2;
        if (interPoints_[middle].x < convertedV) {
            left = middle + 1;
        }
        else if (interPoints_[middle].x > convertedV) {
            right = middle - 1;
        }
        else {
            left  = middle;
            right = middle;

            break;
        }
    } 

    return ((double(CanvasHeight) - interPoints_[left].y) / double(CanvasHeight)) * 100.0;
}

void CurvesTool::apply(booba::Image* image, const booba::Event* event) {
    if (image == nullptr) {
        if (event->type == booba::EventType::CanvasMPressed) {
            if (event->Oleg.cedata.id == canvasIdx_) {
                basePoints_.push_back({double(event->Oleg.cedata.x), double(event->Oleg.cedata.y)});

                ReDraw();
            }
        }
        else if (event->type == booba::EventType::ButtonClicked) {
            std::cout << "HERE";
            if (event->Oleg.bcedata.id == resetButtonIdx_) {
                ResetBasePoints();

                ReDraw();
            }
        }

        return;
    }

    if (event->type == booba::EventType::MousePressed) {
        for (uint64_t curX = 0; curX < image->getX(); curX++) {
            for (uint64_t curY = 0; curY < image->getH(); curY++) {
                HSV curHsv = ConvertRGBToHSV(image->getPixel(curX, curY));

                image->putPixel(curX, curY, ConvertHSVToRGB({curHsv.H, curHsv.S, ConvertV(curHsv.V)}));
            }
        }
    }
}

void booba::init_module() {
    CurvesTool* curvesTool = new CurvesTool();

    booba::addTool(curvesTool);
}


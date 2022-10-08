#include "Main.hpp"

int main() {
    RealWindow mainWindow(1000, 500, 0x8a7f8e00);

    //
    // TRACER INIT
    //

    Raytracer* tracer = new Raytracer(20, 20, 460, 460, 1, 4, {0, 0, 0}, 10, 10, 10);
    mainWindow += tracer;

    //
    // GRAPHICS DROPLIST CONFIG
    //

    DropList* graphicsButton = new DropList(0, 0, 70, 30, 0, 0, 0x00FF0000);
    graphicsButton->SetText("Tools", 0xfefe2200);
    mainWindow += graphicsButton;

    ControlGraphics control = {tracer, nullptr, nullptr, nullptr};
    
    //
    // TRACER OBJECTLIST CONFIG
    //

    List* objectsList = new List(520, 20, 300, 460, 0xffa8af00, 0x1e90ff00);
    mainWindow += objectsList;

    Button* sampleButton = new Button(0, 0, 200, 50, 0, 0, 0xFFFFFF00);
    *objectsList += sampleButton;

    //
    // ButtonToAddObjects CONFIG
    //

    CustomButton<List>* buttonToAddObjects = new CustomButton<List>(840, 20, 30, 30, 0, 0, 0xffff0000, objectsList);
    buttonToAddObjects->onClick_ += new FuncCaller<CustomButton<List>, Vector>(buttonToAddObjects, AddButtonToList);
    mainWindow += buttonToAddObjects;

    //
    // LOW GRAPHICS BUTTON CONFIG
    //

    CustomButton<ControlGraphics>* lowGraphics = new CustomButton<ControlGraphics>(0, 0, 70, 20, 0, 0, 0x99FF9900, &control);
    lowGraphics->onClick_ += new FuncCaller<CustomButton<ControlGraphics>, Vector>(lowGraphics, LowGraphicsOnClick);
    lowGraphics->SetText("Low", 0xfefe2200);
    *graphicsButton += lowGraphics;

    //
    // MID GRAPHICS BUTTON CONFIG
    //

    CustomButton<ControlGraphics>* midGraphics = new CustomButton<ControlGraphics>(0, 0, 70, 20, 0, 0, 0xFFA81200, &control);
    midGraphics->onClick_ += new FuncCaller<CustomButton<ControlGraphics>, Vector>(midGraphics, MidGraphicsOnClick);
    midGraphics->SetText("Middle", 0xfefe2200);
    *graphicsButton += midGraphics;

    //
    // HIGH GRAPHICS BUTTON CONFIG
    //

    CustomButton<ControlGraphics>* highGraphics = new CustomButton<ControlGraphics>(0, 0, 70, 20, 0, 0, 0xff2b2b00, &control);
    highGraphics->onClick_ += new FuncCaller<CustomButton<ControlGraphics>, Vector>(highGraphics, HighGraphicsOnClick);
    highGraphics->SetText("High", 0xfefe2200);
    *graphicsButton += highGraphics;

    control = {tracer, lowGraphics, midGraphics, highGraphics};

    //
    // TRACER CONFIG
    //

    ConstColor red   = {{0.7, 0.3, 0.3}};
    ConstColor green = {{0.8, 0.6, 0.2}};
    ConstColor blue  = {{0.8, 0.8, 0.8}};
    ConstColor white   = {0xffffff00};
    ConstColor lazur   = {0x0b083f00};
    ConstColor realRed = {0xff2b2b00};
    ConstColor realGreen = {0x13880800};
    ConstColor realBlack = {0x161a1e00};
    ConstColor pink      = {0xFF149300};

    DiffLight whiteLight = {&white};

    Squares kvadratiki = {&white, &lazur, 3};

    Mirror redMirror = {&red};
    Mirror greenMirror = {&green};
    Mirror blueMirror = {&blue};
    Mirror pinkMirror = {&pink};

    Scattering squareScat = {&kvadratiki};
    Scattering whiteScat  = {&white};
    Scattering redScat    = {&realRed};
    Scattering greenScat  = {&realGreen};
    Scattering blackScat  = {&realBlack};

    Transparent glass(1.5);

    Sphere* sph1 = new Sphere({-11, 13, 32}, 2, &redMirror);
    *tracer += sph1;

    Sphere* sph2 = new Sphere({-5, 9.9, 37}, 5, &greenMirror);
    *tracer += sph2;

    Sphere* sph3 = new Sphere({8, 9.9, 37}, 5, &glass);
    *tracer += sph3;

    Sphere* sph5 = new Sphere({5, 4.9, 60}, 10, &pinkMirror);
    *tracer += sph5;

    while (mainWindow.IsOpen()) {
        mainWindow.Clear();

        mainWindow.PollEvent();

        mainWindow.Display();
    }

    return 0;
    // ConstColor red   = {{0.7, 0.3, 0.3}};
    // ConstColor green = {{0.8, 0.6, 0.2}};
    // ConstColor blue  = {{0.8, 0.8, 0.8}};
    // ConstColor white   = {0xffffff00};
    // ConstColor lazur   = {0x0b083f00};
    // ConstColor realRed = {0xff2b2b00};
    // ConstColor realGreen = {0x13880800};
    // ConstColor realBlack = {0x161a1e00};
    // ConstColor pink      = {0xFF149300};

    // DiffLight whiteLight = {&white};

    // Squares kvadratiki = {&white, &lazur, 3};

    // Mirror redMirror = {&red};
    // Mirror greenMirror = {&green};
    // Mirror blueMirror = {&blue};
    // Mirror pinkMirror = {&pink};

    // Scattering squareScat = {&kvadratiki};
    // Scattering whiteScat  = {&white};
    // Scattering redScat    = {&realRed};
    // Scattering greenScat  = {&realGreen};
    // Scattering blackScat  = {&realBlack};

    

    // Window window(windowWidth, windowHeight);
    
    // Vector3D cameraCoords = {0, 0, 0};


    // // 0x10901000
    // Sphere sph1({-11, 13, 32}, 2, &redMirror);
    // Sphere sph2({-5, 9.9, 37}, 5, &greenMirror);
    // Sphere sph3({8, 9.9, 37}, 5, &glass);
    // Sphere sph5({5, 4.9, 60}, 10, &pinkMirror);

    // Sphere sph4({-5, -6.8, 45}, 5, &whiteLight);

    // Plane pln1 = GetXZPlane(&squareScat, 15, -15, 15, 10, 60);
    // Plane pln2 = GetXYPlane(&whiteScat, 60, -15, 15);
    // Plane pln3 = GetZYPlane(&redScat, -15, 10, 60);
    // Plane pln4 = GetZYPlane(&greenScat, 15, 10, 60);
    // Plane pln6 = GetXZPlane(&squareScat, -12, -15, 15, 10, 60);

    // const uint32_t objectsAmount = 10;
    // BaseObject* objects[objectsAmount]  = {&pln1, &pln2, &pln3, &pln4, &pln6, &sph1, &sph2, &sph3, &sph4, &sph5};

    // std::array<std::thread, uint64_t(threadsAmount)> threads;

    // MyColor** rawData = new MyColor*[uint32_t(windowHeight)];
    // for (double curRow = 0; curRow < windowHeight; curRow++) {
    //     rawData[uint32_t(curRow)] = new MyColor[uint32_t(windowWidth)];
    // }

    // while(window.IsOpen()) {
    //     if (PollEvent(window)) 
    //         break;

    //     window.Clear();

    //     for (double curY = -(windowHeight / 2); curY < (windowHeight / 2); curY += threadsAmount) {
    //         for (double curThread = 0; (curThread < threadsAmount) && ((curY + curThread) < (windowHeight / 2)); curThread++) {
    //             threads[uint32_t(curThread)] = std::thread(ProcessRow, rawData[uint32_t(curY + curThread + (windowHeight / 2))], curY + curThread,
    //                                                        cameraCoords, &objects, objectsAmount);
    //         }

    //         for (double curThread = 0; (curThread < threadsAmount) && ((curY + curThread) < (windowHeight / 2)); curThread++) {
    //             threads[uint32_t(curThread)].join();
    //         }

    //         for (double curRenderY = curY; (curRenderY < (curY + threadsAmount)) && (curRenderY < (windowHeight / 2)); curRenderY++) {
    //             for (double curX = -(windowWidth / 2); curX < (windowWidth / 2); curX++) {
    //                 MyColor newColor = rawData[uint32_t(curRenderY + (windowHeight / 2))][uint32_t(curX + (windowWidth / 2))];
    //                 MyColor gammaColor = {sqrt(newColor.fRed_),
    //                                       sqrt(newColor.fGreen_),
    //                                       sqrt(newColor.fBlue_)};


    //                 curImage.SetPixel(uint32_t(curX + (windowWidth / 2)), uint32_t(curRenderY + (windowHeight / 2)), gammaColor);
    //             }
    //         }

    //         window.DrawImage(curImage, {0, 0});
    //         window.Display();
    //     }
    // }
}

void LowGraphicsOnClick(CustomButton<ControlGraphics>* button, const Vector& vec) {
    if (!button->IsClicked(vec)) {
        return;
    }

    if (button->GetContext()->tracer_->GetGraphics() == TracerGraphics::LowGraphics) {
        return;
    }

    button->GetContext()->tracer_->SetLowGraphics();

    button->isClicked_ = 1;
    button->GetContext()->midButton_->isClicked_  = 0;
    button->GetContext()->highButton_->isClicked_ = 0;
}

void MidGraphicsOnClick(CustomButton<ControlGraphics>* button, const Vector& vec) {
    if (!button->IsClicked(vec)) {
        return;
    }

    if (button->GetContext()->tracer_->GetGraphics() == TracerGraphics::MidGraphics) {
        return;
    }

    button->GetContext()->tracer_->SetMidGraphics();

    button->isClicked_ = 1;
    button->GetContext()->lowButton_->isClicked_  = 0;
    button->GetContext()->highButton_->isClicked_ = 0;
}

void HighGraphicsOnClick(CustomButton<ControlGraphics>* button, const Vector& vec) {
    if (!button->IsClicked(vec)) {
        return;
    }

    if (button->GetContext()->tracer_->GetGraphics() == TracerGraphics::HightGraphics) {
        return;
    }

    button->GetContext()->tracer_->SetHighGraphics();

    button->isClicked_ = 1;
    button->GetContext()->lowButton_->isClicked_ = 0;
    button->GetContext()->midButton_->isClicked_ = 0;
}

uint32_t iter = 0;

void AddButtonToList(CustomButton<List>* button, const Vector& vec) {
    if (!button->IsClicked(vec))
        return;

    Button* newBut = new Button(0, 0, 200, 50, 0, 0, 0xffffff00); 

    char buf[11] = "";
    snprintf(buf, 11, "%u", iter++);
    std::string newText  = buf;

    newBut->SetText(newText, 0xffffff00);

    *button->GetContext() += newBut;
}

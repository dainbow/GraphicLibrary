#include "Main.hpp"

int main() {
    SkinManager manager("default");
    RealWindow mainWindow(1000, 500, &manager);

    //
    // TRACER OBJECTLIST CONFIG
    //

    List* objectsList = new List(520, 20, 250, 400);
    mainWindow += objectsList;

    //
    // TRACER INIT
    //

    Raytracer* tracer = new Raytracer(0, 0, 460, 460, 1, 4, {0, 0, 0}, 10, 10, 10, objectsList);

    //
    // TracerWindow Init
    //

    Window* tracerWindow = new Window(20, 20, 400, 400);
    *tracerWindow += tracer;
    mainWindow += tracerWindow;

    //
    // ProgressBar Init
    //

    ProgressBar* traceProgress = new ProgressBar(20, 440, 400, 30, &tracer->renderRow_, -double(tracer->GetHeight()) / 2, double(tracer->GetHeight()) / 2);
    mainWindow += traceProgress;

    //
    // GRAPHICS DROPLIST CONFIG
    //

    DropList* graphicsButton = new DropList(0, 0, 70, 30);
    graphicsButton->SetText("Modes", 0xfefe2200);
    mainWindow += graphicsButton;

    ControlGraphics control = {tracer, nullptr, nullptr, nullptr};


    //
    // ButtonToAddObjects CONFIG
    //

    DropList* addObject = new DropList(70, 0, 70, 30);
    addObject->SetText("Add", 0xfefe2200);
    mainWindow += addObject;

    //
    // ButtonToAddSphere CONFIG
    //

    CustomButton<Raytracer>* buttonToAddSphere = new CustomButton<Raytracer>(840, 20, 70, 30, tracer);
    buttonToAddSphere->SetHandler(new FuncCaller<CustomButton<Raytracer>, CordsPair>(buttonToAddSphere, AddSphereToTracer));
    buttonToAddSphere->SetText("Sphere", 0xfefe2200);
    *addObject += buttonToAddSphere;

    //
    // ButtonToAddPlane Config
    //

    CustomButton<Raytracer>* buttonToAddPlane = new CustomButton<Raytracer>(840, 20, 70, 30, tracer);
    buttonToAddPlane->SetHandler(new FuncCaller<CustomButton<Raytracer>, CordsPair>(buttonToAddPlane, AddPlaneToTracer));
    buttonToAddPlane->SetText("Plane", 0xfefe2200);
    *addObject += buttonToAddPlane;

    //
    // LOW GRAPHICS BUTTON CONFIG
    //

    CustomButton<ControlGraphics>* lowGraphics = new CustomButton<ControlGraphics>(0, 0, 70, 20, &control);
    lowGraphics->SetHandler(new FuncCaller<CustomButton<ControlGraphics>, CordsPair>(lowGraphics, LowGraphicsOnClick));
    lowGraphics->SetText("Low", 0xfefe2200);
    *graphicsButton += lowGraphics;

    //
    // MID GRAPHICS BUTTON CONFIG
    //

    CustomButton<ControlGraphics>* midGraphics = new CustomButton<ControlGraphics>(0, 0, 70, 20, &control);
    midGraphics->SetHandler(new FuncCaller<CustomButton<ControlGraphics>, CordsPair>(midGraphics, MidGraphicsOnClick));
    midGraphics->SetText("Middle", 0xfefe2200);
    *graphicsButton += midGraphics;

    //
    // HIGH GRAPHICS BUTTON CONFIG
    //

    CustomButton<ControlGraphics>* highGraphics = new CustomButton<ControlGraphics>(0, 0, 70, 20, &control);
    highGraphics->SetHandler(new FuncCaller<CustomButton<ControlGraphics>, CordsPair>(highGraphics, HighGraphicsOnClick));
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
    sph1->name_ = "LittleSphere";
    *tracer += sph1;

    Sphere* sph2 = new Sphere({-5, 9.9, 37}, 5, &greenMirror);
    sph2->name_ = "MidSphere";
    *tracer += sph2;

    Sphere* sph3 = new Sphere({8, 9.9, 37}, 5, &glass);
    sph3->name_ = "BigGlassSphere";
    *tracer += sph3;

    Sphere* sph5 = new Sphere({5, 4.9, 60}, 10, &squareScat);

    sph5->name_ = "BigPinkSphere";
    *tracer += sph5;

    Plane* pln1 = GetXZPlane(&squareScat, 15, -15, 15, 10, 60);
    pln1->name_ = "SquaredFloor";
    *tracer += pln1;

    //
    // Promo config
    //

    Promotion* promo1 = new Promotion();
    mainWindow += promo1;

    Promotion* promo2 = new Promotion();
    mainWindow += promo2;

    Promotion* promo3 = new Promotion();
    mainWindow += promo3;

    Promotion* promo4 = new Promotion();
    mainWindow += promo4;

    Promotion* promo5 = new Promotion();
    mainWindow += promo5;

    Promotion* promo6 = new Promotion();
    mainWindow += promo6;

    Promotion* promo7 = new Promotion();
    mainWindow += promo7;


    while (mainWindow.IsOpen()) {
        mainWindow.Clear();

        mainWindow.PollEvent();

        mainWindow.Display();
    }
}

void LowGraphicsOnClick(CustomButton<ControlGraphics>* button, const CordsPair& vec) {
    if (!button->IsClicked(vec)) {
        return;
    }

    if (button->GetContext()->tracer_->GetGraphics() == TracerGraphics::LowGraphics) {
        return;
    }

    button->GetContext()->tracer_->SetLowGraphics();

    button->SetClickedState(1);
    button->GetContext()->midButton_->SetClickedState(0);
    button->GetContext()->highButton_->SetClickedState(0);
}

void MidGraphicsOnClick(CustomButton<ControlGraphics>* button, const CordsPair& vec) {
    if (!button->IsClicked(vec)) {
        return;
    }

    if (button->GetContext()->tracer_->GetGraphics() == TracerGraphics::MidGraphics) {
        return;
    }

    button->GetContext()->tracer_->SetMidGraphics();

    button->SetClickedState(1);
    button->GetContext()->lowButton_->SetClickedState(0);
    button->GetContext()->highButton_->SetClickedState(0);
}

void HighGraphicsOnClick(CustomButton<ControlGraphics>* button, const CordsPair& vec) {
    if (!button->IsClicked(vec)) {
        return;
    }

    if (button->GetContext()->tracer_->GetGraphics() == TracerGraphics::HightGraphics) {
        return;
    }

    button->GetContext()->tracer_->SetHighGraphics();

    button->SetClickedState(1);
    button->GetContext()->lowButton_->SetClickedState(0);
    button->GetContext()->midButton_->SetClickedState(0);
}

void AddSphereToTracer(CustomButton<Raytracer>* button, const CordsPair& vec) {
    if (!button->IsClicked(vec))
        return;

    ConstColor* white      = new ConstColor(0xffffff00);
    Scattering* whiteScat  = new Scattering(white);

    *button->GetContext() += new Sphere({0, 0, 0}, 0, whiteScat);
}

void AddPlaneToTracer(CustomButton<Raytracer>* button, const CordsPair& vec) {
    if (!button->IsClicked(vec))
        return;


    ConstColor* white      = new ConstColor(0xffffff00);
    Scattering* whiteScat  = new Scattering(white);

    *button->GetContext() += GetXZPlane(whiteScat, 0);
}

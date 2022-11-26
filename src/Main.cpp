#include "Main.hpp"

booba::ApplicationContext* booba::APPCONTEXT = nullptr;

int main() {
    // std::cout << ConvertHSVToRGB({0, 100, 100}) << std::endl;
    // return 0;

    SkinManager::GetInstance("default");
    RealWindow mainWindow(1600, 900);

    App::GetInstance() += &mainWindow;

    //
    // APPCONTEXT INIT
    //
    
    booba::APPCONTEXT = new booba::ApplicationContext();
    booba::APPCONTEXT->bgColor = 0xffffffff;
    booba::APPCONTEXT->fgColor = 0x000000ff;

    //
    // TRACER OBJECTLIST CONFIG
    //

    List* objectsList = new List(1300, 20, 250, 860);
    mainWindow += objectsList;

    //
    // TOOL PALETTE CONFIG
    //

    ToolPalette* tracerPalette = new ToolPalette(20, 50);
    mainWindow += tracerPalette;

    //
    // COLORS DISPLAY
    //

    ColorWatcher* fgWatcher = new ColorWatcher(100, 50, 40, 40, &booba::APPCONTEXT->fgColor);
    mainWindow += fgWatcher;

    ColorWatcher* bgWatcher = new ColorWatcher(120, 70, 40, 40, &booba::APPCONTEXT->bgColor);
    mainWindow += bgWatcher;

    //
    // TRACER INIT
    //

    Raytracer* tracer = new Raytracer(0, 0, 1000, 1000, 1, 4, {0, 0, 0}, 10, 10, 10, tracerPalette, objectsList);

    //
    // TracerWindow Init
    //

    Window* tracerWindow = new Window(400, 20, 800, 800);
    *tracerWindow += tracer;
    mainWindow += tracerWindow;

    //
    // ProgressBar Init
    //

    ProgressBar* traceProgress = new ProgressBar(400, 840, 800, 30, &tracer->renderRow_, -double(tracer->GetHeight()) / 2, double(tracer->GetHeight()) / 2);
    mainWindow += traceProgress;

    //
    // GRAPHICS DROPLIST CONFIG
    //

    DropList* graphicsButton = new DropList(0, 0, 70, 30);
    graphicsButton->SetText("Modes", 0xfefe22ff);
    mainWindow += graphicsButton;

    ControlGraphics control = {tracer, nullptr, nullptr, nullptr};


    //
    // ButtonToAddObjects CONFIG
    //

    DropList* addObject = new DropList(70, 0, 70, 30);
    addObject->SetText("Add", 0xfefe22ff);
    mainWindow += addObject;

    //
    // ButtonToAddSphere CONFIG
    //

    CustomButton<Raytracer>* buttonToAddSphere = new CustomButton<Raytracer>(840, 20, 70, 30, tracer);
    buttonToAddSphere->SetHandler(new FuncCaller<CustomButton<Raytracer>, CordsPair>(buttonToAddSphere, AddSphereToTracer));
    buttonToAddSphere->SetText("Sphere", 0xfefe22ff);
    *addObject += buttonToAddSphere;

    //
    // ButtonToAddPlane Config
    //

    CustomButton<Raytracer>* buttonToAddPlane = new CustomButton<Raytracer>(840, 20, 70, 30, tracer);
    buttonToAddPlane->SetHandler(new FuncCaller<CustomButton<Raytracer>, CordsPair>(buttonToAddPlane, AddPlaneToTracer));
    buttonToAddPlane->SetText("Plane", 0xfefe22ff);
    *addObject += buttonToAddPlane;

    //
    // LOW GRAPHICS BUTTON CONFIG
    //

    CustomButton<ControlGraphics>* lowGraphics = new CustomButton<ControlGraphics>(0, 0, 70, 20, &control);
    lowGraphics->SetHandler(new FuncCaller<CustomButton<ControlGraphics>, CordsPair>(lowGraphics, LowGraphicsOnClick));
    lowGraphics->SetText("Low", 0xfefe22ff);
    *graphicsButton += lowGraphics;

    //
    // MID GRAPHICS BUTTON CONFIG
    //

    CustomButton<ControlGraphics>* midGraphics = new CustomButton<ControlGraphics>(0, 0, 70, 20, &control);
    midGraphics->SetHandler(new FuncCaller<CustomButton<ControlGraphics>, CordsPair>(midGraphics, MidGraphicsOnClick));
    midGraphics->SetText("Middle", 0xfefe22ff);
    *graphicsButton += midGraphics;

    //
    // HIGH GRAPHICS BUTTON CONFIG
    //

    CustomButton<ControlGraphics>* highGraphics = new CustomButton<ControlGraphics>(0, 0, 70, 20, &control);
    highGraphics->SetHandler(new FuncCaller<CustomButton<ControlGraphics>, CordsPair>(highGraphics, HighGraphicsOnClick));
    highGraphics->SetText("High", 0xfefe22ff);
    *graphicsButton += highGraphics;

    control = {tracer, lowGraphics, midGraphics, highGraphics};

    //
    // COLOR DROPLIST CONFIG
    //

    DropList* colorDrop = new DropList(140, 0, 70, 30);
    colorDrop->SetText("Color", 0xfefe22ff);
    mainWindow += colorDrop;

    //
    // SELECT COLOR CONFIG
    //

    CustomButton<booba::ApplicationContext>* selectTool = new CustomButton<booba::ApplicationContext>(0, 0, 70, 20, booba::APPCONTEXT);
    selectTool->SetHandler(new FuncCaller<CustomButton<booba::ApplicationContext>, CordsPair>(selectTool, SelectColor));
    selectTool->SetText("Select", 0xfefe22ff);
    *colorDrop += selectTool;

    //
    // SWAP COLOR CONFIG
    //

    Button* swapButton = new Button(0, 0, 70, 20);
    swapButton->SetText("Swap", 0xfefe22ff);
    swapButton->SetHandler(new FuncCaller<Button, CordsPair>(swapButton, SwapColors));
    *colorDrop += swapButton;

    //
    // TRACER CONFIG
    //

    ConstColor red   = {{0.7, 0.3, 0.3}};
    ConstColor green = {{0.8, 0.6, 0.2}};
    ConstColor blue  = {{0.8, 0.8, 0.8}};
    ConstColor white   = {0xffffffff};
    ConstColor lazur   = {0x0b083fff};
    ConstColor realRed = {0xff2b2bff};
    ConstColor realGreen = {0x138808ff};
    ConstColor realBlack = {0x161a1eff};
    ConstColor pink      = {0xFF1493ff};

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
    tracer->AddObject(sph1);

    Sphere* sph2 = new Sphere({-5, 9.9, 37}, 5, &greenMirror);
    sph2->name_ = "MidSphere";
    tracer->AddObject(sph2);

    Sphere* sph3 = new Sphere({8, 9.9, 37}, 5, &glass);
    sph3->name_ = "BigGlassSphere";
    tracer->AddObject(sph3);

    Sphere* sph5 = new Sphere({5, 4.9, 60}, 10, &squareScat);

    sph5->name_ = "BigPinkSphere";
    tracer->AddObject(sph5);

    Plane* pln1 = GetXZPlane(&squareScat, 15, -15, 15, 10, 60);
    pln1->name_ = "SquaredFloor";
    tracer->AddObject(pln1);

    //
    // Promo config
    //

    // Promotion* promo1 = new Promotion();
    // mainWindow += promo1;

    // Promotion* promo2 = new Promotion();
    // mainWindow += promo2;

    // Promotion* promo3 = new Promotion();
    // mainWindow += promo3;

    // Promotion* promo4 = new Promotion();
    // mainWindow += promo4;

    // Promotion* promo5 = new Promotion();
    // mainWindow += promo5;

    // Promotion* promo6 = new Promotion();
    // mainWindow += promo6;

    // Promotion* promo7 = new Promotion();
    // mainWindow += promo7;


    while (App::GetInstance().IsOpen()) {
        App::GetInstance().Clear();

        App::GetInstance().PollEvent();

        App::GetInstance().Display();
    }

    FILE* serialFile = fopen("TracerConfig.txt", "w");
    tracer->Serialize(serialFile, 0);

    fclose(serialFile);
    delete booba::APPCONTEXT;
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

    ConstColor* white      = new ConstColor(0xffffffff);
    Scattering* whiteScat  = new Scattering(white);

    button->GetContext()->AddObject(new Sphere({0, 0, 0}, 0, whiteScat));
}

void AddPlaneToTracer(CustomButton<Raytracer>* button, const CordsPair& vec) {
    if (!button->IsClicked(vec))
        return;


    ConstColor* white      = new ConstColor(0xffffffff);
    Scattering* whiteScat  = new Scattering(white);

    button->GetContext()->AddObject(GetXZPlane(whiteScat, 0));
}

void CloseWindow(CustomButton<RealWindow>* button, const CordsPair& vec) {
    if (!button->IsClicked(vec)) {
        return;
    }

    button->GetContext()->Close();
}

void SwapColors(Button* button, const CordsPair& vec) {
    if (!button->IsClicked(vec)) {
        return;
    }

    std::swap(booba::APPCONTEXT->bgColor, booba::APPCONTEXT->fgColor);
}

void SelectColor(CustomButton<booba::ApplicationContext>* button, const CordsPair& vec) {
    if (!button->IsClicked(vec))
        return;


    RealWindow popUp(500, 500);

    GradientChooser* choose = new GradientChooser(20, 20, 400, 400, 0);
    popUp += choose;

    HChooser* hChooser = new HChooser(440, 20, 20, 400, choose);
    popUp += hChooser;

    CustomButton<RealWindow>* closeButton = new CustomButton<RealWindow>(20, 430, 100, 40, &popUp);
    closeButton->SetText("CLOSE", 0xFF0000ff);
    closeButton->SetHandler(new FuncCaller<CustomButton<RealWindow>, CordsPair>(closeButton, CloseWindow));
    popUp += closeButton;

    Button* swapButton = new Button(140, 430, 100, 40);
    swapButton->SetText("SWAP", 0x00FF0000);
    swapButton->SetHandler(new FuncCaller<Button, CordsPair>(swapButton, SwapColors));
    popUp += swapButton;

    ColorWatcher* fgWatcher = new ColorWatcher(260, 430, 40, 40, &booba::APPCONTEXT->fgColor);
    popUp += fgWatcher;

    ColorWatcher* bgWatcher = new ColorWatcher(280, 450, 40, 40, &booba::APPCONTEXT->bgColor);
    popUp += bgWatcher;

    while (popUp.IsOpen()) {
        popUp.Clear();

        popUp.PollEvent();

        popUp.Display();
    }
}

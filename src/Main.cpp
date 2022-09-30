#include "Main.hpp"

int main() {
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

    Window window(windowWidth, windowHeight);
    
    Vector3D cameraCoords = {0, 0, 0};

    Image curImage = {};
    curImage.Create(windowWidth, windowHeight, {0x00, 0x00, 0x00});

    // 0x10901000
    Sphere sph1({-11, 13, 32}, 2, &redMirror);
    Sphere sph2({-5, 9.9, 37}, 5, &greenMirror);
    Sphere sph3({8, 9.9, 37}, 5, &glass);
    Sphere sph5({5, 4.9, 60}, 10, &pinkMirror);

    Sphere sph4({-5, -6.8, 45}, 5, &whiteLight);

    Plane pln1 = GetXZPlane(&squareScat, 15, -15, 15, 10, 60);
    Plane pln2 = GetXYPlane(&whiteScat, 60, -15, 15);
    Plane pln3 = GetZYPlane(&redScat, -15, 10, 60);
    Plane pln4 = GetZYPlane(&greenScat, 15, 10, 60);
    Plane pln6 = GetXZPlane(&squareScat, -12, -15, 15, 10, 60);

    const uint32_t objectsAmount = 10;
    BaseObject* objects[objectsAmount]  = {&pln1, &pln2, &pln3, &pln4, &pln6, &sph1, &sph2, &sph3, &sph4, &sph5};

    std::array<std::thread, uint64_t(threadsAmount)> threads;

    MyColor** rawData = new MyColor*[uint32_t(windowHeight)];
    for (double curRow = 0; curRow < windowHeight; curRow++) {
        rawData[uint32_t(curRow)] = new MyColor[uint32_t(windowWidth)];
    }

    while(window.IsOpen()) {
        if (PollEvent(window)) 
            break;

        window.Clear();

        for (double curY = -(windowHeight / 2); curY < (windowHeight / 2); curY += threadsAmount) {
            for (double curThread = 0; (curThread < threadsAmount) && ((curY + curThread) < (windowHeight / 2)); curThread++) {
                threads[uint32_t(curThread)] = std::thread(ProcessRow, rawData[uint32_t(curY + curThread + (windowHeight / 2))], curY + curThread,
                                                           cameraCoords, &objects, objectsAmount);
            }

            for (double curThread = 0; (curThread < threadsAmount) && ((curY + curThread) < (windowHeight / 2)); curThread++) {
                threads[uint32_t(curThread)].join();
            }

            for (double curRenderY = curY; (curRenderY < (curY + threadsAmount)) && (curRenderY < (windowHeight / 2)); curRenderY++) {
                for (double curX = -(windowWidth / 2); curX < (windowWidth / 2); curX++) {
                    MyColor newColor = rawData[uint32_t(curRenderY + (windowHeight / 2))][uint32_t(curX + (windowWidth / 2))];
                    MyColor gammaColor = {sqrt(newColor.fRed_),
                                          sqrt(newColor.fGreen_),
                                          sqrt(newColor.fBlue_)};


                    curImage.SetPixel(uint32_t(curX + (windowWidth / 2)), uint32_t(curRenderY + (windowHeight / 2)), gammaColor);
                }
            }

            window.DrawImage(curImage, {0, 0});
            window.Display();
        }
    }
}

void ProcessRow(MyColor* curRow, const double row, const Vector3D& cameraCoords,
                void* objectsVoid, const uint32_t objectsAmount) {
    const BaseObject** objects = const_cast<const BaseObject**>(reinterpret_cast<BaseObject**>(objectsVoid));
    for (double curX = -(windowWidth / 2); curX < (windowWidth / 2); curX++) {
        Vector3D sumColor = {0, 0, 0};
        for (uint32_t curSample = 0; curSample < uint32_t(SuperSamplingCoef); curSample++) {
            const double convertedY = (row  + GetRandomDouble()) * (virtualHeight  / windowHeight);
            const double convertedX = (curX + GetRandomDouble()) * (virtualWidth / windowWidth);

            const Vector3D virtualPoint = {convertedX, convertedY, displayDistance};

            Ray curRay = {cameraCoords, virtualPoint - cameraCoords, 1.2};
            curRay.vector_.Normalise();

            sumColor += Vector3D(ProcessRay(objects, objectsAmount, curRay, cameraCoords, 0));
        } 
        sumColor *= (1.0 / SuperSamplingCoef);
        
        curRow[uint32_t(curX + (windowWidth / 2))] = MyColor(sumColor.x_, sumColor.y_, sumColor.z_);
    }
}

MyColor ProcessRay(const BaseObject** objects, const uint32_t objectsAmount,
                   const Ray& curRay, const Vector3D& camCoords,
                   const uint32_t curDepth) {
    if (curDepth == MaxRecursionDepth)
        return 0;               

    MyColor colorAbsorbed = 0;

    uint32_t leastObject = UINT32_MAX;
    Ray processedRay = FindClosestObject(objects, objectsAmount, curRay, &colorAbsorbed, &leastObject);
    
    if (leastObject == UINT32_MAX) {
        double convertedY = 0.5 * (Normalise(curRay.vector_).y_ + 1.0);

        // return 0;
        return MyColor(1.0, 1.0, 1.0) * (1.0 - convertedY) + MyColor(0.5, 0.7, 1.0) * convertedY;
    }

    MyColor emitted = objects[leastObject]->objectMaterial_->GetEmitted(processedRay.point_);
    if (processedRay.vector_.IsNan()) {
        return emitted;
    }

    processedRay.vector_.Normalise();
    processedRay.point_ += processedRay.vector_ * LittleEpsilon;
   
    MyColor newColor = ProcessRay(objects, objectsAmount, processedRay, camCoords, curDepth + 1);

    return emitted + colorAbsorbed * newColor;
}

Ray FindClosestObject(const BaseObject** objects, const uint32_t objectsAmount, const Ray& curRay, MyColor* colorAbsorbed, uint32_t* objectNumber) {
    Ray leastRay = {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};

    double leastDistance   = __DBL_MAX__;
    uint32_t leastObject   = UINT32_MAX;
    MyColor leastColor     = 0;

    for (uint32_t curObject = 0; curObject < objectsAmount; curObject++) {
        Ray newRay = objects[curObject]->Trace(curRay, colorAbsorbed);
        Vector3D curDistance = curRay.point_ - newRay.point_;

        if (!newRay.IsNan() && (leastRay.IsNan() || (curDistance.LengthSquared() < leastDistance))) {
            leastRay = newRay;

            leastColor = *colorAbsorbed;

            leastDistance = curDistance.LengthSquared();
            leastObject   = curObject;
        }
    }

    if (objectNumber)
        *objectNumber = leastObject;

    *colorAbsorbed = leastColor;
    return leastRay;
}

bool PollEvent(Window& window) {
    Event curEvent = {};

    while (window.PollEvent(&curEvent))
    {
        switch (curEvent.type_)
        {
        case Event::EventType::close:
            window.~Window();
            return 1;
            break;
        case Event::EventType::mouseButtonPressed:
            break;
        case Event::EventType::nothing:
            break;
        default:
            break;
        }

        curEvent.type_ = Event::EventType::nothing;
    }

    return 0;
}

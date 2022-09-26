#include "Main.hpp"

int main() {
    LightSource simpleLight;
    simpleLight.lightColor_ = {0xffffff00};
    simpleLight.intensivity_ = 3;

    Material glass = {};
    glass.isTransparent_ = 1;
    glass.materialRefractionCoef_ = 1.5;
    glass.reflectibility_ = 3;
    glass.reflectPowerDecrease_ = 0.15;
    glass.refractPowerDecrease_ = 0.9;

    Material mirror = {};
    mirror.isTransparent_ = 0;
    mirror.materialRefractionCoef_ = 0;
    mirror.reflectibility_ = 3;
    mirror.reflectPowerDecrease_ = 0.5;
    mirror.refractPowerDecrease_ = 0.001;

    Material rubber = {};
    rubber.isTransparent_ = 0;
    rubber.materialRefractionCoef_ = 0;
    rubber.reflectibility_ = 5;
    rubber.reflectPowerDecrease_ = 0.1;
    rubber.refractPowerDecrease_ = 0;

    Window window(windowWidth, windowHeight);
    
    Vector3D cameraCoords = {0, 0, 0};

    Image curImage = {};
    curImage.Create(windowWidth, windowHeight, {0x00, 0x00, 0x00});

    // 0x10901000
    Sphere sph1({5, 4, 35}, 2, 0x00000000, glass, simpleLight);
    Sphere sph2({-10, 7, 50}, 5, 0xffd70000, mirror, simpleLight);
    Sphere sph3({10, 7, 50}, 5, 0x9400d300, mirror, simpleLight);

    Sphere light1({0, 5, 100}, 3, 0xffffff00, mirror, simpleLight, 1);
    Sphere light2({20, 0, 30}, 3, 0xffffff00, mirror, simpleLight, 1);
    Sphere light3({-20, 0, 30}, 3, 0xffffff00, mirror, simpleLight, 1);

    Matrix plane1(1, 4);
    plane1.SetElem(0, 0, 0);
    plane1.SetElem(0, 1, -1);
    plane1.SetElem(0, 2, 0);
    plane1.SetElem(0, 3, 15);

    Matrix limits(4, 5);
    limits.SetElem(0, 0, 1);
    limits.SetElem(0, 1, 0);
    limits.SetElem(0, 2, 0);
    limits.SetElem(0, 3, -25);
    limits.SetElem(0, 4, 0);

    limits.SetElem(1, 0, 1);
    limits.SetElem(1, 1, 0);
    limits.SetElem(1, 2, 0);
    limits.SetElem(1, 3, 25);
    limits.SetElem(1, 4, 1);

    limits.SetElem(2, 0, 0);
    limits.SetElem(2, 1, 0);
    limits.SetElem(2, 2, 1);
    limits.SetElem(2, 3, -160);
    limits.SetElem(2, 4, 0);

    limits.SetElem(3, 0, 0);
    limits.SetElem(3, 1, 0);
    limits.SetElem(3, 2, 1);
    limits.SetElem(3, 3, -30);
    limits.SetElem(3, 4, 1);

    Plane pln1(plane1, limits, 0x50c878, rubber, simpleLight);

    const uint32_t objectsAmount = 7;
    BaseObject* objects[objectsAmount]  = {&pln1, &sph1, &sph2, &sph3, &light1, &light2, &light3};

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
        }

        for (double curY = -(windowHeight / 2); curY < (windowHeight / 2); curY++) {
            for (double curX = -(windowWidth / 2); curX < (windowWidth / 2); curX++) {
                curImage.SetPixel(uint32_t(curX + (windowWidth / 2)), uint32_t(curY + (windowHeight / 2)), rawData[uint32_t(curY + (windowHeight / 2))][uint32_t(curX + (windowWidth / 2))]);
            }
        }

        window.DrawImage(curImage, {0, 0});
        window.Display();
    }

    std::cout << "GOING OUT " << std::endl;
}

void ProcessRow(MyColor* curRow, const double row, const Vector3D& cameraCoords,
                void* objectsVoid, const uint32_t objectsAmount) {
    const BaseObject** objects = const_cast<const BaseObject**>(reinterpret_cast<BaseObject**>(objectsVoid));
    const double convertedY = row * (virtualHeight  / windowHeight);

    for (double curX = -(windowWidth / 2); curX < (windowWidth / 2); curX++) {
        const double convertedX = curX * (virtualWidth / windowWidth);

        const Vector3D virtualPoint = {convertedX, convertedY, displayDistance};

        Ray curRay = {cameraCoords, virtualPoint - cameraCoords, 1.2};
        curRay.vector_.Normalise();

        MyColor newColor = ProcessRay(objects, objectsAmount, curRay, cameraCoords, 0);

        curRow[uint32_t(curX + (windowWidth / 2))] = newColor;
    }
}

MyColor ProcessRay(const BaseObject** objects, const uint32_t objectsAmount,
                   const Ray& curRay, const Vector3D& camCoords,
                   const uint32_t curDepth) {
    uint32_t leastObject = UINT32_MAX;

    Ray reflectedRay = FindClosestObject(objects, objectsAmount, curRay, &leastObject);

    if (reflectedRay.IsNan()) {
        return 0x00000000;
    }

    if (objects[leastObject]->isLight_) {
        if (curDepth == 0) {
            return 0xffffff00;
        }

        return 0;
    }

    reflectedRay.vector_.Normalise();

    MyColor newColor = 0;
    newColor += CalcColor(objects, objectsAmount, reflectedRay, leastObject, camCoords);   
    
    if ((curDepth + 1) < MaxRecursionDepth) {
        if (objects[leastObject]->isTransparent_) {
            Ray refractedRay = objects[leastObject]->GetRefracted({reflectedRay.point_, curRay.vector_});

            if (!refractedRay.IsNan()) {
                refractedRay.vector_.Normalise();
                refractedRay.point_ += refractedRay.vector_ * LittleEpsilon;

                Vector3D saveVector = refractedRay.vector_;
                double savePower = refractedRay.power_;

                refractedRay = FindClosestObject(objects, objectsAmount, refractedRay, &leastObject);

                refractedRay.power_ = savePower;
                refractedRay.vector_ = saveVector;

                refractedRay = objects[leastObject]->GetRefracted(refractedRay);
                refractedRay.point_ += refractedRay.vector_ * LittleEpsilon;
                
                if (!refractedRay.IsNan()) {
                    MyColor colorDiff = ProcessRay(objects, objectsAmount, refractedRay, camCoords, curDepth + 1);

                    newColor += colorDiff;
                }
            }
        }

        reflectedRay.point_ += reflectedRay.vector_ * LittleEpsilon;
        newColor += ProcessRay(objects, objectsAmount, reflectedRay, camCoords, curDepth + 1);
    }

    return newColor;
}

Ray FindClosestObject(const BaseObject** objects, const uint32_t objectsAmount, const Ray& curRay, uint32_t* objectNumber) {
    Ray leastRay = {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};

    double leastDistance   = __DBL_MAX__;
    uint32_t leastObject   = UINT32_MAX;

    for (uint32_t curObject = 0; curObject < objectsAmount; curObject++) {
        Ray newRay = objects[curObject]->Trace(curRay);
        Vector3D curDistance = curRay.point_ - newRay.point_;

        if (!newRay.IsNan() && (leastRay.IsNan() || (curDistance.LengthSquared() < leastDistance))) {
            leastRay = newRay;

            leastDistance = curDistance.LengthSquared();
            leastObject   = curObject;
        }
    }

    if (objectNumber)
        *objectNumber = leastObject;

    return leastRay;
}

MyColor CalcColor(const BaseObject** objects, const uint32_t objectsAmount,
                  const Ray& leastRay, const uint32_t leastObject,
                  const Vector3D& camCoords) {
    MyColor newColor = 0;

    Vector3D normal = objects[leastObject]->GetNormal(leastRay);

    for (uint32_t curObject = 0; curObject < objectsAmount; curObject++) {
        if (!objects[curObject]->isLight_) {
            continue;
        }

        Ray lightRay = {leastRay.point_, -objects[curObject]->GetOutsideNormal(leastRay)};
        lightRay.vector_.Normalise();
        lightRay.point_ += lightRay.vector_ * LittleEpsilon; 

        uint32_t leastShadowObject = UINT32_MAX;
        Ray shadowRay = FindClosestObject(objects, objectsAmount, lightRay, &leastShadowObject);

        if (!shadowRay.IsNan() && (leastShadowObject == curObject)) {
            newColor += (objects[leastShadowObject]->lightColor_ * objects[leastObject]->color_ * CalcLambert(normal, lightRay) +
                         objects[leastShadowObject]->lightColor_ * CalcDiffuse(objects[leastObject], leastRay, lightRay, camCoords)) *
                        (leastRay.power_ * objects[leastShadowObject]->intensivity_ / (objects[leastObject]->reflectPowerDecrease_ * sqrt(objects[leastShadowObject]->GetDistance(leastRay.point_))));
        }
    }

    return newColor;
}

double CalcDiffuse(const BaseObject* object, const Ray& ray, const Ray& lightRay, const Vector3D& camCoords) {
    Ray reflectedLightRay = {ray.point_, object->GetReflected(ray.point_, {lightRay.point_, -lightRay.vector_})};

    double cosBeta = reflectedLightRay.vector_.CosBetween(camCoords - ray.point_);
    if (cosBeta < 0) 
        cosBeta = 0;

    return pow(cosBeta, object->reflectibility_);
}

double CalcLambert(const Vector3D& normal, const Ray& lightRay) {
    double cosAlpha = normal.CosBetween(lightRay.vector_);
    if (cosAlpha < 0)
        cosAlpha = 0;
        
    return cosAlpha;
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

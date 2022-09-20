#include "Main.hpp"

int main()
{
    Window window(windowWidth, windowHeight);
    
    Vector3D cameraCoords = {0, 0, 0};

    Image curImage = {};
    curImage.Create(windowWidth, windowHeight, {0x00, 0x00, 0x00});

    Sphere sph1({10, 7, 40}, 5, 0x10901000, 3, 0.5);
    Sphere sph2({-10, 7, 40}, 5, 0xffd70000, 3, 0.5);
    Sphere sph3({0, 7, 50}, 5, 0x9400d300, 3, 0.5);

    Matrix plane1(1, 4);
    plane1.SetElem(0, 0, 0);
    plane1.SetElem(0, 1, 1);
    plane1.SetElem(0, 2, 0);
    plane1.SetElem(0, 3, -15);

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

    // x <= 25 && x >= -25 && z < 160 && z > 30 && y = 15

    Plane pln1(plane1, limits, 0x50c878, 1, 0.2);

    const uint32_t objectsAmount = 4;
    BaseObject* objects[objectsAmount]  = {&pln1, &sph1, &sph2, &sph3};

    LightSource light1 = {{25, -1, 30}, {0xab274f00}, 0.5};
    LightSource light2 = {{-25, -1, 30}, {0xab274f00}, 0.5};
    LightSource light3 = {{0, -5, 25}, {0xab274f00}, 1};
    LightSource light4 = {{25, -1, 160}, {0xab274f00}, 0.2};
    LightSource light5 = {{-25, -1, 160}, {0xab274f00}, 0.2};

    const uint32_t lightsAmount = 5;
    LightSource* lights[lightsAmount] = {&light1, &light2, &light3, &light4, &light5};

    while(window.IsOpen()) {
        window.Clear();
        PollEvent(window);

        for (double curX = -(windowWidth / 2); curX < (windowWidth / 2); curX++) {
            for (double curY = -(windowHeight / 2); curY < (windowHeight / 2); curY++) {
                const double convertedX = curX * (virtualWidth  / windowWidth);
                const double convertedY = curY * (virtualHeight / windowHeight);

                const Vector3D virtualPoint = {convertedX, convertedY, displayDistance};

                Ray curRay = {cameraCoords, virtualPoint - cameraCoords, 1.2};
                curRay.vector_.Normalise();

                uint32_t leastObject = UINT32_MAX;
                Ray leastRay = FindClosestObject(objects, objectsAmount, curRay, &leastObject);
                
                bool isAny = 0;

                MyColor newColor = ambient;
                for (uint32_t curReflection = 0; (curReflection < maxReflections) && !leastRay.IsNan(); curReflection++) {
                    leastRay.vector_.Normalise();
                    isAny = 1;

                    newColor += CalcColor(lights, lightsAmount, objects, objectsAmount, leastRay, leastObject, cameraCoords);   
                
                    leastRay = FindClosestObject(objects, objectsAmount, leastRay, &leastObject);
                }
                
                if (isAny)
                    curImage.SetPixel(uint32_t(curX + (windowWidth / 2)), uint32_t(curY + (windowHeight / 2)), newColor);
                else
                    curImage.SetPixel(uint32_t(curX + (windowWidth / 2)), uint32_t(curY + (windowHeight / 2)), {0x7d, 0x7f, 0x7d});
            }
        }

        window.DrawImage(curImage, {0, 0});
        window.Display();
    }
}

Ray FindClosestObject(BaseObject** objects, const uint32_t objectsAmount, const Ray& curRay, uint32_t* objectNumber) {
    Ray leastRay = {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};

    double leastDistance   = __DBL_MAX__;
    uint32_t leastObject   = UINT32_MAX;

    for (uint32_t curObject = 0; curObject < objectsAmount; curObject++) {
        if ((objectNumber) && (curObject == *objectNumber))
            continue;

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

MyColor CalcColor(LightSource** lights, const uint32_t lightsAmount, 
                  BaseObject** objects, const uint32_t objectsAmount,
                  const Ray& leastRay, const uint32_t leastObject,
                  const Vector3D& camCoords) {
    MyColor newColor = 0;

    for (uint32_t curLight = 0; curLight < lightsAmount; curLight++) {
        Ray lightRay = {lights[curLight]->point_, leastRay.point_ - lights[curLight]->point_};
        lightRay.vector_.Normalise();

        uint32_t leastShadowObject = UINT32_MAX;
        FindClosestObject(objects, objectsAmount, lightRay, &leastShadowObject);
        
        if (leastShadowObject == leastObject) {
            newColor += (lights[curLight]->color_ * objects[leastObject]->color_ * CalcLambert(objects[leastObject], leastRay, lightRay) +
                         lights[curLight]->color_ * CalcDiffuse(objects[leastObject], leastRay, lightRay, camCoords)) *
                         (leastRay.power_ * lights[curLight]->intensivity_ / objects[leastObject]->powerDecrease_);
        }
    }
    
    return newColor;
}

double CalcDiffuse(const BaseObject* object, const Ray& ray, const Ray& lightRay, const Vector3D& camCoords) {
    Ray reflectedLightRay = {ray.point_, object->GetReflected(ray.point_, lightRay)};

    double cosBeta = reflectedLightRay.vector_.CosBetween(camCoords - ray.point_);
    if (cosBeta < 0) 
        cosBeta = 0;

    return pow(cosBeta, object->reflectibility_);
}

double CalcLambert(const BaseObject* object, const Ray& ray, const Ray& lightRay) {
    Vector3D normal = object->GetNormal(ray);

    double cosAlpha = normal.CosBetween(-lightRay.vector_);
    if (cosAlpha < 0)
        cosAlpha = 0;

    return cosAlpha;
}

void PollEvent(Window& window) {
    Event curEvent = {};

    while (window.PollEvent(&curEvent))
    {
        switch (curEvent.type_)
        {
        case Event::EventType::close:
            window.~Window();
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
}

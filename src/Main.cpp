#include "Main.hpp"

int main()
{
    Window window(windowWidth, windowHeight);
    
    Vector3D cameraCoords = {0, 0, 0};

    Image curImage = {};
    curImage.Create(600, 600, {0x00, 0x00, 0x00});

    Sphere sph1({0, 0, 20}, 5, 0x10901000, 3, 0.8);

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
    limits.SetElem(2, 3, -100);
    limits.SetElem(2, 4, 0);

    limits.SetElem(3, 0, 0);
    limits.SetElem(3, 1, 0);
    limits.SetElem(3, 2, 1);
    limits.SetElem(3, 3, -30);
    limits.SetElem(3, 4, 1);

    Plane pln1(plane1, limits, 0x50c878, 1, 0.8);

    BaseObject* objects[]  = {&pln1, &sph1};
    uint32_t objectsAmount = 2;

    LightSource light1 = {{-20, 0, 0}, {0xab274f00}, 1};
    LightSource light2 = {{12.5, -1, 30.5}, {0xab274f00}, 1};
    
    LightSource* lights[] = {&light2, &light1};
    uint32_t lightsAmount = 1;

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

                if (!leastRay.IsNan()) {
                    MyColor newColor = 0;
                    for (uint32_t curLight = 0; curLight < lightsAmount; curLight++) {
                        newColor += CalcColor(leastRay, objects[leastObject], cameraCoords, lights[curLight]);
                    }

                    curImage.SetPixel(uint32_t(curX + (windowWidth / 2)), uint32_t(curY + (windowHeight / 2)), newColor);
                }
                else {
                    curImage.SetPixel(uint32_t(curX + (windowWidth / 2)), uint32_t(curY + (windowHeight / 2)), {0x7d, 0x7f, 0x7d});
                }
            }
        }

        window.DrawImage(curImage, {0, 0});
        window.Display();
    }
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

MyColor CalcColor(const Ray& ray, const BaseObject* object, const Vector3D camCoords, const LightSource* light) {
    Vector3D normal = object->GetNormal(ray);
    
    Ray lightRay = {light->point_, ray.point_ - light->point_};
    lightRay.vector_.Normalise();

    double cosAlpha = normal.CosBetween(-lightRay.vector_);
    if (cosAlpha < 0)
        cosAlpha = 0;

    Ray reflectedLightRay = {ray.point_, object->GetReflected(ray.point_, lightRay)};

    double cosBeta = reflectedLightRay.vector_.CosBetween(camCoords - ray.point_);
    if (cosBeta < 0) 
        cosBeta = 0;

    double specular = pow(cosBeta, object->reflectibility_);

    MyColor newColor =  object->color_ * light->color_ * cosAlpha + 
                        light->color_  * specular +
                        object->color_ * light->color_ * ambient;

    newColor *= ray.power_;

    return newColor;
}

#include "Main.hpp"

int main()
{
    // Matrix matrix(4, 4);

    // matrix.SetElem(0, 0, 0);
    // matrix.SetElem(0, 1, 0);
    // matrix.SetElem(0, 2, 1);
    // matrix.SetElem(0, 3, 0);

    // matrix.SetElem(1, 0, -1);
    // matrix.SetElem(1, 1, 1);
    // matrix.SetElem(1, 2, 1);
    // matrix.SetElem(1, 3, -1);

    // matrix.SetElem(2, 0, 0);
    // matrix.SetElem(2, 1, 1);
    // matrix.SetElem(2, 2, 0);
    // matrix.SetElem(2, 3, 0);

    // matrix.SetElem(3, 0, 1);
    // matrix.SetElem(3, 1, 1);
    // matrix.SetElem(3, 2, 1);
    // matrix.SetElem(3, 3, -1);
    
    // // 0 0 1 0
    // // -1 1 1 -1
    // // 0 1 0 0
    // // 1 1 1 -1


    // // (0, 0.19, 0), (0, 0.68, -1)

    // Vector3D startPoint = {0, 0.68, -1};
    // Vector3D vector     = {0, 0.19 - 0.68, 1};

    // Vector3D answer = GetIntersection(startPoint, vector, matrix);

    Window newWindow(630, 420);

    Button sphereButton = {{70, 100}, {570, 200}, {0xFF, 0xFF, 0xFF}, "Sphere", {0x00, 0x00, 0x00}};
    Button arrowsButton = {{70, 210}, {570, 310}, {0x00, 0x00, 0xFF}, "Arrows", {0x00, 0x00, 0x00}};

    Event curEvent = {};
    while (newWindow.IsOpen())
    {
        newWindow.Clear();

        while (newWindow.PollEvent(&curEvent))
        {
            switch (curEvent.type_)
            {
            case Event::EventType::close:
                newWindow.~Window();
                break;
            case Event::EventType::mouseButtonPressed:
                if (sphereButton.IsPressed(curEvent.mouseCoords_))
                {
                    newWindow.~Window();
                    SphereGame();
                }

                if (arrowsButton.IsPressed(curEvent.mouseCoords_))
                {
                    ArrowsGame(&newWindow);
                }
                break;
            case Event::EventType::nothing:
                break;
            default:
                break;
            }
        }

        newWindow.DrawButton(sphereButton);
        newWindow.DrawButton(arrowsButton);

        newWindow.Display();
    }
}

void SphereGame()
{
    Window newWindow(700, 700);

    const float specularN = float(10);
    MyColor ambient(float(0.01), float(0.01), float(0.01), float(0.1));

    const int32_t sphereRadius = 300;
    Vector3D sphereCrds = {350, 350, 0};

    Vector3D cameraCrds = {350, 350, 900};
    

    MyColor lightColor = {0xab, 0x34, 0x3a};
    MyColor materialColor = {0xff, 0x90, 0xff};

    float t = 0;
    while (newWindow.IsOpen())
    {
        Vector3D lampCrds = {350 + 350 * cosf(t), 350 + 350 * sinf(t), 900};
        newWindow.Clear();

        Image curImage = {};
        curImage.Create(700, 700, {0x00, 0x00, 0x00});

        Event curEvent = {};
        while (newWindow.PollEvent(&curEvent))
        {
            switch (curEvent.type_)
            {
            case Event::EventType::close:
                newWindow.~Window();
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

        for (int32_t curX = 0; curX < 700; curX++)
        {
            for (int32_t curY = 0; curY < 700; curY++)
            {
                if (((350 - curX) * (350 - curX) + (350 - curY) * (350 - curY)) >= (sphereRadius * sphereRadius))
                {
                    curImage.SetPixel(curX, curY, {0xFF, 0xFF, 0xFF});
                }
                else
                {
                    float curZ = sqrtf(float(sphereRadius * sphereRadius - (350 - curX) * (350 - curX) - (350 - curY) * (350 - curY)));
                    Vector3D curPoint = {float(curX), float(curY), curZ};

                    Vector3D normalVector = curPoint - sphereCrds;
                    normalVector.Normalise();

                    Vector3D lightVector  = lampCrds - curPoint;
                    lightVector.Normalise();

                    Vector3D camVector    = cameraCrds - curPoint;
                    camVector.Normalise();

                    float cosAlpha = normalVector.CosBetween(lightVector);

                    normalVector *= fsqrt(2 * cosAlpha);
                    if (cosAlpha < 0)
                        cosAlpha = 0;

                    Vector3D reflectedLightVector = normalVector - lightVector;
                    reflectedLightVector.Normalise();

                    float cosBeta = reflectedLightVector.CosBetween(camVector);
                    if (cosBeta < 0)
                        cosBeta = 0;

                    float specular = powf(cosBeta, specularN);
                    
                    MyColor newColor = lightColor * materialColor * cosAlpha +
                                       lightColor * specular +
                                       lightColor * materialColor * ambient;

                    curImage.SetPixel(curX, curY, newColor);
                }
            }
        }

        newWindow.DrawImage(curImage, {0, 0});
        newWindow.Display();

        t += float(0.1);
    }
}

void ArrowsGame(Window *newWindow)
{
    CoordSystem s1 = {{5, 5}, {405, 405}, {10, 10}, {30, 30}};
    CoordSystem s2 = {{420, 5}, {620, 205}, {10, 10}, {30, 30}};

    Button resetButton = {{420, 220}, {620, 270}, {0xFF, 0xFF, 0xFF}, "Reset", {0x8b, 0x00, 0xFF}};
    Button closeButton = {{420, 280}, {620, 330}, {0xFF, 0x00, 0x00}, "Close", {0x00, 0x00, 0x00}};

    Vector firstArrow = {0, 10};
    Vector secondArrow = {0, -10};

    Vector firstRotator = {0, 9};
    Vector secondRotator = {0, -9};

    Vector middleCoords = {20, 20};

    Event curEvent = {};
    while (newWindow->IsOpen())
    {
        newWindow->Clear();

        while (newWindow->PollEvent(&curEvent))
        {
            switch (curEvent.type_)
            {
            case Event::EventType::close:
                newWindow->~Window();
                break;
            case Event::EventType::mouseButtonPressed:
            {
                if (resetButton.IsPressed(curEvent.mouseCoords_))
                {
                    firstArrow = {0, 10};
                    secondArrow = {0, -10};

                    firstRotator = {0, 9};
                    secondRotator = {0, -9};
                }

                if (s1.IsPressed(curEvent.mouseCoords_))
                {
                    Vector mathCoords = s1.ConvertVectorToMath(curEvent.mouseCoords_);

                    if (GetDistanceSquared(middleCoords, middleCoords + firstArrow, mathCoords) <
                        GetDistanceSquared(middleCoords, middleCoords + secondArrow, mathCoords))
                    {
                        firstArrow = mathCoords;
                        firstArrow -= middleCoords;

                        firstArrow.Resize(ArrowLength);
                    }
                    else
                    {
                        secondArrow = mathCoords;
                        secondArrow -= middleCoords;

                        secondArrow.Resize(ArrowLength);
                    }
                }

                if (closeButton.IsPressed(curEvent.mouseCoords_))
                {
                    newWindow->~Window();
                }

                break;
            }
            case Event::EventType::nothing:
                break;
            default:
                break;
            }

            curEvent.type_ = Event::EventType::nothing;
        }

        firstRotator.Rotate(RotateCoeft);
        secondRotator.Rotate(RotateCoeft);

        newWindow->DrawCoordSystem(s1);
        newWindow->DrawCoordSystem(s2);

        newWindow->DrawVector(s1, firstArrow, middleCoords);
        newWindow->DrawVector(s1, secondArrow, middleCoords);

        newWindow->DrawVector(s2, firstRotator, middleCoords);
        newWindow->DrawVector(s2, secondRotator, middleCoords);

        newWindow->DrawButton(resetButton);
        newWindow->DrawButton(closeButton);

        newWindow->Display();
    }
}

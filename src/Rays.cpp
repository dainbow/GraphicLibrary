#include "Rays.hpp"

bool VocabCmp(const Vector& vec1, const Vector& vec2) {
    return (vec1.x_ < vec2.x_) || (CmpFloat(vec1.x_, vec2.x_) && (vec1.y_ < vec2.y_));
}

bool TgCmp(const Vector& vec1, const Vector& vec2) {
    float tg1 = (!CmpFloat(vec1.x_, 0)) ? vec1.y_ / vec1.x_ : INFINITY; 
    float tg2 = (!CmpFloat(vec2.x_, 0)) ? vec2.y_ / vec2.x_ : INFINITY;

    return tg1 <= tg2;
}

bool CheckPointInPolygon(const Vector& intersectionPoint, Vector* points, const uint32_t pointsAmount) {
    std::sort(points, points + pointsAmount, VocabCmp);

    Vector* zeroPoint = points;
    if (intersectionPoint.x_ < zeroPoint->x_)
        return 0;

    if (intersectionPoint == *zeroPoint)
        return 1;


    for (uint32_t curPoint = 1; curPoint < pointsAmount; curPoint++) {
        points[curPoint] -= *zeroPoint;
    }

    std::sort(points + 1, points + pointsAmount, TgCmp);

    Vector curTg = intersectionPoint -  *zeroPoint;

    Vector* firstPoint = nullptr;
    Vector* secondPoint = nullptr;

    // TODO Binsearch instead of linear
    for (uint32_t curPoint = 1; curPoint < (pointsAmount - 1); curPoint++) {
        if (TgCmp(points[curPoint], curTg) && TgCmp(curTg, points[curPoint + 1])) {
            firstPoint  = points + curPoint;
            secondPoint = points + curPoint + 1;

            break;
        }
    }
    if (firstPoint == nullptr) {
        return 0;
    }

    *firstPoint  += *zeroPoint;
    *secondPoint += *zeroPoint;

    if (OrientSquare(*firstPoint, *secondPoint, intersectionPoint) <= 0)
        return 1;

    return 0;
}

bool CheckPointInPolygon3D(const Vector3D& intersectionPoint, const Vector3D* points, const uint32_t pointAmount) {
    // y0 = kx0 + b
    // y1 = kx1 + b

    bool flagZ = 0;

    if (!CmpFloat(points[0].x_, points[1].x_)) {
        float kZ = (points[0].y_ - points[1].y_) / (points[0].x_ - points[1].x_);
        float bZ = points[0].y_ - kZ * points[0].x_;

        flagZ = CmpFloat(points[2].y_, kZ * points[2].x_ + bZ);
    }
    else {
        flagZ = CmpFloat(points[2].x_, points[0].x_);
    }

    Vector* points2D = new Vector[pointAmount]();
    Vector intersectionPoint2D = {};

    if (flagZ) {
        bool flagX = 0;
        
        if (CmpFloat(points[0].y_, points[1].y_)) {
            float kX = (points[0].y_ - points[1].y_) / (points[0].z_ - points[1].z_);
            float bX = points[0].y_ - kX * points[0].z_;

            flagX = CmpFloat(points[2].y_, kX * points[2].z_ + bX);
        }
        else {
            flagX = CmpFloat(points[2].y_, points[0].y_);
        }

        if (flagX) {
            for (uint32_t curPoint = 0; curPoint < pointAmount; curPoint++) {
                points2D[curPoint] = {points[curPoint].y_, points[curPoint].z_};
            }

            intersectionPoint2D = {intersectionPoint.y_, intersectionPoint.z_};
        }
        else {
            for (uint32_t curPoint = 0; curPoint < pointAmount; curPoint++) {
                points2D[curPoint] = {points[curPoint].x_, points[curPoint].z_};
            }

            intersectionPoint2D = {intersectionPoint.x_, intersectionPoint.z_};
        }
    }
    else {
        for (uint32_t curPoint = 0; curPoint < pointAmount; curPoint++) {
            points2D[curPoint] = {points[curPoint].x_, points[curPoint].y_};
        }

        intersectionPoint2D = {intersectionPoint.x_, intersectionPoint.y_};
    }

    bool result = CheckPointInPolygon(intersectionPoint2D, points2D, pointAmount);

    delete[] points2D;

    return result;
}

Vector3D GetIntersectionSection(const Vector3D& intersectionPoint, const Matrix& planeCoefs) {
    Vector3D* points  = new Vector3D[planeCoefs.GetRows() - 1]();
    uint32_t curPoint = 0;

    Matrix mainSystem(3, 3);
    Matrix extensionColumn(3, 1);

    for (uint32_t curColumn = 0; curColumn < 3; curColumn++) {
        mainSystem.SetElem(0, curColumn, *planeCoefs.GetElem(0, curColumn));
    }
    extensionColumn.SetElem(0, 0, *planeCoefs.GetElem(0, 3));

    for (uint32_t curPlane = 1; curPlane < planeCoefs.GetRows(); curPlane++) {
        // A1x + B1y + C1z + D1 = 0
        // A2x + B2y + C2z + D2 = 0
        // A3x + B3y + C3z + D3 = 0

        for (uint32_t curColumn = 0; curColumn < 3; curColumn++) {
            mainSystem.SetElem(1, curColumn, *planeCoefs.GetElem(curPlane, curColumn));
        }
        extensionColumn.SetElem(1, 0, *planeCoefs.GetElem(curPlane, 3));


        for (uint32_t curSecondPlane = curPlane + 1; curSecondPlane < planeCoefs.GetRows(); curSecondPlane++) {
            for (uint32_t curColumn = 0; curColumn < 3; curColumn++) {
                mainSystem.SetElem(2, curColumn, *planeCoefs.GetElem(curSecondPlane, curColumn));
            }
            extensionColumn.SetElem(2, 0, *planeCoefs.GetElem(curSecondPlane, 3));

            float delta = mainSystem.CalculateDeterminant();
            if (CmpFloat(delta, 0))
                continue;

            float deltas[3] = {};
            for (uint32_t curColumn = 0; curColumn < 3; curColumn++) {
                mainSystem.SwapColumns(curColumn, 0, extensionColumn);
                deltas[curColumn] = mainSystem.CalculateDeterminant(); 
                mainSystem.SwapColumns(curColumn, 0, extensionColumn);
            }

            points[curPoint++] = {deltas[0] / delta, deltas[1] / delta, deltas[2] / delta};
            std::cout << points[curPoint - 1] << std::endl;
        }
    }

    assert((curPoint > 2) && "CAN'T FIND POLYGON");
    bool result = CheckPointInPolygon3D(intersectionPoint, points, curPoint);
    std::cout << "RESULT IS " << result << std::endl;    

    delete[] points;
    if (result)
        return intersectionPoint;

    return {NAN, NAN, NAN};
}

Vector3D GetIntersection(const Vector3D& rayPoint, const Vector3D& rayVector, const Matrix& planeCoefs) {
    if (planeCoefs.GetColumns() != 4)
        return {NAN, NAN, NAN};

    if (planeCoefs.GetRows() == 0) {
        return {NAN, NAN, NAN};
    }

    if (planeCoefs.GetRows() == 2) {
        return {NAN, NAN, NAN};
    }

    float intersectionTime = - (*planeCoefs.GetElem(0, 0) * rayPoint.x_ + *planeCoefs.GetElem(0, 1) * rayPoint.y_  + *planeCoefs.GetElem(0, 2) * rayPoint.z_ + *planeCoefs.GetElem(0, 3)) 
                            /  (*planeCoefs.GetElem(0, 0) * rayVector.x_+ *planeCoefs.GetElem(0, 1) * rayVector.y_ + *planeCoefs.GetElem(0, 2) * rayVector.z_);
    assert(std::isfinite(intersectionTime));

    Vector3D intersectionPoint = {rayVector.x_ * intersectionTime + rayPoint.x_,
                                  rayVector.y_ * intersectionTime + rayPoint.y_,
                                  rayVector.z_ * intersectionTime + rayPoint.z_};

    if (planeCoefs.GetRows() > 2) {
        return GetIntersectionSection(intersectionPoint, planeCoefs);
    }

    return {NAN, NAN, NAN};
}

Vector3D GetIntersectionSection(const Vector3D& rayPoint, const Vector3D& rayVector, 
                                       const Vector3D& sphereCenter, const float radius) {
    // (x - x0)^2 + (y - y0)^2 + (z - z0)^2 = R^2
    // x = lt + x1
    // y = mt + y1
    // z = nt + z1
    // (lt + x1 - x0)^2 + (mt + y1 - y0)^2 + (nt + z1 - z0)^2 = R^2
    // l^2t^2 + x1^2 + x0^2 + 2lt(x1) - 2lt(x0) - 2(x1)(x0) +
    // m^2t^2 + y1^2 + y0^2 + 2mt(y1) - 2mt(y0) - 2(y1)(y0) +
    // n^2t^2 + z1^2 + z0^2 + 2nt(z1) - 2nt(z0) - 2(z1)(z0) = R^2
    // (l^2 + m^2 + n^2)t^2 + (2l(x1 - x0) + 2m(y1 - y0) + 2n(z1 - z0))t + x1^2

    float c = powf(rayPoint.x_, 2) + powf(rayPoint.y_, 2) + powf(rayPoint.z_, 2) +
              powf(sphereCenter.x_, 2) + powf(sphereCenter.y_, 2) + powf(sphereCenter.z_, 2) - powf(radius, 2);

    float a = powf(rayVector.x_, 2) + powf(rayVector.y_, 2) + powf(rayVector.z_, 2);

    Vector3D deltaVector = rayPoint - sphereCenter;
    float b = 2 * (deltaVector * rayVector);

    float descriminant = powf(b, 2) - 4 * a * c;
    if (descriminant <= 0) {
        return {NAN, NAN, NAN};
    }

    float t0 = (-b + fsqrt(descriminant)) / (2 * a);
    float t1 = (-b - fsqrt(descriminant)) / (2 * a);

    Vector3D vector0 = (rayVector * t0);
    Vector3D vector1 = (rayVector * t1);

    if (vector0.LengthSquared() < vector1.LengthSquared()) {
        return vector0;
    }

    return vector1;
}   

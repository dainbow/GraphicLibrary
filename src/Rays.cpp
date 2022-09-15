#include "Rays.hpp"

bool VocabCmp(const Vector& vec1, const Vector& vec2) {
    return (vec1.x_ < vec2.x_) || (CmpFloat(vec1.x_, vec2.x_) && (vec1.y_ < vec2.y_));
}

bool TgCmp(const Vector& vec1, const Vector& vec2) {
    return (vec1.y_ / vec1.x_) <= (vec2.y_ / vec2.x_);
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

    Vector curTg = intersectionPoint - *zeroPoint;

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

const Vector3D* GetIntersectionSection(const Vector3D& intersectionPoint, const Matrix& planeCoefs) {
    Vector3D* points  = new Vector3D[planeCoefs.GetRows() - 1]();
    uint32_t curPoint = 0;

    for (uint32_t curPlane = 2; curPlane < planeCoefs.GetRows(); curPlane++) {
        // A1x + B1y + C1z + D1 = 0
        // A2x + B2y + C2z + D2 = 0
        // A3x + B3y + C3z + D3 = 0
        Matrix mainSystem(3, 3);
        Matrix extensionColumn(3, 1);

        for (uint32_t curRow = 0; curRow < 3; curRow++) {
            for (uint32_t curColumn = 0; curColumn < 3; curColumn++) {
                mainSystem.SetElem(curRow, curColumn, *planeCoefs.GetElem(curRow, curColumn));
            }

            extensionColumn.SetElem(curRow, 0, *planeCoefs.GetElem(curRow, 3));
        }

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
    }

    assert((curPoint > 2) && "CAN'T FIND POLYGON");
    bool result = CheckPointInPolygon3D(intersectionPoint, points, curPoint);
        
    delete[] points;
    if (result)
        return &intersectionPoint;

    return nullptr;
}

const Vector3D* GetIntersection(const Vector3D& rayPoint, const Vector3D& rayVector, const Matrix& planeCoefs) {
    if (planeCoefs.GetColumns() != 4)
        return nullptr;

    if (planeCoefs.GetRows() == 0) {
        return nullptr;
    }

    if (planeCoefs.GetRows() == 2) {
        return nullptr;
    }

    float intersectionTime = - (*planeCoefs.GetElem(0, 0) * rayPoint.x_ + *planeCoefs.GetElem(0, 1) * rayPoint.y_  + *planeCoefs.GetElem(0, 2) * rayPoint.z_ + *planeCoefs.GetElem(0, 3)) 
                            /  (*planeCoefs.GetElem(0, 0) * rayVector.x_+ *planeCoefs.GetElem(0, 1) * rayVector.y_ + *planeCoefs.GetElem(0, 2) * rayVector.z_);

    Vector3D intersectionPoint = {rayVector.x_ * intersectionTime + rayPoint.x_,
                                  rayVector.y_ * intersectionTime + rayPoint.y_,
                                  rayVector.z_ * intersectionTime + rayPoint.z_};

    if (planeCoefs.GetRows() > 2) {
        return GetIntersectionSection(intersectionPoint, planeCoefs);
    }

    return nullptr;
}

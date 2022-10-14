#pragma once

#include <cstring>

#include <thread>
#include <array>
#include <vector>

#include "Vector3d.hpp"
#include "Image.hpp"

#include "BaseObject.hpp"
#include "Matrix.hpp"

#include "Sphere.hpp"
#include "Plane.hpp"

#include "Button.hpp"
#include "Window.hpp"

constexpr double threadsAmount = 16.0;
const double LittleEpsilon = 3 * __FLT_EPSILON__;

enum class TracerGraphics {
    CustomGraphics,
    LowGraphics,
    MidGraphics,
    HightGraphics,
};

class SphereInfo;
class PlaneInfo;

void DeleteSphere(CustomButton<SphereInfo>* obj, const Vector& coords);
void DeletePlane(CustomButton<PlaneInfo>* obj, const Vector& coords);

class SphereInfo : public DynamicWindow {
    private:
        Sphere* spherePtr_;
        bool* isChanged_;

    public:
        Sphere* GetSpherePtr() {
            return spherePtr_;
        }

        SphereInfo(const SphereInfo& sphereInfo) = default;
        SphereInfo& operator=(const SphereInfo& sphereInfo) = default;

        SphereInfo(uint32_t x, uint32_t y, const MyColor& color, Sphere* spherePtr, bool* isChanged) :
        DynamicWindow(x, y, color),
        spherePtr_(spherePtr),
        isChanged_(isChanged)
        {
            Window* nameDeleteWindow = new Window(0, 0, 240, 30, 0);
            *this += nameDeleteWindow;

            *nameDeleteWindow += new CtrlTextField<std::string>(0, 0, 200, 30, 0x42aaff00, 0, 0xffffff00, 0, &spherePtr_->name_, 0);

            CustomButton<SphereInfo>* deleteButton    = new CustomButton<SphereInfo>(200, 0, 40, 30, 0xFF000000, 0, 0, this);
            deleteButton->onClick_ += new FuncCaller<CustomButton<SphereInfo>, Vector>(deleteButton, &DeleteSphere);
            *nameDeleteWindow += deleteButton;

            Window* coordsWindow = new Window(0, 0, 240, 30, 0);
            *this += coordsWindow;

            CustomButton<int>* xLabel = new CustomButton<int>(0, 0, 30, 30, 0, 0, 0, 0);
            xLabel->SetText("x:", 0xffffff00);
            *coordsWindow += xLabel;
            *coordsWindow += new CtrlTextField<double>(30, 0, 50, 30, 0x42aaff00, 0, 0xffffff00, 0, &spherePtr_->center_.x_, isChanged_);
            
            CustomButton<int>* yLabel = new CustomButton<int>(80, 0, 30, 30, 0, 0, 0, 0);
            yLabel->SetText("y:", 0xffffff00);
            *coordsWindow += yLabel;
            *coordsWindow += new CtrlTextField<double>(110, 0, 50, 30, 0x42aaff00, 0, 0xffffff00, 0, &spherePtr_->center_.y_, isChanged_);
            
            CustomButton<int>* zLabel = new CustomButton<int>(160, 0, 30, 30, 0, 0, 0, 0);
            zLabel->SetText("z:", 0xffffff00);
            *coordsWindow += zLabel;
            *coordsWindow += new CtrlTextField<double>(190, 0, 50, 30, 0x42aaff00, 0, 0xffffff00, 0, &spherePtr_->center_.z_, isChanged_);
            
            Window* radiusWindow = new Window(0, 0, 80, 30, 0);
            *this += radiusWindow;

            CustomButton<int>* rLabel = new CustomButton<int>(0, 0, 30, 30, 0, 0, 0, 0);
            rLabel->SetText("R:", 0xffffff00);
            *radiusWindow += rLabel;
            *radiusWindow += new CtrlTextField<double>(30, 0, 50, 30, 0x42aaff00, 0, 0xffffff00, 0, &spherePtr_->radius_, isChanged_);
        }
};

class PlaneInfo : public DynamicWindow {
    private:
        Plane* planePtr_;
        bool* isChanged_;

    public:
        Plane* GetPlanePtr() {
            return planePtr_;
        }

        PlaneInfo(const PlaneInfo& planeInfo) = default;
        PlaneInfo& operator=(const PlaneInfo& planeInfo) = default;

        PlaneInfo(uint32_t x, uint32_t y, const MyColor& color, Plane* planePtr, bool* isChanged) :
        DynamicWindow(x, y, color),
        planePtr_(planePtr),
        isChanged_(isChanged)
        {
            Window* nameDeleteWindow = new Window(0, 0, 240, 30, 0);
            *this += nameDeleteWindow;

            *nameDeleteWindow += new CtrlTextField<std::string>(0, 0, 200, 30, 0x42aaff00, 0, 0xffffff00, 0, &planePtr_->name_, 0);
            
            CustomButton<PlaneInfo>* deleteButton    = new CustomButton<PlaneInfo>(200, 0, 40, 30, 0xFF000000, 0, 0, this);
            deleteButton->onClick_ += new FuncCaller<CustomButton<PlaneInfo>, Vector>(deleteButton, &DeletePlane);
            *nameDeleteWindow += deleteButton;

            Window* mainWindow = new Window(0, 0, 240, 30, 0);
            *this += mainWindow;

            CustomButton<int>* aLabel = new CustomButton<int>(0, 0, 20, 30, 0, 0, 0, 0);
            aLabel->SetText("A", 0xffffff00);
            *mainWindow += aLabel;
            *mainWindow += new CtrlTextField<double>(20, 0, 40, 30, 0x42aaff00, 0, 0xffffff00, 0, &planePtr->plane_.array_[0][0], isChanged_);
        
            CustomButton<int>* bLabel = new CustomButton<int>(60, 0, 20, 30, 0, 0, 0, 0);
            bLabel->SetText("B", 0xffffff00);
            *mainWindow += bLabel;
            *mainWindow += new CtrlTextField<double>(80, 0, 40, 30, 0x42aaff00, 0, 0xffffff00, 0, &planePtr->plane_.array_[0][1], isChanged_);

            CustomButton<int>* cLabel = new CustomButton<int>(120, 0, 20, 30, 0, 0, 0, 0);
            cLabel->SetText("C", 0xffffff00);
            *mainWindow += cLabel;
            *mainWindow += new CtrlTextField<double>(140, 0, 40, 30, 0x42aaff00, 0, 0xffffff00, 0, &planePtr->plane_.array_[0][2], isChanged_);

            CustomButton<int>* dLabel = new CustomButton<int>(180, 0, 20, 30, 0, 0, 0, 0);
            dLabel->SetText("D", 0xffffff00);
            *mainWindow += dLabel;
            *mainWindow += new CtrlTextField<double>(200, 0, 40, 30, 0x42aaff00, 0, 0xffffff00, 0, &planePtr->plane_.array_[0][3], isChanged_);
        }
};

const int64_t tracerFrameTime = 100;

class Raytracer : public FlexImageWindow {
    private:
        bool isChanged_;

        bool isNewObjects_;
        List* objectsList_;

        std::vector<BaseObject*> objects_;

        double SuperSamplingCoef_;
        uint32_t MaxRecursionDepth_;

        Vector3D cameraCoords_;

        float virtualWidth_;
        float virtualHeight_;

        float displayDistance_;

        TracerGraphics graphSettings_;

        void ProcessRow(const double row, const Vector3D& cameraCoords, void* objectsVoid) {
            std::vector<BaseObject*> objects = *reinterpret_cast<std::vector<BaseObject*>*>(objectsVoid);

            for (double curX = double(-width_) / 2; curX < double(width_) / 2; curX++) {
                Vector3D sumColor = {0, 0, 0};

                for (uint32_t curSample = 0; curSample < uint32_t(SuperSamplingCoef_); curSample++) {
                    const double convertedY = (row  + GetRandomDouble()) * (virtualHeight_  / double(height_));
                    const double convertedX = (curX + GetRandomDouble()) * (virtualWidth_   / double(width_));

                    const Vector3D virtualPoint = {convertedX, convertedY, displayDistance_};

                    Ray curRay = {cameraCoords, virtualPoint - cameraCoords, 1.2};
                    curRay.vector_.Normalise();

                    sumColor += Vector3D(ProcessRay(objects, curRay, cameraCoords, 0));
                } 
                sumColor *= (1.0 / SuperSamplingCoef_);
                
                image_.SetPixel(uint32_t(curX + (double(width_) / 2)), uint32_t(row + (double(height_) / 2)), MyColor(sumColor.x_, sumColor.y_, sumColor.z_));
            }
        }

        Ray FindClosestObject(std::vector<BaseObject*>& objects, const Ray& curRay, MyColor* colorAbsorbed, uint32_t* objectNumber) {
            Ray leastRay = {{NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};

            double leastDistance   = __DBL_MAX__;
            uint32_t leastObject   = UINT32_MAX;
            MyColor leastColor     = 0;

            for (uint32_t curObject = 0; curObject < objects.size(); curObject++) {
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

        MyColor ProcessRay(std::vector<BaseObject*>& objects,
                   const Ray& curRay, const Vector3D& camCoords,
                   const uint32_t curDepth) {
            if (curDepth == MaxRecursionDepth_)
                return 0;               

            MyColor colorAbsorbed = 0;

            uint32_t leastObject = UINT32_MAX;
            Ray processedRay = FindClosestObject(objects, curRay, &colorAbsorbed, &leastObject);
            
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
        
            MyColor newColor = ProcessRay(objects, processedRay, camCoords, curDepth + 1);

            return emitted + colorAbsorbed * newColor;
        }

    public:
        double renderRow_;

        void FillList(List* listToFill) {
            for (auto& curObject : objects_) {
                const std::type_info& curObjInfo = typeid(*curObject);

                if (strstr(curObjInfo.name(), "Sphere")) {
                    *listToFill += new SphereInfo(0, 0, 0x00FF0000, (Sphere*)curObject, &isChanged_);
                }
                else if (strstr(curObjInfo.name(), "Plane")) {
                    *listToFill += new PlaneInfo(0, 0, 0x00FF0000, (Plane*)curObject, &isChanged_);
                }
            }
        }

        Raytracer(const Raytracer& tracer)            = default;
        Raytracer& operator=(const Raytracer& tracer) = default;

        Raytracer(uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
        double SuperSamplingCoef, uint32_t maxRecurs,
        const Vector3D& cameraCoords,
        float virtualW, float virtualH,
        float displayD, List* objects = nullptr):

        FlexImageWindow(x, y, width, height),
        isChanged_(1), isNewObjects_(1), 
        objectsList_(objects),
        objects_(),
        SuperSamplingCoef_(SuperSamplingCoef), MaxRecursionDepth_(maxRecurs),
        cameraCoords_(cameraCoords),
        virtualWidth_(virtualW), virtualHeight_(virtualH),
        displayDistance_(displayD),
        graphSettings_(TracerGraphics::CustomGraphics),
        renderRow_(-double(height) / 2)
        {
            
        }

        TracerGraphics GetGraphics() {
            return graphSettings_;
        }

        void SetLowGraphics() {
            SuperSamplingCoef_ = 1;
            MaxRecursionDepth_ = 1;
            
            isChanged_ = 1;
            graphSettings_ = TracerGraphics::LowGraphics;
        }

        void SetMidGraphics() {
            SuperSamplingCoef_ = 5;
            MaxRecursionDepth_ = 4;

            isChanged_ = 1;
            graphSettings_ = TracerGraphics::MidGraphics;
        }

        void SetHighGraphics() {
            SuperSamplingCoef_ = 20;
            MaxRecursionDepth_ = 8;

            isChanged_ = 1;
            graphSettings_ = TracerGraphics::HightGraphics;
        }

        virtual void operator+=(BaseObject* obj) {
            obj->parent_ = this;
            objects_.push_back(obj);

            isChanged_ = 1;
            isNewObjects_ = 1;
        }

        virtual void operator-=(BaseObject* obj) {
            for (auto it = objects_.begin(); it != objects_.end(); it++) {
                if (*it == obj) {
                    objects_.erase(it);

                    delete obj;
                    break;
                }
            }

            obj->parent_  = nullptr;
            isChanged_    = 1;
            isNewObjects_ = 1;
        }

        void Draw(const int64_t& time) override {
            if (isChanged_ == 0) {
                FlexImageWindow::Draw(time);

                return;
            }

            if (CmpDbl(renderRow_, (double(height_) / 2))) {
                renderRow_ = -double(height_) / 2;
            }

            if (CmpDbl(renderRow_, -double(height_) / 2)) {
                image_.Clear();
            }

            if (isNewObjects_) {
                objectsList_->Clear();
                objectsList_->maxHeight_ = 0;

                FillList(objectsList_);

                isNewObjects_ = 0;
            }

            std::array<std::thread, uint64_t(threadsAmount)> threads;

            for (double curY = renderRow_; curY < double(height_) / 2; curY += threadsAmount) {
                if ((GetTimeMiliseconds() - time) > tracerFrameTime) {
                    return;
                }

                for (double curThread = 0; (curThread < threadsAmount) && ((curY + curThread) < double(height_) / 2); curThread++) {
                    threads[uint32_t(curThread)] = std::thread(&Raytracer::ProcessRow, this, curY + curThread, cameraCoords_, &objects_);
                    
                }

                for (double curThread = 0; (curThread < threadsAmount) && ((curY + curThread) < double(height_) / 2); curThread++) {
                    threads[uint32_t(curThread)].join();
                    renderRow_++;
                }

                FlexImageWindow::Draw(time);
            }

            isChanged_ = 0;
            FlexImageWindow::Draw(time);
        }

        ~Raytracer() {
            for (auto& curObject : objects_) {
                delete curObject;
            }
        }
};

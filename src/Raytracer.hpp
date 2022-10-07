#pragma once

#include <thread>
#include <array>
#include <vector>

#include "Vector3d.hpp"
#include "Image.hpp"

#include "BaseObject.hpp"
#include "Matrix.hpp"
#include "Sphere.hpp"

#include "Window.hpp"

constexpr double threadsAmount = 16.0;
const double LittleEpsilon = 3 * __FLT_EPSILON__;

enum class TracerGraphics {
    CustomGraphics,
    LowGraphics,
    MidGraphics,
    HightGraphics,
};

class Raytracer : public Window {
    private:
        MyColor** rawData_;
        bool isChanged;

        std::vector<BaseObject*> objects_;

        double SuperSamplingCoef_;
        uint32_t MaxRecursionDepth_;

        Vector3D cameraCoords_;

        float virtualWidth_;
        float virtualHeight_;

        float displayDistance_;

        TracerGraphics graphSettings_;

        void ProcessRow(MyColor* curRow, const double row, const Vector3D& cameraCoords, void* objectsVoid) {
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
                
                curRow[uint32_t(curX + (double(width_) / 2))] = MyColor(sumColor.x_, sumColor.y_, sumColor.z_);
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
        Raytracer(const Raytracer& tracer)            = default;
        Raytracer& operator=(const Raytracer& tracer) = default;

        Raytracer(uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
        double SuperSamplingCoef, uint32_t maxRecurs,
        const Vector3D& cameraCoords,
        float virtualW, float virtualH,
        float displayD):

        Window(x, y, width, height, 0),
        rawData_(new MyColor*[uint32_t(height_)]), isChanged(1),
        objects_(),
        SuperSamplingCoef_(SuperSamplingCoef), MaxRecursionDepth_(maxRecurs),
        cameraCoords_(cameraCoords),
        virtualWidth_(virtualW), virtualHeight_(virtualH),
        displayDistance_(displayD),
        graphSettings_(TracerGraphics::CustomGraphics)
        {
            for (double curRow = 0; curRow < double(height_); curRow++) {
                rawData_[uint32_t(curRow)] = new MyColor[uint32_t(width_)];
            }
        }

        TracerGraphics GetGraphics() {
            return graphSettings_;
        }

        void SetLowGraphics() {
            SuperSamplingCoef_ = 1;
            MaxRecursionDepth_ = 2;
            
            isChanged = 1;
            graphSettings_ = TracerGraphics::LowGraphics;
        }

        void SetMidGraphics() {
            SuperSamplingCoef_ = 5;
            MaxRecursionDepth_ = 4;

            isChanged = 1;
            graphSettings_ = TracerGraphics::MidGraphics;
        }

        void SetHighGraphics() {
            SuperSamplingCoef_ = 20;
            MaxRecursionDepth_ = 8;

            isChanged = 1;
            graphSettings_ = TracerGraphics::HightGraphics;
        }

        void operator+=(BaseObject* obj) {
            objects_.push_back(obj);
        }

        void Draw([[maybe_unused]] const int64_t& time) override {
            Image curImage = {};
            curImage.Create(uint32_t(width_), uint32_t(height_), {0x00, 0x00, 0x00});

            if (isChanged == 0) {
                for (double curY = double(-height_) / 2; curY < double(height_) / 2; curY++) {
                    for (double curX = (double(-width_) / 2); curX < (double(width_) / 2); curX++) {
                        MyColor newColor = rawData_[uint32_t(curY + (double(height_) / 2))][uint32_t(curX + (double(width_) / 2))];
                        MyColor gammaColor = {sqrt(newColor.fRed_),
                                              sqrt(newColor.fGreen_),
                                              sqrt(newColor.fBlue_)};

                        curImage.SetPixel(uint32_t(curX + (double(width_) / 2)), uint32_t(curY + (double(height_) / 2)), gammaColor);
                    }
                }

                curImage.Draw(ptrToRealWdw_, ConvertXY(0, 0));
                return;
            }

            std::array<std::thread, uint64_t(threadsAmount)> threads;

            for (double curY = double(-height_) / 2; curY < double(height_) / 2; curY += threadsAmount) {
                for (double curThread = 0; (curThread < threadsAmount) && ((curY + curThread) < double(height_) / 2); curThread++) {
                    threads[uint32_t(curThread)] = std::thread(&Raytracer::ProcessRow, this, rawData_[uint32_t(curY + curThread + double(height_) / 2)], curY + curThread,
                                                                cameraCoords_, &objects_);
                    
                }

                for (double curThread = 0; (curThread < threadsAmount) && ((curY + curThread) < double(height_) / 2); curThread++) {
                    threads[uint32_t(curThread)].join();
                }

                for (double curRenderY = curY; (curRenderY < (curY + threadsAmount)) && (curRenderY < (double(height_) / 2)); curRenderY++) {
                    for (double curX = (double(-width_) / 2); curX < (double(width_) / 2); curX++) {
                        MyColor newColor = rawData_[uint32_t(curRenderY + (double(height_) / 2))][uint32_t(curX + (double(width_) / 2))];
                        MyColor gammaColor = {sqrt(newColor.fRed_),
                                              sqrt(newColor.fGreen_),
                                              sqrt(newColor.fBlue_)};

                        curImage.SetPixel(uint32_t(curX + (double(width_) / 2)), uint32_t(curRenderY + (double(height_) / 2)), gammaColor);
                    }
                }
            }

            curImage.Draw(ptrToRealWdw_, ConvertXY(0, 0));
            isChanged = 0;
        }

        ~Raytracer() {
            for (auto& curObject : objects_) {
                delete curObject;
            }

            for (double curRow = 0; curRow < double(height_); curRow++) {
                delete[] rawData_[uint32_t(curRow)];
            }

            delete rawData_;
        }
};

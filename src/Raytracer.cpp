#include "Raytracer.hpp"

void DeleteSphere(CustomButton<SphereInfo>* obj, const Vector& coords) {
    if (obj->IsClicked(coords)) {
        *obj->GetContext()->GetSpherePtr()->parent_ -= (BaseObject*)obj->GetContext()->GetSpherePtr(); 
    }
}

void DeletePlane(CustomButton<PlaneInfo>* obj, const Vector& coords) {
    if (obj->IsClicked(coords)) {
        *obj->GetContext()->GetPlanePtr()->parent_ -= (BaseObject*)obj->GetContext()->GetPlanePtr(); 
    }
}

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

void AddMaterialInfo(DynamicWindow* materialWindow, Material* material, bool* isChanged) {
    const std::type_info& curObjInfo = typeid(*material);

    CustomButton<int>* materialLabel = new CustomButton<int>(0, 0, 240, 30, nullptr);
    *materialWindow += materialLabel;

    std::cout << curObjInfo.name() << std::endl;
    if (strstr(curObjInfo.name(), "Scattering")) {
        materialLabel->SetText("Scattering", 0xffffff00);

        AddTextureInfo(materialWindow, ((Scattering*)material)->albedo_, isChanged);
    }
    else if (strstr(curObjInfo.name(), "Transparent")) {
        materialLabel->SetText("Transparent", 0xffffff00);
    }
    else if (strstr(curObjInfo.name(), "DiffLight")) {
        materialLabel->SetText("Light", 0xffffff00);

        AddTextureInfo(materialWindow, ((DiffLight*)material)->emitLight_, isChanged);
    }
    else if (strstr(curObjInfo.name(), "Mirror")) {
        materialLabel->SetText("Mirror", 0xffffff00);

        AddTextureInfo(materialWindow, ((Mirror*)material)->albedo_, isChanged);
    }
}

void AddTextureInfo(DynamicWindow* curWindow, const Texture* texture, bool* isChanged) {
    const std::type_info& curObjInfo = typeid(*texture);

    if (strstr(curObjInfo.name(), "ConstColor")) {
        Window* colorWindow = new Window(0, 0, 240, 30);
        *curWindow += colorWindow;

        CustomButton<int>* rLabel = new CustomButton<int>(0, 0, 30, 30, nullptr);
        rLabel->SetText("R:", 0xffffff00);
        *colorWindow += rLabel;
        *colorWindow += new CtrlTextField<double>(30, 0, 50, 30, &((const_cast<ConstColor*>(reinterpret_cast<const ConstColor*>(texture)))->color_.fRed_), isChanged);
        
        CustomButton<int>* gLabel = new CustomButton<int>(80, 0, 30, 30, nullptr);
        gLabel->SetText("G:", 0xffffff00);
        *colorWindow += gLabel;
        *colorWindow += new CtrlTextField<double>(110, 0, 50, 30, &((const_cast<ConstColor*>(reinterpret_cast<const ConstColor*>(texture)))->color_.fGreen_), isChanged);
        
        CustomButton<int>* bLabel = new CustomButton<int>(160, 0, 30, 30, nullptr);
        bLabel->SetText("B:", 0xffffff00);
        *colorWindow += bLabel;
        *colorWindow += new CtrlTextField<double>(190, 0, 50, 30, &((const_cast<ConstColor*>(reinterpret_cast<const ConstColor*>(texture)))->color_.fBlue_), isChanged);
    }
    else if (strstr(curObjInfo.name(), "Squares")) {
        AddTextureInfo(curWindow, ((const_cast<Squares*>(reinterpret_cast<const Squares*>(texture))))->oddTexture_, isChanged);
        AddTextureInfo(curWindow, (const_cast<Squares*>(reinterpret_cast<const Squares*>(texture)))->evenTexture_, isChanged);
    }
}

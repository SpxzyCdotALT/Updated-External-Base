#include "../../Engine.h"
#include <Globals.hxx>

namespace SDK {

    void Lighting::SetAmbient(uintptr_t LightingAddress, SDK::Vector3 Color)
    {
        Driver->Write<SDK::Vector3>(LightingAddress + Offsets::Lighting::Ambient, Color);
        Driver->Write<SDK::Vector3>(LightingAddress + Offsets::Lighting::OutdoorAmbient, Color);
    }

    void Lighting::SetFog(uintptr_t LightingAddress, float End, SDK::Vector3 Color)
    {
        Driver->Write<float>(LightingAddress + Offsets::Lighting::FogEnd, End);
        Driver->Write<SDK::Vector3>(LightingAddress + Offsets::Lighting::FogColor, Color);
    }

    void Lighting::SetBrightness(uintptr_t LightingAddress, float Value)
    {
        Driver->Write<float>(LightingAddress + Offsets::Lighting::Brightness, Value);
    }

    void Lighting::SetExposure(uintptr_t LightingAddress, float Value)
    {
        Driver->Write<float>(LightingAddress + Offsets::Lighting::ExposureCompensation, Value);
    }

    void Lighting::SetFOV(uintptr_t CameraAddress, float Degrees)
    {
        Driver->Write<float>(CameraAddress + Offsets::Camera::FieldOfView, Degrees * 0.0174533f);
    }
}
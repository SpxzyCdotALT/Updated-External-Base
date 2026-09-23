#include "../../Engine.h"

namespace SDK {

    float Humanoid::Get_Health() const
    {
        if (!this->Address)
            return 0.0f;

        union Conv
        {
            std::uint64_t hex;
            float f;
        } EasyConversion;

        uintptr_t healthPtr = Driver->Read<uintptr_t>(this->Address + Offsets::Humanoid::Health);
        uintptr_t healthRead = Driver->Read<uintptr_t>(healthPtr);
        EasyConversion.hex = healthPtr ^ healthRead;

        return EasyConversion.f;
    }

    float Humanoid::Get_MaxHealth() const
    {
        if (!this->Address)
            return 0.0f;

        union Conv
        {
            std::uint64_t hex;
            float f;
        } EasyConversion;

        uintptr_t healthPtr = Driver->Read<uintptr_t>(this->Address + Offsets::Humanoid::MaxHealth);
        uintptr_t healthRead = Driver->Read<uintptr_t>(healthPtr);
        EasyConversion.hex = healthPtr ^ healthRead;

        return EasyConversion.f;
    }

    void Humanoid::Kill() const
    {
        if (!this->Address)
            return;

        Driver->Write<uintptr_t>(this->Address + Offsets::Humanoid::Health, 0);
    }

    int Humanoid::Get_RigType() const
    {
        if (!this->Address)
            return 0;

        return Driver->Read<int>(this->Address + Offsets::Humanoid::RigType);
    }
}

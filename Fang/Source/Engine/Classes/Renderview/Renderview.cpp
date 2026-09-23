#include "../../Engine.h"
#include <Globals.hxx>

namespace SDK {

    std::uint64_t Renderview::GetRenderview() {

        auto Rv1 = Driver->Read<std::uintptr_t>(Globals::Datamodel.Address + 0x1D0);
        auto Rv2 = Driver->Read<std::uintptr_t>(Rv1 + 0x8);
        auto Rv3 = Driver->Read<std::uintptr_t>(Rv2 + 0x28);
        return Rv3;
    }

    void Renderview::InvalidateLighting() {

        Driver->Write<bool>(Globals::Renderview.GetRenderview() + 0x148, false);
    }

    void Renderview::ValidateSkybox() {

        Driver->Write<bool>(Globals::Renderview.GetRenderview() + 0x2cd, false);
    }
}

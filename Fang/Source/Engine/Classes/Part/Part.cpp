#include "../../Engine.h"

namespace SDK {

    SDK::Vector3 Part::Get_Position() const {

        return Driver->Read<SDK::Vector3>(Address + Offsets::Primitive::Position);
    }

    SDK::Matrix3 Part::Get_Rotation() const {

        return Driver->Read<SDK::Matrix3>(Address + Offsets::Primitive::Rotation);
    }

    SDK::Vector3 Part::Get_Size() const {

        return Driver->Read<SDK::Vector3>(Address + Offsets::Primitive::Size);
    }

    SDK::Vector3 Part::Get_MoveDir() const {

        return Driver->Read<SDK::Vector3>(Address + Offsets::Player::CameraMode);
    }

    SDK::Vector3 Part::Get_Velocity() const {

        if (!Address) return {};
        uintptr_t primitive = Driver->Read<uintptr_t>(Address + Offsets::BasePart::Primitive);
        if (!primitive) return {};
        return Driver->Read<SDK::Vector3>(primitive + Offsets::Primitive::AssemblyAngularVelocity);
    }

    bool Part::Get_Anchored() const {

        return Driver->Read<bool>(Address + Offsets::PrimitiveFlags::Anchored);
    }

    SDK::Vector3 Part::Get_CFrame() const {

        return Driver->Read<SDK::Vector3>(Address + Offsets::Primitive::Rotation);
    }

    Part Part::Get_Primitive() const {

        uintptr_t primitiveAddress = Driver->Read<uintptr_t>(Address + Offsets::BasePart::Primitive);
        return Part{ primitiveAddress };
    }

    SDK::Vector3 Part::Get_PartPosition() const {

        Part primitive = Get_Primitive();
        if (!primitive.Address) return {};
        return Driver->Read<SDK::Vector3>(primitive.Address + Offsets::Primitive::Position);
    }

    float Part::Get_Transparency() const {

        return Driver->Read<float>(Address + Offsets::BasePart::Transparency);
    }

    void Part::Write_Velocity(const SDK::Vector3& Velocity) const {

        Part primitive = Get_Primitive();
        if (!primitive.Address) return;
        Driver->Write<SDK::Vector3>(primitive.Address + Offsets::Primitive::AssemblyLinearVelocity, Velocity);
    }

    void Part::Set_PartPosition(const SDK::Vector3& Position) const {

        Part primitive = Get_Primitive();
        if (!primitive.Address) return;
        Driver->Write<SDK::Vector3>(primitive.Address + Offsets::Primitive::Position, Position);
    }

    void Part::Set_Rotation(const SDK::Matrix3& Rotation) const {

        Driver->Write<SDK::Matrix3>(Address + Offsets::Primitive::Rotation, Rotation);
    }

    void Part::Set_MeshID(const std::string& MeshID) const {

        Driver->Write_String(Address + Offsets::MeshPart::MeshId, MeshID);
    }

    void Part::Set_Transparency(float Transparency) const {

        Driver->Write<float>(Address + Offsets::BasePart::Transparency, Transparency);
    }

    void Part::Write_MoveDir(const SDK::Vector3& MoveDir) const {

        Driver->Write<SDK::Vector3>(Address + Offsets::Player::CameraMode, MoveDir);
    }
}
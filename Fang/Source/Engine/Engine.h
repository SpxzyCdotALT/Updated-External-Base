#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../Driver/Driver.h"
#include "Offsets/Offsets.h"
#include "Math/Math.h"

namespace SDK {

    struct ViewPort {

        unsigned short x, y;
    };

    struct Instance {
        uintptr_t Address = 0;

        Instance() = default;
        explicit Instance(uintptr_t addr) : Address(addr) {}

        std::string Name() const;
        std::string Text() const;
        std::string Class() const;

        Instance Parent() const;
        std::vector<Instance> Children() const;
        Instance Character() const;

        Instance Find_First_Child(const std::string& Name) const;
        Instance Find_First_Child_Of_Class(const std::string& Class_Name) const;

        template <typename T>
        std::vector<T> GetChildren() const
        {
            std::vector<T> children;
            if (!Address) return children;

            for (SDK::Instance child : Children())
            {
                if (!child.Address) continue;
                children.emplace_back(T(child.Address));
            }

            return children;
        }
    };

    struct Datamodel : public Instance {

        using Instance::Instance;
        std::uint64_t Get_PlaceID() const;
        std::uint64_t Get_GameID() const;
        std::uint64_t Get_CreatorID() const;
        std::uint64_t Get_ServerIP() const;
    };

    struct VisualEngine : public Instance {

        using Instance::Instance;
        SDK::Vector2 Get_Dimensions() const;
        SDK::Matrix4 Get_ViewMatrix() const;
        SDK::Vector2 World_To_Screen(const SDK::Vector3& World) const;
    };

    struct Players : public Instance {

        using Instance::Instance;
        std::uint64_t Get_UserID() const;
        std::uint64_t Get_Team() const;
        std::string Get_DisplayName() const;
        Players Get_Local_Player() const;
    };

    struct Camera : public Instance {

        using Instance::Instance;
        SDK::Vector3 Get_CameraPos() const;
        SDK::Matrix3 Get_CameraRot() const;
        void Set_CameraPos(const SDK::Vector3& pos) const;
        void Set_CameraRot(const SDK::Matrix3& rot) const;

        Vector2 FetchViewPort(Vector2 target_screen_pos, Vector2 screen_size);
        void SetViewPort(SDK::ViewPort Vp) const;
    };

    struct Humanoid : public Instance {

        using Instance::Instance;
        float Get_Health() const;
        float Get_MaxHealth() const;
        void Kill() const;
        int Get_RigType() const;
    };

    struct Part : public Instance {

        using Instance::Instance;
        SDK::Vector3 Get_Position() const;
        SDK::Matrix3 Get_Rotation() const;
        SDK::Vector3 Get_Size() const;
        SDK::Vector3 Get_MoveDir() const;
        SDK::Vector3 Get_Velocity() const;
        SDK::Vector3 Get_CFrame() const;
        SDK::Vector3 Get_PartPosition() const;
        Part Get_Primitive() const;
        float Get_Transparency() const;
        bool Get_Anchored() const;

        void Write_Velocity(const SDK::Vector3& Velocity) const;
        void Set_PartPosition(const SDK::Vector3& Position) const;
        void Set_Rotation(const SDK::Matrix3& Rotation) const;
        void Set_MeshID(const std::string& MeshID) const;
        void Set_Transparency(float Transparency) const;
        void Write_MoveDir(const SDK::Vector3& MoveDir) const;
    };

    struct Renderview : public Instance {

        using Instance::Instance;
        std::uint64_t GetRenderview();

        static void InvalidateLighting();
        static void ValidateSkybox();
    };

    struct Lighting : public Instance {

        using Instance::Instance;

        static void SetAmbient(uintptr_t LightingAddress, SDK::Vector3 Color);
        static void SetFog(uintptr_t LightingAddress, float End, SDK::Vector3 Color);
        static void SetBrightness(uintptr_t LightingAddress, float Value);
        static void SetExposure(uintptr_t LightingAddress, float Value);
        static void SetFOV(uintptr_t CameraAddress, float Degrees);
    };
}
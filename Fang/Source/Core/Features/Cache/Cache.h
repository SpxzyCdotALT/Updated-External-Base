#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <string_view>

#include <atomic>
#include <cmath>

#include "../../../Engine/Engine.h"

namespace Cache
{
    extern std::mutex Mutex;
    extern std::atomic<bool> Is_Running;
    void RunService();
}

namespace SDK
{
    class Player
    {
    public:
		bool PartsCached = false;
        bool Local_Player;

        float Health;
        float MaxHealth;
        float Distance = 0.f;

        std::uint8_t Rig_Type;

        SDK::Instance Team;
        std::string Name;
        std::string Display_Name;
        std::string Tool_Name;
        std::vector<SDK::Instance> Children;
        std::vector<SDK::Instance> Bones;
        SDK::Players Player;
        SDK::Instance Character;
        std::uint64_t UserID;
        SDK::Instance Head;
        SDK::Instance HumanoidRootPart;
        SDK::Instance Humanoid;

        SDK::Instance LeftArm;
        SDK::Instance RightArm;

        SDK::Instance LeftHand;
        SDK::Instance RightHand;
        SDK::Instance LeftLowerArm;
        SDK::Instance RightLowerArm;
        SDK::Instance LeftUpperArm;
        SDK::Instance RightUpperArm;

        SDK::Instance LeftFoot;
        SDK::Instance RightFoot;
        SDK::Instance RightUpperLeg;
        SDK::Instance LeftUpperLeg;
        SDK::Instance LeftLowerLeg;
        SDK::Instance RightLowerLeg;

        SDK::Instance UpperTorso;
        SDK::Instance LowerTorso;
        SDK::Instance Torso;

        SDK::Instance LeftLeg;
        SDK::Instance RightLeg;
    };
}
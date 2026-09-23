#include "Engine/Engine.h"
#include "Globals.hxx"
#include "Core/Features/Cache/Cache.h"

SDK::Vector3 LookVector(const SDK::Matrix3& rotationMatrix) {
    return { rotationMatrix.data[2], rotationMatrix.data[2 + 3], rotationMatrix.data[2 + 6] };
}

SDK::Vector3 RightVector(const SDK::Matrix3& rotationMatrix) {
    return { rotationMatrix.data[0], rotationMatrix.data[0 + 3], rotationMatrix.data[0 + 6] };
}

namespace Misc {

    /*void Fly() {
        bool AirCheck = false;

        while (true) {
            if (!Globals::Misc::Fly || Globals::Misc::Fly_Speed <= 0.f) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            auto& lp = Globals::LocalPlayer;
            if (!lp.HumanoidRootPart.Address || !Globals::Camera.Address) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            SDK::Part HumanoidRootPart(lp.HumanoidRootPart.Address);
            SDK::Camera Camera(Globals::Camera.Address);

            SDK::Matrix3 CamRot = Camera.Get_CameraRot();
            SDK::Vector3 LookVec = LookVector(CamRot);
            SDK::Vector3 RightVec = RightVector(CamRot);
            SDK::Vector3 Direction(0, 0, 0);

            if (GetAsyncKeyState('W') & 0x8000) {
                Direction = Direction - LookVec;
                AirCheck = true;
            }
            if (GetAsyncKeyState('S') & 0x8000) {
                Direction = Direction + LookVec;
                AirCheck = true;
            }
            if (GetAsyncKeyState('A') & 0x8000) {
                Direction = Direction - RightVec;
                AirCheck = true;
            }
            if (GetAsyncKeyState('D') & 0x8000) {
                Direction = Direction + RightVec;
                AirCheck = true;
            }

            if(GetAsyncKeyState(VK_SPACE) & 0x8000) {
                Direction.y += 1.0f;
                AirCheck = true;
            }

            if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) {
                Direction.y -= 1.0f;
                AirCheck = true;
            }

            if (Direction.magnitude() > 0)
                Direction = Direction.normalize();

            if (!AirCheck) {
                HumanoidRootPart.Write_Velocity({ 0.f, 0.f, 0.f });
            }
            else {
                SDK::Vector3 Velocity = { Direction.x * Globals::Misc::Fly_Speed, Direction.y * Globals::Misc::Fly_Speed, Direction.z * Globals::Misc::Fly_Speed };
                HumanoidRootPart.Write_Velocity(Velocity);
            }

            AirCheck = false;
        }
    }

    void RunService() {
        std::thread(Fly).detach();
    }*/
}
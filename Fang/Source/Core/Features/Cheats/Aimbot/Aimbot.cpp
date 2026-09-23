#include <Windows.h>
#include <thread>
#include <cmath>
#include <string>
#include <algorithm>
#include <vector>

#include "Aimbot.h"
#include "../../Cache/Cache.h"
#include "../../../../Engine/Engine.h"
#include <Globals.hxx>
#include "../../../../Engine/Offsets/Offsets.h"

namespace Aimbot {
    std::string CurrentLockedName = "";
    SDK::Vector3 AimPositionW = { 0, 0, 0 };
    SDK::Vector2 AimPositionS = { 0, 0 };
    bool TargetFound = false;

    std::string PersistenceName = "";
    bool IsPersisting = false;

    static bool IsPlayerKnocked(SDK::Player& Player)
    {
        if (Player.Character.Address == 0)
            return false;

        SDK::Instance BodyEffects = Player.Character.Find_First_Child("BodyEffects");
        if (BodyEffects.Address == 0)
            return false;

        SDK::Instance Ko = BodyEffects.Find_First_Child("K.O");
        if (Ko.Address == 0)
            return false;

        bool KoValue = Driver->Read<bool>(Ko.Address + Offsets::Misc::Value);

        return KoValue;
    }

    static SDK::Matrix3 LerpMatrix3(const SDK::Matrix3& A, const SDK::Matrix3& B, float T) {
        SDK::Matrix3 Result;
        for (int I = 0; I < 9; ++I) Result.data[I] = A.data[I] + (B.data[I] - A.data[I]) * T;
        return Result;
    }

    static SDK::Vector3 Cross(const SDK::Vector3& A, const SDK::Vector3& B) {
        return { A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x };
    }

    static SDK::Matrix3 LookAtToMatrix(const SDK::Vector3& CamPos, const SDK::Vector3& TargetPos) {
        SDK::Vector3 Forward = (TargetPos - CamPos).normalize();
        SDK::Vector3 Right = Cross({ 0.f, 1.f, 0.f }, Forward).normalize();
        SDK::Vector3 Up = Cross(Forward, Right);

        SDK::Matrix3 M;
        M.data[0] = -Right.x; M.data[1] = Up.x; M.data[2] = -Forward.x;
        M.data[3] = Right.y;  M.data[4] = Up.y; M.data[5] = -Forward.y;
        M.data[6] = -Right.z; M.data[7] = Up.z; M.data[8] = -Forward.z;
        return M;
    }

    void AcquireTarget() {
        TargetFound = false;
        if (!Globals::VisualEngine.Address) return;

        POINT CursorPos;
        if (!GetCursorPos(&CursorPos)) return;
        ScreenToClient(FindWindowA(0, "Roblox"), &CursorPos);

        float ClosestDistance = 999999.f;
        std::string BestName = "";
        uintptr_t BestCharacterAddr = 0;

        SDK::Datamodel Dm(Globals::Datamodel.Address);
        SDK::Instance WorkspaceInst = Dm.Find_First_Child_Of_Class("Workspace");
        SDK::Instance CameraInst = WorkspaceInst.Find_First_Child("Camera");
        if (!CameraInst.Address) return;

        SDK::Camera Cam(CameraInst.Address);
        auto CameraOrigin = Cam.Get_CameraPos();

        if (Globals::Aimbot::AimbotSticky && IsPersisting && !PersistenceName.empty()) {
            for (auto& Plr : Globals::Player_Cache) {
                if (Plr.Name != PersistenceName) continue;
                if (Plr.Local_Player || !Plr.Character.Address || !Plr.Head.Address) {
                    IsPersisting = false;
                    PersistenceName = "";
                    Globals::Aimbot::AimTarget = SDK::Instance(0);
                    return;
                }

                std::vector<SDK::Vector3> Bones;
                if (Plr.Head.Address) Bones.push_back(SDK::Part(Plr.Head.Address).Get_PartPosition());
                if (Plr.Torso.Address) Bones.push_back(SDK::Part(Plr.Torso.Address).Get_PartPosition());
                if (Plr.LowerTorso.Address) Bones.push_back(SDK::Part(Plr.LowerTorso.Address).Get_PartPosition());

                if (Bones.empty()) {
                    IsPersisting = false;
                    PersistenceName = "";
                    Globals::Aimbot::AimTarget = SDK::Instance(0);
                    return;
                }

                int HitboxIdx = Globals::Aimbot::HitPart;
                if (HitboxIdx >= Bones.size()) HitboxIdx = 0;
                if (HitboxIdx == 4) HitboxIdx = rand() % Bones.size();

                SDK::Vector3 BonePos = Bones[HitboxIdx];
                if (std::isnan(BonePos.x)) {
                    IsPersisting = false;
                    PersistenceName = "";
                    Globals::Aimbot::AimTarget = SDK::Instance(0);
                    return;
                }

                SDK::VisualEngine Ve(Globals::VisualEngine.Address);
                auto ScreenPos = Ve.World_To_Screen(BonePos);

                float Dist2D = sqrtf((ScreenPos.x - CursorPos.x) * (ScreenPos.x - CursorPos.x) + (ScreenPos.y - CursorPos.y) * (ScreenPos.y - CursorPos.y));

                if (Globals::Aimbot::Aimbot_type == 1 && Dist2D > Globals::Aimbot::FovSize) {
                    IsPersisting = false;
                    PersistenceName = "";
                    Globals::Aimbot::AimTarget = SDK::Instance(0);
                    return;
                }

                AimPositionW = BonePos;
                AimPositionS = { ScreenPos.x, ScreenPos.y };
                CurrentLockedName = Plr.Name;
                Globals::Aimbot::AimTarget = SDK::Instance(Plr.Character.Address);
                TargetFound = true;
                return;
            }

            IsPersisting = false;
            PersistenceName = "";
            Globals::Aimbot::AimTarget = SDK::Instance(0);
            return;
        }

        for (auto& Plr : Globals::Player_Cache) {
            if (Plr.Local_Player || !Plr.Character.Address || !Plr.Head.Address) continue;

            if (Globals::Aimbot::KnockedCheck && IsPlayerKnocked(Plr))
                continue;

            std::vector<SDK::Vector3> Bones;
            if (Plr.Head.Address) Bones.push_back(SDK::Part(Plr.Head.Address).Get_PartPosition());
            if (Plr.Torso.Address) Bones.push_back(SDK::Part(Plr.Torso.Address).Get_PartPosition());
            if (Plr.HumanoidRootPart.Address) Bones.push_back(SDK::Part(Plr.HumanoidRootPart.Address).Get_PartPosition());
            if (Bones.empty()) continue;

            int HitboxIdx = Globals::Aimbot::HitPart;
            if (HitboxIdx >= Bones.size()) HitboxIdx = 0;
            if (HitboxIdx == 4) HitboxIdx = rand() % Bones.size();

            SDK::Vector3 BonePos = Bones[HitboxIdx];
            if (std::isnan(BonePos.x)) continue;

            float Dist3D = (CameraOrigin - BonePos).magnitude();
            if (Dist3D > 700.f) continue;

            SDK::VisualEngine Ve(Globals::VisualEngine.Address);
            auto ScreenPos = Ve.World_To_Screen(BonePos);

            RECT ClientRect;
            GetClientRect(FindWindowA(0, "Roblox"), &ClientRect);
            if (ScreenPos.x < 0 || ScreenPos.y < 0 || ScreenPos.x > ClientRect.right || ScreenPos.y > ClientRect.bottom) continue;

            float Dist2D = sqrtf((ScreenPos.x - CursorPos.x) * (ScreenPos.x - CursorPos.x) + (ScreenPos.y - CursorPos.y) * (ScreenPos.y - CursorPos.y));

            if (Globals::Aimbot::useFov && Dist2D > Globals::Aimbot::FovSize) continue;

            if (Dist2D < ClosestDistance) {
                ClosestDistance = Dist2D;
                AimPositionW = BonePos;
                AimPositionS = { ScreenPos.x, ScreenPos.y };
                BestName = Plr.Name;
                BestCharacterAddr = Plr.Character.Address;
                TargetFound = true;
            }
        }

        if (TargetFound) {
            CurrentLockedName = BestName;
            Globals::Aimbot::AimTarget = SDK::Instance(BestCharacterAddr);
            if (Globals::Aimbot::AimbotSticky && !IsPersisting) {
                PersistenceName = BestName;
                IsPersisting = true;
            }
        }
    }

    void UpdateAimbot() {
        if (!TargetFound) return;

        if (Globals::Aimbot::Aimbot_type == 1) {
            SDK::Datamodel Dm(Globals::Datamodel.Address);
            SDK::Instance WorkspaceInst = Dm.Find_First_Child_Of_Class("Workspace");
            SDK::Instance CameraInst = WorkspaceInst.Find_First_Child("Camera");
            if (!CameraInst.Address) return;

            SDK::Camera Cam(CameraInst.Address);
            auto CamPos = Cam.Get_CameraPos();
            auto TargetPos = AimPositionW;

            TargetPos.x += Globals::Aimbot::ShakeX;
            TargetPos.y += Globals::Aimbot::ShakeY;
            TargetPos.z += Globals::Aimbot::ShakeY;

            if (Globals::Aimbot::Shake) {
                TargetPos.x += ((float)rand() / RAND_MAX * 2 - 1) * Globals::Aimbot::ShakeX;
                TargetPos.y += ((float)rand() / RAND_MAX * 2 - 1) * Globals::Aimbot::ShakeY;
                TargetPos.z += ((float)rand() / RAND_MAX * 2 - 1) * Globals::Aimbot::ShakeZ;
            }

            float SmoothFactor = Globals::Aimbot::Camera::Smoothing_X / 100.f;
            SmoothFactor = std::pow(SmoothFactor, 1.2f);

            SDK::Matrix3 TargetMatrix = LookAtToMatrix(CamPos, TargetPos);
            SDK::Matrix3 CurrentMatrix = Cam.Get_CameraRot();
            SDK::Matrix3 FinalMatrix = LerpMatrix3(CurrentMatrix, TargetMatrix, 1.f - SmoothFactor);

            Cam.Set_CameraRot(FinalMatrix);
        }
        else {
            POINT CursorPos;
            if (!GetCursorPos(&CursorPos)) return;
            ScreenToClient(FindWindowA(0, "Roblox"), &CursorPos);

            float Sensitivity = Globals::Aimbot::Mouse::Mouse_Sensitivty;
            if (Globals::Aimbot::Mouse::Smoothing_X > 0) {
                float SmoothVal = Globals::Aimbot::Mouse::Smoothing_X;
                if (SmoothVal < 1.f) SmoothVal = 1.f;
                if (SmoothVal > 100.f) SmoothVal = 100.f;
                Sensitivity /= SmoothVal;
            }

            float MoveX = (AimPositionS.x - CursorPos.x) * Sensitivity;
            float MoveY = (AimPositionS.y - CursorPos.y) * Sensitivity;

            if (Globals::Aimbot::Shake) {
                MoveX += ((float)rand() / RAND_MAX * 2 - 1) * Globals::Aimbot::ShakeX;
                MoveY += ((float)rand() / RAND_MAX * 2 - 1) * Globals::Aimbot::ShakeY;
            }

            if (MoveX < -100.f) MoveX = -100.f;
            if (MoveX > 100.f) MoveX = 100.f;
            if (MoveY < -100.f) MoveY = -100.f;
            if (MoveY > 100.f) MoveY = 100.f;

            if (abs(MoveX) >= 1.f || abs(MoveY) >= 1.f) {
                INPUT Input = {};
                Input.type = INPUT_MOUSE;
                Input.mi.dx = (LONG)MoveX;
                Input.mi.dy = (LONG)MoveY;
                Input.mi.dwFlags = MOUSEEVENTF_MOVE;
                SendInput(1, &Input, sizeof(INPUT));
            }
        }
    }

    void RunService() {
        std::thread([]() {

            bool Toggled = false;
            bool LastPressed = false;

            while (true) {

                if (Globals::Aimbot::Enabled) {

                    int Vk = ImGuiKeyToVK(Globals::Aimbot::Aimbot_Key);
                    if (!Vk) { Sleep(1); continue; }

                    bool Pressed = (GetAsyncKeyState(Vk) & 0x8000) != 0;

                    if (Globals::Aimbot::Aimbot_Mode == ImKeyBindMode_Toggle) {

                        if (Pressed && !LastPressed)
                            Toggled = !Toggled;

                        if (Toggled) {
                            AcquireTarget();
                            UpdateAimbot();
                        }
                        else {
                            CurrentLockedName = "";
                            IsPersisting = false;
                            PersistenceName = "";
                            Globals::Aimbot::AimTarget = SDK::Instance(0);
                        }
                    }
                    else {

                        if (Pressed) {
                            AcquireTarget();
                            UpdateAimbot();
                        }
                        else {
                            CurrentLockedName = "";
                            IsPersisting = false;
                            PersistenceName = "";
                            Globals::Aimbot::AimTarget = SDK::Instance(0);
                        }
                    }

                    LastPressed = Pressed;
                }

                Sleep(1);
            }

            }).detach();
    }

}
#include <iostream>
#include "visuals.h"
#include <Engine/Engine.h>
#include <cfloat>
#include <Clipper2Lib/include/clipper2/clipper.h>
#include <imgui/imgui.h>
#include <globals.hxx>
#include <Windows.h>
#include <algorithm>
#include <ImGui/imgui_internal.h>
#include "../../../Graphics/Graphics.h"

#define IMGUI_DEFINE_MATH_OPERATORS

#define FlotationDevice(c) ImGui::ColorConvertFloat4ToU32(ImVec4(c[0], c[1], c[2], c[3]))

static void Outline(const ImVec2& Pos, const char* Text, const float Col[4]) {

    if (!Text || !*Text)
        return;

    ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    ImDrawList* Draw = ImGui::GetBackgroundDrawList();
    Draw->Flags &= ~ImDrawListFlags_AntiAliasedLines;

    ImGui::PushFont(Tahoma_BoldXP);

    const ImVec2 Position(std::roundf(Pos.x), std::roundf(Pos.y));

    const ImU32 Col1 = ImGui::ColorConvertFloat4ToU32(ImVec4(Col[0], Col[1], Col[2], Col[3]));
    const ImU32 Col2 = IM_COL32(0, 0, 0, 255);

    static constexpr ImVec2 Offsets[8] = {
        {-1.f,  0.f}, {1.f,  0.f},
        { 0.f, -1.f}, {0.f,  1.f},
        {-1.f, -1.f}, {1.f, -1.f},
        {-1.f,  1.f}, {1.f,  1.f}
    };

    const float Font_Size = ImGui::GetFontSize();

    for (const ImVec2& o : Offsets) {

        Draw->AddText(nullptr, Font_Size, Position + o, Col2, Text);
    }

    Draw->AddText(nullptr, Font_Size, Position, Col1, Text);

    ImGui::PopFont();
}

static const SDK::Vector3 Corners[8] = {

    {-1,-1,-1}, {1,-1,-1}, {-1,1,-1}, {1,1,-1},
    {-1,-1, 1}, {1,-1, 1}, {-1,1, 1}, {1,1, 1}
};

namespace Visuals {


    std::vector<const SDK::Instance*> Get_Bones(const SDK::Player& Player) {

        std::vector<const SDK::Instance*> Parts;

        const bool R15 = Player.UpperTorso.Address && Player.LowerTorso.Address;
        const bool R6 = Player.Torso.Address;

        if (R15) {

            if (Player.Head.Address) Parts.push_back(&Player.Head);

            if (Player.UpperTorso.Address) Parts.push_back(&Player.UpperTorso);
            if (Player.LowerTorso.Address) Parts.push_back(&Player.LowerTorso);

            if (Player.LeftUpperArm.Address) Parts.push_back(&Player.LeftUpperArm);
            if (Player.LeftLowerArm.Address) Parts.push_back(&Player.LeftLowerArm);
            if (Player.LeftHand.Address)     Parts.push_back(&Player.LeftHand);

            if (Player.RightUpperArm.Address) Parts.push_back(&Player.RightUpperArm);
            if (Player.RightLowerArm.Address) Parts.push_back(&Player.RightLowerArm);
            if (Player.RightHand.Address)     Parts.push_back(&Player.RightHand);

            if (Player.LeftUpperLeg.Address) Parts.push_back(&Player.LeftUpperLeg);
            if (Player.LeftLowerLeg.Address) Parts.push_back(&Player.LeftLowerLeg);
            if (Player.LeftFoot.Address)     Parts.push_back(&Player.LeftFoot);

            if (Player.RightUpperLeg.Address) Parts.push_back(&Player.RightUpperLeg);
            if (Player.RightLowerLeg.Address) Parts.push_back(&Player.RightLowerLeg);
            if (Player.RightFoot.Address)     Parts.push_back(&Player.RightFoot);
        }
        else if (R6) {

            if (Player.Head.Address)  Parts.push_back(&Player.Head);
            if (Player.Torso.Address) Parts.push_back(&Player.Torso);

            if (Player.LeftArm.Address)  Parts.push_back(&Player.LeftArm);
            if (Player.RightArm.Address) Parts.push_back(&Player.RightArm);

            if (Player.LeftLeg.Address)  Parts.push_back(&Player.LeftLeg);
            if (Player.RightLeg.Address) Parts.push_back(&Player.RightLeg);
        }
        else {

            for (const auto& Bone : Player.Bones) {

                if (Bone.Address)
                    Parts.push_back(&Bone);
            }

            if (Parts.empty()) {

                if (Player.HumanoidRootPart.Address) Parts.push_back(&Player.HumanoidRootPart);
                if (Player.Head.Address)             Parts.push_back(&Player.Head);
                if (Player.Torso.Address)            Parts.push_back(&Player.Torso);
                if (Player.UpperTorso.Address)       Parts.push_back(&Player.UpperTorso);
                if (Player.LowerTorso.Address)       Parts.push_back(&Player.LowerTorso);
            }
        }
        return Parts;
    }

    void RunService()
    {
        if (Globals::VisualEngine.Address == 0 || Globals::Datamodel.Address == 0)
            return;

        ImDrawList* Draw = ImGui::GetBackgroundDrawList();
        Draw->Flags &= ~ImDrawListFlags_AntiAliasedLines;

        const auto Snapshot = Globals::Player_Cache;
        const auto& Local = Globals::LocalPlayer;

        for (auto& Player : Snapshot)
        {
            if (!Globals::Visuals::Enabled || !Player.Character.Address)
                continue;

            if (Globals::Visuals::Render_Distance > 0.f && Player.Distance > Globals::Visuals::Render_Distance)
                continue;

            SDK::Part Head(Player.Head.Address);
            if (!Head.Address) continue;

            auto Head_W2S = Globals::VisualEngine.World_To_Screen(Head.Get_Primitive().Get_Position());
            if (Head_W2S.x <= 0.f || Head_W2S.y <= 0.f)
                continue;

            float Left = FLT_MAX, Top = FLT_MAX, Right = -FLT_MAX, Bottom = -FLT_MAX;
            bool Valid = false;

            auto Bones = Visuals::Get_Bones(Player);
            if (Bones.empty()) continue;

            for (auto* Inst : Bones) {
                if (!Inst || !Inst->Address) continue;

                const auto Part = SDK::Part(Inst->Address);
                const auto Primitive = Part.Get_Primitive();

                SDK::Vector3 Size = Primitive.Get_Size();
                const auto Position = Primitive.Get_Position();
                const auto Rotation = Primitive.Get_Rotation();

                if (Globals::GameID == 292439477)
                {
                    std::string Name = Part.Name();

                    if (Name.find("Other_") != std::string::npos) {
                        Size = { 1.f, 2.f, 1.f };
                    }
                    else if (Name == "Head") {
                        Size = { 1.f, 1.f, 1.f };
                    }
                    else if (Name == "Torso") {
                        Size = { 2.f, 2.f, 1.f };
                    }
                }

                if (Size.x == 0.f && Size.y == 0.f && Size.z == 0.f)
                    continue;

                for (const auto& LocalCorners : Corners) {
                    SDK::Vector3 Offset{
                        LocalCorners.x * Size.x * 0.5f,
                        LocalCorners.y * Size.y * 0.5f,
                        LocalCorners.z * Size.z * 0.5f
                    };

                    SDK::Vector3 World = Position + Rotation * Offset;
                    auto W2S = Globals::VisualEngine.World_To_Screen(World);

                    if (W2S.x < 0.f || W2S.y < 0.f) continue;

                    Valid = true;
                    Left = min(Left, W2S.x);
                    Top = min(Top, W2S.y);
                    Right = max(Right, W2S.x);
                    Bottom = max(Bottom, W2S.y);
                }
            }

            if (!Valid || Left >= Right || Top >= Bottom) continue;

            ImVec2 Pos(Left - 1.f, Top - 1.f);
            ImVec2 Size((Right - Left) + 2.f, (Bottom - Top) + 2.f);

            if (Globals::Visuals::Box)
            {
                if (Globals::Visuals::Box_Type == 0) {

                    Pos.x = std::round(Pos.x);
                    Pos.y = std::round(Pos.y);
                    Size.x = std::round(Size.x);
                    Size.y = std::round(Size.y);

                    ImVec2 Min = Pos;
                    ImVec2 Max = ImVec2(Pos.x + Size.x, Pos.y + Size.y);

                    if (Globals::Visuals::Box_Fill) {

                        if (Globals::Visuals::Box_Fill_Gradient) {

                            float time = (Globals::Visuals::Box_Fill_Gradient_Rotate) ? (float)ImGui::GetTime() * Globals::Visuals::BoxFillSpeed : 0.0f;
                            ImU32 col1 = FlotationDevice(Globals::Visuals::Colors::BoxFill_Top);
                            ImU32 col2 = FlotationDevice(Globals::Visuals::Colors::BoxFill_Bottom);

                            float s = sinf(time);
                            float c = cosf(time);

                            ImU32 c_tl, c_tr, c_br, c_bl;

                            if (Globals::Visuals::Box_Fill_Type == 0) {
                                c_tl = c_bl = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (s + 1.0f) * 0.5f));
                                c_tr = c_br = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (c + 1.0f) * 0.5f));
                            }
                            else if (Globals::Visuals::Box_Fill_Type == 1) {
                                c_tl = c_tr = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (s + 1.0f) * 0.5f));
                                c_bl = c_br = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (c + 1.0f) * 0.5f));
                            }
                            else {
                                c_tl = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (s + 1.0f) * 0.5f));
                                c_tr = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (c + 1.0f) * 0.5f));
                                c_br = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (-s + 1.0f) * 0.5f));
                                c_bl = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (-c + 1.0f) * 0.5f));
                            }

                            Draw->AddRectFilledMultiColor(ImVec2(Min.x + 1.f, Min.y + 1.f), ImVec2(Max.x - 1.f, Max.y - 1.f), c_tl, c_tr, c_br, c_bl);

                        }
                        else {
                            Draw->AddRectFilled(ImVec2(Min.x + 1.f, Min.y + 1.f), ImVec2(Max.x - 1.f, Max.y - 1.f), FlotationDevice(Globals::Visuals::Colors::BoxFill_Top));
                        }
                    }

                    Draw->AddRect(Min, Max, IM_COL32(0, 0, 0, 255), 0.f, 0, 1.f);
                    Draw->AddRect(ImVec2(Min.x - 1.f, Min.y - 1.f), ImVec2(Max.x + 1.f, Max.y + 1.f), FlotationDevice(Globals::Visuals::Colors::Box), 0.f, 0, 1.f);
                    Draw->AddRect(ImVec2(Min.x - 2.f, Min.y - 2.f), ImVec2(Max.x + 2.f, Max.y + 2.f), IM_COL32(0, 0, 0, 255), 0.f, 0, 1.f);
                }

                else if (Globals::Visuals::Box_Type == 1) {

                    float X1 = Pos.x - 1.f;
                    float Y1 = Pos.y - 1.f;
                    float X2 = Pos.x + Size.x + 1.f;
                    float Y2 = Pos.y + Size.y + 1.f;

                    float Box_Width = X2 - X1;
                    float Box_Height = Y2 - Y1;

                    float Length = min(Box_Width, Box_Height) * 0.25f;
                    Length = min(Length, 50.0f);
                    Length = min(Length, (min(Box_Width, Box_Height) * 0.5f) - 1.0f);

                    float X1_Length = X1 + Length;
                    float Y1_Length = Y1 + Length;
                    float X2_Length = X2 - Length;
                    float Y2_Length = Y2 - Length;

                    if (Globals::Visuals::Box_Fill) {

                        if (Globals::Visuals::Box_Fill_Gradient) {

                            float time = (Globals::Visuals::Box_Fill_Gradient_Rotate) ? (float)ImGui::GetTime() * Globals::Visuals::BoxFillSpeed : 0.0f;
                            ImU32 col1 = FlotationDevice(Globals::Visuals::Colors::BoxFill_Top);
                            ImU32 col2 = FlotationDevice(Globals::Visuals::Colors::BoxFill_Bottom);

                            float s = sinf(time);
                            float c = cosf(time);

                            ImU32 c_tl, c_tr, c_br, c_bl;

                            if (Globals::Visuals::Box_Fill_Type == 0) {
                                c_tl = c_bl = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (s + 1.0f) * 0.5f));
                                c_tr = c_br = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (c + 1.0f) * 0.5f));
                            }
                            else if (Globals::Visuals::Box_Fill_Type == 1) {
                                c_tl = c_tr = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (s + 1.0f) * 0.5f));
                                c_bl = c_br = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (c + 1.0f) * 0.5f));
                            }
                            else {
                                c_tl = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (s + 1.0f) * 0.5f));
                                c_tr = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (c + 1.0f) * 0.5f));
                                c_br = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (-s + 1.0f) * 0.5f));
                                c_bl = ImGui::ColorConvertFloat4ToU32(ImLerp(ImGui::ColorConvertU32ToFloat4(col1), ImGui::ColorConvertU32ToFloat4(col2), (-c + 1.0f) * 0.5f));
                            }

                            Draw->AddRectFilledMultiColor(ImVec2(X1 + 2.f, Y1 + 2.f), ImVec2(X2 - 2.f, Y2 - 2.f), c_tl, c_tr, c_br, c_bl);

                        }
                        else {
                            Draw->AddRectFilled(ImVec2(X1 + 2.f, Y1 + 2.f), ImVec2(X2 - 2.f, Y2 - 2.f), FlotationDevice(Globals::Visuals::Colors::BoxFill_Top));
                        }
                    }

                    Draw->AddRectFilled(ImVec2(X1 - 1.f, Y1 - 1.f), ImVec2(X1_Length + 1.f, Y1 + 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X1 - 1.f, Y1 - 1.f), ImVec2(X1 + 1.f, Y1_Length + 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X2_Length - 1.f, Y1 - 1.f), ImVec2(X2 + 1.f, Y1 + 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X2 - 1.f, Y1 - 1.f), ImVec2(X2 + 1.f, Y1_Length + 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X1 - 1.f, Y2 - 1.f), ImVec2(X1_Length + 1.f, Y2 + 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X1 - 1.f, Y2_Length - 1.f), ImVec2(X1 + 1.f, Y2 + 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X2_Length - 1.f, Y2 - 1.f), ImVec2(X2 + 1.f, Y2 + 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X2 - 1.f, Y2_Length - 1.f), ImVec2(X2 + 1.f, Y2 + 1.f), IM_COL32(0, 0, 0, 255));

                    Draw->AddRectFilled(ImVec2(X1 + 1.f, Y1 + 1.f), ImVec2(X1_Length + 1.f, Y1 + 2.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X1 + 1.f, Y1 + 1.f), ImVec2(X1 + 2.f, Y1_Length + 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X2_Length - 1.f, Y1 + 1.f), ImVec2(X2 - 1.f, Y1 + 2.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X2 - 2.f, Y1 + 1.f), ImVec2(X2 - 1.f, Y1_Length + 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X1 + 1.f, Y2 - 2.f), ImVec2(X1_Length + 1.f, Y2 - 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X1 + 1.f, Y2_Length - 1.f), ImVec2(X1 + 2.f, Y2 - 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X2_Length - 1.f, Y2 - 2.f), ImVec2(X2 - 1.f, Y2 - 1.f), IM_COL32(0, 0, 0, 255));
                    Draw->AddRectFilled(ImVec2(X2 - 2.f, Y2_Length - 1.f), ImVec2(X2 - 1.f, Y2 - 1.f), IM_COL32(0, 0, 0, 255));

                    Draw->AddRectFilled(ImVec2(X1, Y1), ImVec2(X1_Length, Y1 + 1.f), FlotationDevice(Globals::Visuals::Colors::Box));
                    Draw->AddRectFilled(ImVec2(X1, Y1), ImVec2(X1 + 1.f, Y1_Length), FlotationDevice(Globals::Visuals::Colors::Box));
                    Draw->AddRectFilled(ImVec2(X2_Length, Y1), ImVec2(X2, Y1 + 1.f), FlotationDevice(Globals::Visuals::Colors::Box));
                    Draw->AddRectFilled(ImVec2(X2 - 1.f, Y1), ImVec2(X2, Y1_Length), FlotationDevice(Globals::Visuals::Colors::Box));
                    Draw->AddRectFilled(ImVec2(X1, Y2 - 1.f), ImVec2(X1_Length, Y2), FlotationDevice(Globals::Visuals::Colors::Box));
                    Draw->AddRectFilled(ImVec2(X1, Y2_Length), ImVec2(X1 + 1.f, Y2), FlotationDevice(Globals::Visuals::Colors::Box));
                    Draw->AddRectFilled(ImVec2(X2_Length, Y2 - 1.f), ImVec2(X2, Y2), FlotationDevice(Globals::Visuals::Colors::Box));
                    Draw->AddRectFilled(ImVec2(X2 - 1.f, Y2_Length), ImVec2(X2, Y2), FlotationDevice(Globals::Visuals::Colors::Box));
                }
            }

            if (Globals::Visuals::Healthbar) {

                if (Globals::Visuals::Healthbar_Type == 0) {

                    float Ratio = (Player.MaxHealth > 0.f) ? Player.Health / Player.MaxHealth : 0.f;
                    Ratio = ImClamp(Ratio, 0.f, 1.f);

                    float Gap = Globals::Visuals::Gap;
                    float Thickness = Globals::Visuals::Thickness;

                    float X_Min = Pos.x - Gap - Thickness - 3.f;
                    float Y_Min = Pos.y - 1.f;
                    float Y_Max = Pos.y + Size.y + 1.f;

                    ImVec2 Bg_Min(X_Min - 1.f, Y_Min - 1.f);
                    ImVec2 Bg_Max(X_Min + Thickness + 1.f, Y_Max + 1.f);
                    Draw->AddRectFilled(Bg_Min, Bg_Max, IM_COL32(0, 0, 0, 255));

                    ImVec2 Empty_Min(X_Min, Y_Min);
                    ImVec2 Empty_max(X_Min + Thickness, Y_Max);
                    Draw->AddRectFilled(Empty_Min, Empty_max, IM_COL32(130, 130, 130, 150));

                    float Height = (Y_Max - Y_Min) * Ratio;
                    ImVec2 Fg_Min(X_Min, Y_Max - Height);
                    ImVec2 Fg_max(X_Min + Thickness, Y_Max);
                    Draw->AddRectFilled(Fg_Min, Fg_max, FlotationDevice(Globals::Visuals::Colors::Healthbar));
                }

                else if (Globals::Visuals::Healthbar_Type == 1) {

                    float Ratio = (Player.MaxHealth > 0.f) ? Player.Health / Player.MaxHealth : 0.f;
                    Ratio = ImClamp(Ratio, 0.f, 1.f);

                    float Gap = Globals::Visuals::Gap;
                    float Thickness = Globals::Visuals::Thickness;

                    float X_Min = Pos.x - Gap - Thickness - 3.f;
                    float Y_Min = Pos.y - 1.f;
                    float Y_Max = Pos.y + Size.y + 1.f;

                    Draw->AddRectFilled(ImVec2(X_Min - 1, Y_Min - 1), ImVec2(X_Min + Thickness + 1, Y_Max + 1), IM_COL32(0, 0, 0, 255));

                    Draw->AddRectFilled(ImVec2(X_Min, Y_Min), ImVec2(X_Min + Thickness, Y_Max), IM_COL32(130, 130, 130, 150));

                    ImU32 TopColor = IM_COL32((int)(Globals::Visuals::Colors::Healthbar_Top[0] * 255), (int)(Globals::Visuals::Colors::Healthbar_Top[1] * 255), (int)(Globals::Visuals::Colors::Healthbar_Top[2] * 255), (int)(Globals::Visuals::Colors::Healthbar_Top[3] * 255));
                    ImU32 MiddleColor = IM_COL32((int)(Globals::Visuals::Colors::Healthbar_Middle[0] * 255), (int)(Globals::Visuals::Colors::Healthbar_Middle[1] * 255), (int)(Globals::Visuals::Colors::Healthbar_Middle[2] * 255), (int)(Globals::Visuals::Colors::Healthbar_Middle[3] * 255));
                    ImU32 BottomColor = IM_COL32((int)(Globals::Visuals::Colors::Healthbar_Bottom[0] * 255), (int)(Globals::Visuals::Colors::Healthbar_Bottom[1] * 255), (int)(Globals::Visuals::Colors::Healthbar_Bottom[2] * 255), (int)(Globals::Visuals::Colors::Healthbar_Bottom[3] * 255));

                    float FullHeight = Y_Max - Y_Min;
                    float HealthHeight = FullHeight * Ratio;

                    float FillMinY = Y_Max - HealthHeight;
                    float MidY = Y_Min + FullHeight * 0.5f;

                    if (FillMinY < MidY) {

                        Draw->AddRectFilledMultiColor(ImVec2(X_Min, ImMax(FillMinY, Y_Min)), ImVec2(X_Min + Thickness, MidY), TopColor, TopColor, MiddleColor, MiddleColor);
                    }

                    Draw->AddRectFilledMultiColor(ImVec2(X_Min, ImMax(FillMinY, MidY)), ImVec2(X_Min + Thickness, Y_Max), MiddleColor, MiddleColor, BottomColor, BottomColor);
                }
            }

            if (Globals::Visuals::Health)
            {
                std::string HealthStr = "[" + std::to_string(static_cast<int>(Player.Health)) + "]";

                float X_Text = Pos.x - 6.0f;
                if (Globals::Visuals::Healthbar) {
                    X_Text -= Globals::Visuals::Thickness + Globals::Visuals::Gap;
                }


                float Y_text = Pos.y - 3.0f;

                ImGui::PushFont(Tahoma_BoldXP);
                ImVec2 Text_Size = ImGui::CalcTextSize(HealthStr.c_str());
                ImGui::PopFont();

                ImVec2 Text_Pos(X_Text - Text_Size.x, Y_text);

                Outline(Text_Pos, HealthStr.c_str(), Globals::Visuals::Colors::Health);
            }

            if (Globals::Visuals::Name) {

                if (Globals::Visuals::Name_Type == 0) {

                    ImGui::PushFont(Tahoma_BoldXP);
                    ImVec2 Text_Size = ImGui::CalcTextSize(Player.Name.c_str());
                    ImGui::PopFont();

                    ImVec2 Text_Position(Pos.x + (Size.x * 0.5f) - (Text_Size.x * 0.5f), Pos.y - Text_Size.y - 3.f);

                    Outline(Text_Position, Player.Name.c_str(), Globals::Visuals::Colors::Name);
                }

                else if (Globals::Visuals::Name_Type == 1) {

                    ImGui::PushFont(Tahoma_BoldXP);
                    ImVec2 Text_Size = ImGui::CalcTextSize(Player.Display_Name.c_str());
                    ImGui::PopFont();

                    ImVec2 Text_Position(Pos.x + (Size.x * 0.5f) - (Text_Size.x * 0.5f), Pos.y - Text_Size.y - 3.f);

                    Outline(Text_Position, Player.Display_Name.c_str(), Globals::Visuals::Colors::Name);
                }

                else if (Globals::Visuals::Name_Type == 2) {

                    std::string Text = Player.Name + " [" + Player.Display_Name + "]";

                    ImGui::PushFont(Tahoma_BoldXP);
                    ImVec2 Text_Size = ImGui::CalcTextSize(Text.c_str());
                    float NameW = ImGui::CalcTextSize((Player.Name + " ").c_str()).x;
                    float BracketW = ImGui::CalcTextSize("[").x;
                    float DisplayW = ImGui::CalcTextSize(Player.Display_Name.c_str()).x;
                    ImGui::PopFont();

                    ImVec2 Position(Pos.x + (Size.x * 0.5f) - (Text_Size.x * 0.5f), Pos.y - Text_Size.y - 3.f);

                    Outline(Position, Player.Name.c_str(), Globals::Visuals::Colors::Name);
                    Outline(ImVec2(Position.x + NameW, Position.y - 2.f), "[", Globals::Visuals::Colors::Name);

                    static float white[4] = { 1.f, 1.f, 1.f, 1.f };
                    Outline(ImVec2(Position.x + NameW + BracketW, Position.y - 1.f), Player.Display_Name.c_str(), white);

                    Outline(ImVec2(Position.x + NameW + BracketW + DisplayW, Position.y - 2.f), "]", Globals::Visuals::Colors::Name);
                }
            }

            if (Globals::Visuals::Distance) {

                ImGui::PushFont(Tahoma_BoldXP);

                char Buffer[16];
                snprintf(Buffer, sizeof(Buffer), "[%dm]", static_cast<int>(Player.Distance));

                ImVec2 Text_Size = ImGui::CalcTextSize(Buffer);
                ImGui::PopFont();

                ImVec2 Text_Position(Pos.x + (Size.x * 0.5f) - (Text_Size.x * 0.5f), Pos.y + Size.y + 3.0f);

                Outline(Text_Position, Buffer, Globals::Visuals::Colors::Distance);
            }

            if (Globals::Visuals::Rig_Type)
            {
                ImGui::PushFont(Tahoma_BoldXP);

                const char* Rig_Type = nullptr;

                if (Player.Rig_Type == 1)
                    Rig_Type = "[R15]";
                else if (Player.Rig_Type == 0)
                    Rig_Type = "[R6]";
                else
                {
                    ImGui::PopFont();
                    return;
                }

                ImVec2 Text_Size = ImGui::CalcTextSize(Rig_Type);

                ImVec2 Text_Position(std::round(Pos.x + Size.x + 5.0f), std::round(Pos.y - Text_Size.y + 10.0f));

                Outline(Text_Position, Rig_Type, Globals::Visuals::Colors::Rig_Type);

                ImGui::PopFont();
            }

            if (Globals::Visuals::Tool)
            {
                std::string Cl_Name;

                const std::string& Tool_Name = Player.Tool_Name;

                if (Tool_Name.empty()) {

                    Cl_Name = "[None]";
                }

                else {

                    Cl_Name.reserve(Tool_Name.size());

                    for (char c : Tool_Name) {

                        if (c != '[' && c != ']')
                            Cl_Name.push_back(c);
                    }

                    if (!Cl_Name.empty()) {

                        Cl_Name.insert(Cl_Name.begin(), '[');
                        Cl_Name.push_back(']');
                    }
                }

                ImGui::PushFont(Tahoma_BoldXP);
                ImVec2 Text_Size = ImGui::CalcTextSize(Cl_Name.c_str());
                ImGui::PopFont();

                float Offset = Globals::Visuals::Distance ? 18.0f : 3.0f;

                ImVec2 Text_Position(std::round(Pos.x + (Size.x * 0.5f) - (Text_Size.x * 0.5f)), std::round(Pos.y + Size.y + Offset));

                Outline(Text_Position, Cl_Name.c_str(), Globals::Visuals::Colors::Tool);
            }

            if (Globals::Visuals::Chams) {

                ImDrawList* Draw = ImGui::GetBackgroundDrawList();
                Draw->Flags &= ~ImDrawListFlags_AntiAliasedLines;

                auto ProjectPart = [&](const SDK::Part& Part) -> std::vector<ImVec2> {
                    std::vector<ImVec2> Projected;
                    if (!Part.Address) return Projected;

                    const auto Prim = Part.Get_Primitive();
                    const auto Size = Prim.Get_Size();
                    const auto Pos = Prim.Get_Position();
                    const auto Rot = Prim.Get_Rotation();

                    Projected.reserve(8);
                    for (int i = 0; i < 8; ++i) {
                        const auto& Corner = Corners[i];
                        SDK::Vector3 World = Pos + Rot * SDK::Vector3{ Corner.x * Size.x * 0.5f, Corner.y * Size.y * 0.5f, Corner.z * Size.z * 0.5f };
                        SDK::Vector2 Screen = Globals::VisualEngine.World_To_Screen(World);
                        if (Screen.x >= 0.f && Screen.y >= 0.f)
                            Projected.emplace_back(Screen.x, Screen.y);
                    }

                    if (Projected.size() < 3) return {};

                    std::sort(Projected.begin(), Projected.end(), [](const ImVec2& A, const ImVec2& B) {
                        return A.x < B.x || (A.x == B.x && A.y < B.y);
                        });

                    std::vector<ImVec2> Hull;
                    Hull.reserve(Projected.size() * 2);

                    auto Cross = [](const ImVec2& O, const ImVec2& A, const ImVec2& B) {
                        return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
                        };

                    for (auto& P : Projected) {
                        while (Hull.size() >= 2 && Cross(Hull[Hull.size() - 2], Hull.back(), P) <= 0)
                            Hull.pop_back();
                        Hull.push_back(P);
                    }

                    size_t T = Hull.size() + 1;
                    for (int i = (int)Projected.size() - 1; i >= 0; --i) {
                        auto& P = Projected[i];
                        while (Hull.size() >= T && Cross(Hull[Hull.size() - 2], Hull.back(), P) <= 0)
                            Hull.pop_back();
                        Hull.push_back(P);
                    }

                    Hull.pop_back();
                    return Hull;
                    };

                Clipper2Lib::Paths64 AllParts;
                for (auto* Inst : Visuals::Get_Bones(Player)) {
                    const SDK::Part Part(Inst->Address);
                    auto Hull = ProjectPart(Part);
                    if (Hull.size() < 3) continue;

                    Clipper2Lib::Path64 Path;
                    Path.reserve(Hull.size());
                    for (auto& Pt : Hull) Path.emplace_back(static_cast<int64_t>(Pt.x * 1000.0), static_cast<int64_t>(Pt.y * 1000.0));
                    AllParts.push_back(std::move(Path));
                }

                if (!AllParts.empty()) {
                    auto UnifiedSolution = Clipper2Lib::Union(AllParts, Clipper2Lib::FillRule::NonZero);

                    std::vector<std::vector<ImVec2>> AllPolys;
                    AllPolys.reserve(UnifiedSolution.size());
                    for (auto& Sp : UnifiedSolution) {
                        std::vector<ImVec2> Poly;
                        Poly.reserve(Sp.size());
                        for (auto& Pt : Sp) Poly.emplace_back(Pt.x / 1000.0f, Pt.y / 1000.0f);
                        if (Poly.size() >= 3) AllPolys.push_back(std::move(Poly));
                    }

                    ImU32 FillColor = ImGui::ColorConvertFloat4ToU32({ Globals::Visuals::Colors::Chams[0], Globals::Visuals::Colors::Chams[1], Globals::Visuals::Colors::Chams[2], Globals::Visuals::Colors::Chams[3] });
                    ImU32 OutlineColor = ImGui::ColorConvertFloat4ToU32({ Globals::Visuals::Colors::ChamsOutline[0], Globals::Visuals::Colors::ChamsOutline[1], Globals::Visuals::Colors::ChamsOutline[2], Globals::Visuals::Colors::ChamsOutline[3] });

                    if (Globals::Visuals::ChamsFade) {
                        float time = (float)ImGui::GetTime() * Globals::Visuals::ChamsFadeSpeed;
                        float t = (sinf(time) + 1.0f) * 0.5f;
                        ImVec4 c = ImGui::ColorConvertU32ToFloat4(FillColor);
                        c.w *= t;
                        FillColor = ImGui::ColorConvertFloat4ToU32(c);
                    }

                    for (auto& Poly : AllPolys)
                        Draw->AddConcavePolyFilled(Poly.data(), Poly.size(), FillColor);

                    for (auto& Poly : AllPolys)
                        Draw->AddPolyline(Poly.data(), Poly.size(), OutlineColor, true, 1.0f);
                }
            }

            if (Globals::Visuals::Skeleton) {

                const ImU32 SkelCol = FlotationDevice(Globals::Visuals::Colors::Skeleton);
                const ImU32 OutlineCol = IM_COL32(0, 0, 0, 255);
                const float Thickness = 1.0f;

                auto W2S = [&](const SDK::Vector3& WorldPos, ImVec2& Out) -> bool
                    {
                        auto ScreenPos = Globals::VisualEngine.World_To_Screen(WorldPos);
                        if (ScreenPos.x <= 0.f || ScreenPos.y <= 0.f) return false;
                        Out.x = std::roundf(ScreenPos.x);
                        Out.y = std::roundf(ScreenPos.y);
                        return true;
                    };

                auto DrawPoly = [&](const ImVec2* Points, int Count)
                    {
                        if (Count < 2) return;
                        Draw->AddPolyline(Points, Count, OutlineCol, false, Thickness + 2.f);
                        Draw->AddPolyline(Points, Count, SkelCol, false, Thickness);
                    };

                if (Player.UpperTorso.Address && Player.LowerTorso.Address)
                {

                    auto ProcessR6Chain = [&](const SDK::Instance* Instances, int Count)
                        {
                            ImVec2 ScreenPoints[4];
                            int ValidCount = 0;

                            for (int i = 0; i < Count; ++i)
                            {
                                if (!Instances[i].Address)
                                {
                                    DrawPoly(ScreenPoints, ValidCount);
                                    ValidCount = 0;
                                    continue;
                                }

                                SDK::Part Part(Instances[i].Address);
                                if (!Part.Address)
                                {
                                    DrawPoly(ScreenPoints, ValidCount);
                                    ValidCount = 0;
                                    continue;
                                }

                                ImVec2 ScreenPos;
                                if (!W2S(Part.Get_Primitive().Get_Position(), ScreenPos))
                                {
                                    DrawPoly(ScreenPoints, ValidCount);
                                    ValidCount = 0;
                                    continue;
                                }

                                ScreenPoints[ValidCount++] = ScreenPos;
                            }
                            DrawPoly(ScreenPoints, ValidCount);
                        };

                    const SDK::Instance Spine[] = { Player.Head, Player.UpperTorso, Player.LowerTorso };
                    ProcessR6Chain(Spine, 3);

                    const SDK::Instance LeftArm[] = { Player.UpperTorso, Player.LeftUpperArm, Player.LeftLowerArm, Player.LeftHand };
                    ProcessR6Chain(LeftArm, 4);

                    const SDK::Instance RightArm[] = { Player.UpperTorso, Player.RightUpperArm, Player.RightLowerArm, Player.RightHand };
                    ProcessR6Chain(RightArm, 4);

                    const SDK::Instance LeftLeg[] = { Player.LowerTorso, Player.LeftUpperLeg, Player.LeftLowerLeg, Player.LeftFoot };
                    ProcessR6Chain(LeftLeg, 4);

                    const SDK::Instance RightLeg[] = { Player.LowerTorso, Player.RightUpperLeg, Player.RightLowerLeg, Player.RightFoot };
                    ProcessR6Chain(RightLeg, 4);
                }

                else if (Player.Torso.Address && Player.Head.Address)
                {
                    SDK::Part TorsoPart(Player.Torso.Address);
                    SDK::Part HeadPart(Player.Head.Address);
                    const auto& TorsoPrim = TorsoPart.Get_Primitive();
                    const auto& HeadPrim = HeadPart.Get_Primitive();

                    const SDK::Vector3 TorsoPos = TorsoPrim.Get_Position();
                    const SDK::Vector3 TorsoSize = TorsoPrim.Get_Size();
                    const auto TorsoRot = TorsoPrim.Get_Rotation();
                    const SDK::Vector3 HeadPos = HeadPrim.Get_Position();
                    const SDK::Vector3 HeadSize = HeadPrim.Get_Size();

                    const SDK::Vector3 ShoulderCenter = TorsoPos + TorsoRot * SDK::Vector3{ 0, TorsoSize.y * 0.2f, 0 };
                    const SDK::Vector3 HipCenter = TorsoPos - TorsoRot * SDK::Vector3{ 0, TorsoSize.y * 0.4f, 0 };
                    const SDK::Vector3 HeadBottom = HeadPos - SDK::Vector3{ 0, HeadSize.y * 0.5f, 0 };
                    const SDK::Vector3 ShoulderLeft = ShoulderCenter + TorsoRot * SDK::Vector3{ -TorsoSize.x * 0.5f, 0, 0 };
                    const SDK::Vector3 ShoulderRight = ShoulderCenter + TorsoRot * SDK::Vector3{ TorsoSize.x * 0.5f, 0, 0 };

                    auto ProcessR15Chain = [&](const SDK::Vector3* Points, int Count)
                        {
                            ImVec2 ScreenPoints[8];
                            int ValidCount = 0;
                            for (int i = 0; i < Count; ++i)
                            {
                                ImVec2 ScreenPos;
                                if (W2S(Points[i], ScreenPos))
                                    ScreenPoints[ValidCount++] = ScreenPos;
                            }
                            DrawPoly(ScreenPoints, ValidCount);
                        };

                    {
                        const SDK::Vector3 SpinePts[] = { HeadPos, HeadBottom, ShoulderCenter, HipCenter };
                        ProcessR15Chain(SpinePts, 4);
                    }

                    {
                        SDK::Vector3 ArmPts[4];
                        int Count = 0;
                        ArmPts[Count++] = ShoulderCenter;
                        ArmPts[Count++] = ShoulderLeft;

                        if (Player.LeftArm.Address)
                        {
                            SDK::Part Arm(Player.LeftArm.Address);
                            const auto& ArmPrim = Arm.Get_Primitive();
                            const auto& ArmRot = ArmPrim.Get_Rotation();
                            const SDK::Vector3 ArmPos = ArmPrim.Get_Position();
                            const SDK::Vector3 ArmSize = ArmPrim.Get_Size();

                            ArmPts[Count++] = ArmPos + ArmRot * SDK::Vector3{ 0, ArmSize.y * 0.2f, 0 };
                            ArmPts[Count++] = ArmPos - ArmRot * SDK::Vector3{ 0, ArmSize.y * 0.5f, 0 };
                        }
                        ProcessR15Chain(ArmPts, Count);
                    }

                    {
                        SDK::Vector3 ArmPts[4];
                        int Count = 0;
                        ArmPts[Count++] = ShoulderCenter;
                        ArmPts[Count++] = ShoulderRight;

                        if (Player.RightArm.Address)
                        {
                            SDK::Part Arm(Player.RightArm.Address);
                            const auto& ArmPrim = Arm.Get_Primitive();
                            const auto& ArmRot = ArmPrim.Get_Rotation();
                            const SDK::Vector3 ArmPos = ArmPrim.Get_Position();
                            const SDK::Vector3 ArmSize = ArmPrim.Get_Size();

                            ArmPts[Count++] = ArmPos + ArmRot * SDK::Vector3{ 0, ArmSize.y * 0.2f, 0 };
                            ArmPts[Count++] = ArmPos - ArmRot * SDK::Vector3{ 0, ArmSize.y * 0.5f, 0 };
                        }
                        ProcessR15Chain(ArmPts, Count);
                    }

                    {
                        SDK::Vector3 LegPts[3];
                        int Count = 0;
                        LegPts[Count++] = HipCenter;

                        if (Player.LeftLeg.Address)
                        {
                            SDK::Part Leg(Player.LeftLeg.Address);
                            const auto& LegPrim = Leg.Get_Primitive();
                            const auto& LegRot = LegPrim.Get_Rotation();
                            const SDK::Vector3 LegPos = LegPrim.Get_Position();
                            const SDK::Vector3 LegSize = LegPrim.Get_Size();

                            LegPts[Count++] = LegPos + LegRot * SDK::Vector3{ 0, LegSize.y * 0.5f, 0 };
                            LegPts[Count++] = LegPos - LegRot * SDK::Vector3{ 0, LegSize.y * 0.5f, 0 };
                        }
                        ProcessR15Chain(LegPts, Count);
                    }

                    {
                        SDK::Vector3 LegPts[3];
                        int Count = 0;
                        LegPts[Count++] = HipCenter;

                        if (Player.RightLeg.Address)
                        {
                            SDK::Part Leg(Player.RightLeg.Address);
                            const auto& LegPrim = Leg.Get_Primitive();
                            const auto& LegRot = LegPrim.Get_Rotation();
                            const SDK::Vector3 LegPos = LegPrim.Get_Position();
                            const SDK::Vector3 LegSize = LegPrim.Get_Size();

                            LegPts[Count++] = LegPos + LegRot * SDK::Vector3{ 0, LegSize.y * 0.5f, 0 };
                            LegPts[Count++] = LegPos - LegRot * SDK::Vector3{ 0, LegSize.y * 0.5f, 0 };
                        }
                        ProcessR15Chain(LegPts, Count);
                    }
                }
            }
        }

        if (Globals::Aimbot::DrawFov)
        {
            static float FovRotation = 0.f;

            if (Globals::Aimbot::FovSpin)
            {
                FovRotation += Globals::Aimbot::FovSpinSpeed / 1000.f;
            }

            POINT Cursor;
            GetCursorPos(&Cursor);

            HWND Roblox = FindWindowA(NULL, "Roblox");
            if (Roblox)
                ScreenToClient(Roblox, &Cursor);

            ImVec2 MousePos((float)Cursor.x, (float)Cursor.y);

            ImDrawList* Draw = ImGui::GetBackgroundDrawList();

            const int Sides = 10;
            ImVec2 Points[Sides];
            float Step = 2.f * IM_PI / Sides;

            for (int i = 0; i < Sides; i++)
            {
                float Angle = i * Step + FovRotation;
                Points[i] = ImVec2(MousePos.x + cosf(Angle) * Globals::Aimbot::FovSize, MousePos.y + sinf(Angle) * Globals::Aimbot::FovSize);
            }

            ImU32 Color = ImGui::ColorConvertFloat4ToU32(ImVec4(Globals::Aimbot::FovColor[0], Globals::Aimbot::FovColor[1], Globals::Aimbot::FovColor[2], Globals::Aimbot::FovColor[3]));

            if (Globals::Aimbot::FillFov)
            {
                Draw->AddConvexPolyFilled(Points, Sides, IM_COL32((Color >> 0) & 0xFF, (Color >> 8) & 0xFF, (Color >> 16) & 0xFF, 100));
            }

            Draw->AddPolyline(Points, Sides, Color, true, 2.f);
        }

        if (Globals::Silent::DrawFov)
        {
            static float FovRotation = 0.f;

            if (Globals::Silent::FovSpin)
            {
                FovRotation += Globals::Silent::FovSpinSpeed / 1000.f;
            }

            POINT Cursor;
            GetCursorPos(&Cursor);

            HWND Roblox = FindWindowA(NULL, "Roblox");
            if (Roblox)
                ScreenToClient(Roblox, &Cursor);

            ImVec2 MousePos((float)Cursor.x, (float)Cursor.y);

            ImDrawList* Draw = ImGui::GetBackgroundDrawList();

            const int Sides = 10;
            ImVec2 Points[Sides];
            float Step = 2.f * IM_PI / Sides;

            for (int i = 0; i < Sides; i++)
            {
                float Angle = i * Step + FovRotation;
                Points[i] = ImVec2(MousePos.x + cosf(Angle) * Globals::Silent::Fov, MousePos.y + sinf(Angle) * Globals::Silent::Fov);
            }

            ImU32 Color = ImGui::ColorConvertFloat4ToU32(ImVec4(Globals::Silent::FovColor[0], Globals::Silent::FovColor[1], Globals::Silent::FovColor[2], Globals::Silent::FovColor[3]));

            if (Globals::Silent::FillFov)
            {
                Draw->AddConvexPolyFilled(Points, Sides, IM_COL32((Color >> 0) & 0xFF, (Color >> 8) & 0xFF, (Color >> 16) & 0xFF, 100));
            }

            Draw->AddPolyline(Points, Sides, Color, true, 2.f);
        }
    }
}
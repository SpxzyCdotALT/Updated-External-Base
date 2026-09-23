#pragma once
#include <memory>
#include <vector>
#include <atomic>

#include "Engine/Engine.h"
#include "Core/Features/Cache/Cache.h"
#include "ImGui/imgui.h"

namespace Globals {

	inline SDK::Datamodel Datamodel;
	inline std::uint64_t GameID;
	inline SDK::VisualEngine VisualEngine;
	inline SDK::Player LocalPlayer;
	inline SDK::Players Players;
	inline SDK::Datamodel Workspace;
	inline SDK::Lighting Lighting;
	inline SDK::Renderview Renderview;
	inline SDK::Camera Camera;

	inline std::vector<SDK::Player> Player_Cache;

	namespace Settings {

		inline bool Team_Check;
		inline bool Client_Check;
		inline bool Streamproof;
		inline int Performance_Mode = 2;
	}

	namespace World {

		inline bool Skybox;
		inline bool Rotate;
		inline bool Ambience;
		inline bool Fog;
		inline bool Brightness;
		inline bool Exposure;
		inline bool FOV;

		inline int Skybox_Type = 0;
		inline float Skybox_Rotate_Speed = 1.0f;
		inline float Fog_Distance = 300.0f;
		inline float FOV_Distance = 90.0f;

		inline float ExposureI = 0.0f;
		inline float BrightnessI = 1.0f;

		namespace Colors {

			inline float Ambience[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };
			inline float Fog[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };
		}
	}

	namespace Aimbot {

		inline bool useFov;


		inline bool Enabled;
		inline bool DrawFov;
		inline bool FovSpin;
		inline bool ClosestPlayerFound;
		inline bool FillFov;
		inline bool AimbotSticky;
		inline bool Shake;
		inline bool KnockedCheck;

		inline float ShakeX{ 0.0f };
		inline float ShakeY{ 0.0f };
		inline float ShakeZ{ 0.0f };


		inline SDK::Instance AimTarget;
		inline float FovSize = 50;

		inline int HitPart = 0; // 0 = Head, 1 = Torso, 2 = LowerTorso
		inline int Aimbot_type = 0; // 0 = Mouse , 1 = Camera

		inline float FovColor[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };

		inline int FovSpinSpeed = 1;

		inline ImGuiKey Aimbot_Key = ImGuiKey_Q;
		inline ImKeyBindMode Aimbot_Mode = ImKeyBindMode_Hold;


		namespace Camera {
			inline float Smoothing_X{ 1.0f };
			inline float Smoothing_Y{ 1.0f };
		}

		namespace Mouse {
			inline float Smoothing_X{ 1.0f };
			inline float Smoothing_Y{ 1.0f };

			inline float Mouse_Sensitivty{ 1.0f };
		}
	}

	namespace Silent
	{
		enum SilentMethod {
			SILENT_VIEWPORT = 0,
			SILENT_MOUSE,
			SILENT_RAYCAST,
			SILENT_MAGIC_BULLET,
			SILENT_METHOD_COUNT
		};

		inline bool DrawFov{ false };
		inline bool Enabled{ false };
		inline bool StickyAim{ false };
		inline bool UseFov{ false };
		inline bool KnockedCheck{ false };
		inline bool GunBasedFov{ false };
		inline int Method{ SILENT_RAYCAST };
		inline float Fov{ 67.67f };
		inline float FovDoubleBarrel{ 67.67f };
		inline float FovTacticalShotgun{ 67.67f };
		inline float FovRevolver{ 67.67f };
		inline ImGuiKey Silent_Key = ImGuiKey_Q;
		inline ImKeyBindMode Silent_Mode = ImKeyBindMode_Toggle;
		inline int AimPart{ 0 };
		inline float FovColor[4]{ 0.960784f, 0.709804f, 0.960784f, 1.0f };
		inline int FovSpinSpeed = 1;
		inline bool FovSpin;
		inline bool FillFov;
	}

	namespace Visuals {

		inline bool Enabled;
		inline bool Box;
		inline bool Box_Fill;
		inline bool Box_Fill_Gradient;
		inline bool Box_Fill_Gradient_Rotate;
		inline bool Healthbar;
		inline bool Health;
		inline bool Name;
		inline bool Distance;
		inline bool Rig_Type;
		inline bool Tool;
		inline bool Skeleton;
		inline bool Chams;
		inline bool ChamsFade;

		inline float Render_Distance = 200.0f;

		inline int ChamsFadeSpeed = 2;
		inline int BoxFillSpeed = 2;
		inline int Healthbar_Type = 1;
		inline int Box_Type = 0;
		inline int Box_Fill_Type = 0;
		inline int Name_Type = 1;
		inline int Gap = 2;
		inline int Thickness = 2;

		namespace Colors {

			inline float Box[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };
			inline float BoxFill_Top[4] = { 0.960784f, 0.709804f, 0.960784f, 0.50f };
			inline float BoxFill_Bottom[4] = { 0.f, 0.f, 0.f, 0.50f };
			inline float Healthbar[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };
			inline float Name[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };
			inline float Distance[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };
			inline float Rig_Type[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };
			inline float Tool[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };
			inline float Health[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };
			inline float Skeleton[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };
			inline float Chams[4] = { 0.960784f, 0.709804f, 0.960784f, 1.0f };
			inline float ChamsOutline[4] = { 0.f, 0.f, 0.f, 1.0f };

			inline float Healthbar_Top[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
			inline float Healthbar_Middle[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
			inline float Healthbar_Bottom[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		}
	}

	namespace Misc {

		inline bool Fly = true;
		inline float Fly_Speed = 50.0f;
		inline ImGuiKey Fly_Key = ImGuiKey_Z;
		inline ImKeyBindMode Fly_Mode = ImKeyBindMode_Toggle;
	}
}
#define NOMINMAX
#include <Windows.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>
#include "Silent.h"
#include "ViewportSilent.h"
#include "MouseSilent.h"
#include "RaycastSilent.h"
#include "MagicBullet.h"
#include <Globals.hxx>
#include "Engine/Engine.h"
#include "Engine/Math/Math.h"

namespace Silent
{
	namespace
	{
		inline bool IsSilentReady{ false };
		inline SDK::Player SilentCachedTarget{};
		inline SDK::Vector3 SilentTargetPos{};
		inline bool SilentAimLocked{ false };
		inline bool SilentAimKeyWasPressed{ false };
		inline bool SilentFTarget{ false };

		float GetEffectiveFov()
		{
			if (!Globals::Silent::GunBasedFov)
				return Globals::Silent::Fov;

			std::string ToolName = Globals::LocalPlayer.Tool_Name;

			if (ToolName.empty())
				return Globals::Silent::Fov;

			std::transform(ToolName.begin(), ToolName.end(), ToolName.begin(), ::tolower);

			if (ToolName.find("double-barrel") != std::string::npos ||
				ToolName.find("double barrel") != std::string::npos ||
				ToolName.find("doublebarrel") != std::string::npos)
			{
				return Globals::Silent::FovDoubleBarrel;
			}
			else if (ToolName.find("tacticalshotgun") != std::string::npos ||
				ToolName.find("tactical shotgun") != std::string::npos)
			{
				return Globals::Silent::FovTacticalShotgun;
			}
			else if (ToolName.find("revolver") != std::string::npos)
			{
				return Globals::Silent::FovRevolver;
			}

			return Globals::Silent::Fov;
		}

		SDK::Instance GetTargetPart(SDK::Player& Player, int AimPart)
		{
			SDK::Instance TargetPart{};

			if (AimPart == 0)
			{
				TargetPart = Player.Head;
			}
			else if (AimPart == 1)
			{
				if (Player.UpperTorso.Address != 0)
					TargetPart = Player.UpperTorso;
				else
					TargetPart = Player.Torso;
			}
			else if (AimPart == 2)
			{
				if (Player.LowerTorso.Address != 0)
					TargetPart = Player.LowerTorso;
				else
					TargetPart = Player.HumanoidRootPart;
			}

			return TargetPart;
		}

		bool IsPlayerKnocked(SDK::Player& Player)
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

		bool IsTargetWithinFov(SDK::Player& Player)
		{
			if (Player.Character.Address == 0)
				return false;

			POINT CursorPoint;
			HWND Window = FindWindowA(nullptr, "Roblox");
			if (!Window || !GetCursorPos(&CursorPoint) || !ScreenToClient(Window, &CursorPoint))
				return false;

			SDK::Vector2 Cursor = { static_cast<float>(CursorPoint.x), static_cast<float>(CursorPoint.y) };

			SDK::Instance TargetPart = GetTargetPart(Player, Globals::Silent::AimPart);
			if (TargetPart.Address == 0)
				return false;

			SDK::Part PartObj(TargetPart.Address);
			SDK::Vector3 PartPosition = PartObj.Get_PartPosition();

			SDK::Vector2 PartScreen = Globals::VisualEngine.World_To_Screen(PartPosition);

			if (PartScreen.x < 0 || PartScreen.y < 0)
				return false;

			float DistanceFromCursor = PartScreen.distance(Cursor);

			return DistanceFromCursor <= GetEffectiveFov();
		}

		SDK::Player GetClosestPlayerFromCursor()
		{
			POINT CursorPoint;
			HWND Window = FindWindowA(nullptr, "Roblox");
			if (!Window || !GetCursorPos(&CursorPoint) || !ScreenToClient(Window, &CursorPoint))
				return {};

			SDK::Vector2 Cursor = { static_cast<float>(CursorPoint.x), static_cast<float>(CursorPoint.y) };

			std::vector<SDK::Player> PlayersSnapshot;
			{
				PlayersSnapshot = Globals::Player_Cache;
			}

			if (PlayersSnapshot.empty())
			{
				return {};
			}

			SDK::Player ClosestPlayer{};
			float ShortestDistance = std::numeric_limits<float>::max();

			for (SDK::Player& Player : PlayersSnapshot)
			{
				if (Player.Character.Address == 0)
					continue;

				if (Player.Character.Address == Globals::LocalPlayer.Character.Address)
					continue;

				SDK::Instance TargetPart = GetTargetPart(Player, Globals::Silent::AimPart);
				if (TargetPart.Address == 0)
					continue;

				SDK::Part PartObj(TargetPart.Address);
				SDK::Vector3 PartPosition = PartObj.Get_PartPosition();
				SDK::Vector2 PartScreen = Globals::VisualEngine.World_To_Screen(PartPosition);

				if (PartScreen.x < 0 || PartScreen.y < 0)
					continue;

				float DistanceFromCursor = PartScreen.distance(Cursor);

				if (Globals::Silent::UseFov && DistanceFromCursor > GetEffectiveFov())
					continue;

				if (Globals::Silent::KnockedCheck && IsPlayerKnocked(Player))
					continue;

				if (DistanceFromCursor < ShortestDistance)
				{
					ShortestDistance = DistanceFromCursor;
					ClosestPlayer = Player;
				}
			}

			return ClosestPlayer;
		}

		bool ShouldSilentAimBeActive()
		{
			if (!Globals::Silent::Enabled)
				return false;

			return SilentAimLocked;
		}

		void UpdateSilentAimKeyState()
		{
			int Vk = ImGuiKeyToVK(Globals::Silent::Silent_Key);
			if (!Vk) return;

			bool Pressed = (GetAsyncKeyState(Vk) & 0x8000) != 0;

			if (Globals::Silent::Silent_Mode == ImKeyBindMode_Toggle)
			{
				if (Pressed && !SilentAimKeyWasPressed)
				{
					SilentAimLocked = !SilentAimLocked;
				}

				if (!SilentAimLocked)
				{
					SilentCachedTarget = {};
					IsSilentReady = false;
				}
			}
			else
			{
				if (Pressed)
				{
					SilentAimLocked = true;
				}
				else
				{
					SilentAimLocked = false;
					SilentCachedTarget = {};
					IsSilentReady = false;
				}
			}

			SilentAimKeyWasPressed = Pressed;
		}

		bool UsesRaycastHook()
		{
			return Globals::Silent::Method == Globals::Silent::SILENT_RAYCAST ||
				Globals::Silent::Method == Globals::Silent::SILENT_MAGIC_BULLET;
		}

		void SilenceAll()
		{
			ViewportSilent::SetActive(false);
			MouseSilent::SetActive(false);
			RaycastSilent::SetActive(false);
		}

		void DispatchMethod(bool active, const SDK::Vector3& world)
		{
			if (!active)
			{
				SilenceAll();
				return;
			}

			switch (Globals::Silent::Method)
			{
			case Globals::Silent::SILENT_VIEWPORT:
				ViewportSilent::SetActive(true, world);
				break;

			case Globals::Silent::SILENT_MOUSE:
				MouseSilent::SetActive(true, world);
				break;

			case Globals::Silent::SILENT_RAYCAST:
				RaycastSilent::SetActive(true, world, false);
				break;

			case Globals::Silent::SILENT_MAGIC_BULLET:
				RaycastSilent::SetActive(true, world, true);
				break;

			default:
				SilenceAll();
				break;
			}
		}
	}

	void RunService()
	{
		for (;;)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			if (!Globals::Datamodel.Address || !Globals::VisualEngine.Address)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			UpdateSilentAimKeyState();

			// magic и raycast делят один слот BoundFunc
			if (UsesRaycastHook())
			{
				if (MagicBullet::Ready())
				{
					MagicBullet::Ensure(false);
				}
				if (!RaycastSilent::Ready())
				{
					RaycastSilent::Ensure(true);
				}
			}
			else
			{
				MagicBullet::Ensure(false);
				RaycastSilent::Ensure(false);
			}

			if (!ShouldSilentAimBeActive())
			{
				SilenceAll();
				IsSilentReady = false;
				SilentCachedTarget = {};
				SilentFTarget = false;
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			SDK::Player Target{};

			if (!SilentFTarget || SilentCachedTarget.Character.Address == 0)
			{
				Target = GetClosestPlayerFromCursor();
				SDK::Instance TargetPart = GetTargetPart(Target, Globals::Silent::AimPart);
				SilentFTarget = (TargetPart.Address != 0);
				SilentCachedTarget = Target;
			}
			else
			{
				if (!Globals::Silent::StickyAim)
				{
					Target = GetClosestPlayerFromCursor();
					SilentCachedTarget = Target;
				}
				else if (Globals::Silent::UseFov)
				{
					if (!IsTargetWithinFov(SilentCachedTarget))
					{
						SilentFTarget = false;
						SilentCachedTarget = {};
						continue;
					}
				}
			}

			if (SilentFTarget && SilentCachedTarget.Character.Address != 0)
			{
				if (Globals::Silent::KnockedCheck && IsPlayerKnocked(SilentCachedTarget))
				{
					SilentFTarget = false;
					SilentCachedTarget = {};
					continue;
				}

				SDK::Instance TargetPart = GetTargetPart(SilentCachedTarget, Globals::Silent::AimPart);
				if (TargetPart.Address != 0)
				{
					SDK::Part PartObj(TargetPart.Address);
					SDK::Vector3 Part3D = PartObj.Get_PartPosition();
					SilentTargetPos = Part3D;
					DispatchMethod(true, SilentTargetPos);
					IsSilentReady = true;
					continue;
				}
			}

			IsSilentReady = false;
			SilenceAll();
		}
	}
}
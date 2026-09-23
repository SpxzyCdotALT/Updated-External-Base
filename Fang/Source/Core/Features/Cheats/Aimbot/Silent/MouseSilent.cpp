#define NOMINMAX
#include <Windows.h>
#include <cstdint>

#include "MouseSilent.h"
#include <Globals.hxx>
#include "Engine/Engine.h"
#include "Engine/Math/Math.h"

namespace MouseSilent
{
	namespace
	{
		bool g_aiming = false;
		std::uint64_t g_mouse_service = 0;

		bool world_to_screen(const SDK::Matrix4& m, const SDK::Vector2& dim, const SDK::Vector3& p, SDK::Vector2& out)
		{
			float w = p.x * m.data[12] + p.y * m.data[13] + p.z * m.data[14] + m.data[15];
			if (w < 0.01f)
			{
				return false;
			}

			float x = p.x * m.data[0] + p.y * m.data[1] + p.z * m.data[2] + m.data[3];
			float y = p.x * m.data[4] + p.y * m.data[5] + p.z * m.data[6] + m.data[7];
			float inv = 1.0f / w;

			out.x = (dim.x * 0.5f) + (x * inv * dim.x * 0.5f);
			out.y = (dim.y * 0.5f) - (y * inv * dim.y * 0.5f);
			return true;
		}

		std::uint64_t resolve_mouse()
		{
			if (g_mouse_service && Driver->IsValid(g_mouse_service))
			{
				return g_mouse_service;
			}

			if (!Globals::Datamodel.Address ||
				!Driver->IsValid(Globals::Datamodel.Address))
			{
				return 0;
			}

			SDK::Instance ms = Globals::Datamodel.Find_First_Child_Of_Class("MouseService");
			if (!ms.Address || !Driver->IsValid(ms.Address))
			{
				return 0;
			}

			g_mouse_service = ms.Address;
			return g_mouse_service;
		}

		bool write_mouse_pos(std::uint64_t mouse_service, float x, float y)
		{
			auto try_input = [&](std::uintptr_t input_off) -> bool
			{
				std::uint64_t input = Driver->Read<std::uint64_t>(mouse_service + input_off);
				if (!input || input == (std::uint64_t)-1 || !Driver->IsValid(input))
				{
					return false;
				}

				float pos[2]{ x, y };
				return Driver->WriteRaw(
					input + Offsets::MouseService::MousePosition, pos, sizeof(pos)) == sizeof(pos);
			};

			if (try_input(Offsets::MouseService::InputObject2))
			{
				return true;
			}

			return try_input(Offsets::MouseService::InputObject);
		}

		bool resolve_view(SDK::Matrix4& out_view, SDK::Vector2& out_dims)
		{
			if (!Globals::Workspace.Address || !Globals::VisualEngine.Address)
			{
				return false;
			}

			SDK::Vector2 dims = Globals::VisualEngine.Get_Dimensions();
			if (dims.x < 1.f || dims.y < 1.f)
			{
				return false;
			}

			static std::uintptr_t base = 0;
			if (!base)
			{
				base = Driver->Get_Module();
			}

			if (!base)
			{
				return false;
			}

			std::uintptr_t ve = Driver->Read<std::uintptr_t>(base + Offsets::VisualEngine::Pointer);
			if (!Driver->IsValid(ve))
			{
				return false;
			}

			out_view = Driver->Read<SDK::Matrix4>(ve + Offsets::VisualEngine::ViewMatrix);
			out_dims = dims;
			return true;
		}
	}

	void Restore()
	{
		g_aiming = false;
	}

	void SetActive(bool on, const SDK::Vector3& world_target)
	{
		if (!on)
		{
			Restore();
			return;
		}

		std::uint64_t ms = resolve_mouse();
		if (!ms)
		{
			return;
		}

		SDK::Matrix4 view{};
		SDK::Vector2 dims{};
		if (!resolve_view(view, dims))
		{
			return;
		}

		SDK::Vector2 target{};
		if (!world_to_screen(view, dims, world_target, target))
		{
			return;
		}

		if (!write_mouse_pos(ms, target.x, target.y))
		{
			g_mouse_service = 0;
			return;
		}

		g_aiming = true;
	}

	bool Aiming()
	{
		return g_aiming;
	}
}
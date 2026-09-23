#pragma once
#include "Engine/Math/Math.h"
#include <cstdint>

namespace MouseSilent
{
	void SetActive(bool on, const SDK::Vector3& world_target = {});
	void Restore();
	bool Aiming();
}
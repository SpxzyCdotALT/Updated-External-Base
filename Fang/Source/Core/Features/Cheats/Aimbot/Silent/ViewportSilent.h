#pragma once
#include "Engine/Math/Math.h"

namespace ViewportSilent
{
	void SetActive(bool on, const SDK::Vector3& world_target = {});
	void Restore();
	void Shutdown();
	bool Aiming();
}
#include "../../Engine.h"

namespace SDK {

	SDK::Vector2 SDK::VisualEngine::Get_Dimensions() const {

		return Driver->Read<SDK::Vector2>(this->Address + Offsets::VisualEngine::Dimensions);
	}

	SDK::Matrix4 SDK::VisualEngine::Get_ViewMatrix() const {

		return Driver->Read<SDK::Matrix4>(this->Address + Offsets::VisualEngine::ViewMatrix);
	}

	SDK::Vector2 SDK::VisualEngine::World_To_Screen(const SDK::Vector3& World)  const {

		SDK::Matrix4 ViewMatrix = Get_ViewMatrix();
		SDK::Vector2 Dimensions = Get_Dimensions();
		SDK::Vector4 Q;
		Q.x = (World.x * ViewMatrix.data[0]) + (World.y * ViewMatrix.data[1]) + (World.z * ViewMatrix.data[2]) + ViewMatrix.data[3];
		Q.y = (World.x * ViewMatrix.data[4]) + (World.y * ViewMatrix.data[5]) + (World.z * ViewMatrix.data[6]) + ViewMatrix.data[7];
		Q.z = (World.x * ViewMatrix.data[8]) + (World.y * ViewMatrix.data[9]) + (World.z * ViewMatrix.data[10]) + ViewMatrix.data[11];
		Q.w = (World.x * ViewMatrix.data[12]) + (World.y * ViewMatrix.data[13]) + (World.z * ViewMatrix.data[14]) + ViewMatrix.data[15];
		if (Q.w < 0.1f)
			return { -1, -1 };
		SDK::Vector3 NDC;
		NDC.x = Q.x / Q.w;
		NDC.y = Q.y / Q.w;
		NDC.z = Q.z / Q.w;
		SDK::Vector2 ScreenPosition =
		{
			(Dimensions.x / 2 * NDC.x) + (Dimensions.x / 2),
			-(Dimensions.y / 2 * NDC.y) + (Dimensions.y / 2),
		};
		if (ScreenPosition.x < 0 || ScreenPosition.x > Dimensions.x || ScreenPosition.y < 0 || ScreenPosition.y > Dimensions.y)
			return { -1, -1 };
		return ScreenPosition;
	}
}
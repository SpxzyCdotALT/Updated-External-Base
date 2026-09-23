#include "../../Engine.h"

namespace SDK {

	SDK::Vector3 SDK::Camera::Get_CameraPos() const {

		return Driver->Read<SDK::Vector3>(this->Address + Offsets::Camera::Position);
	}

	SDK::Matrix3 SDK::Camera::Get_CameraRot() const {

		return Driver->Read<Matrix3>(this->Address + Offsets::Camera::Rotation);
	}

	void SDK::Camera::Set_CameraPos(const SDK::Vector3& pos) const {

		Driver->Write<SDK::Vector3>(this->Address + Offsets::Camera::Position, pos);
	}

	void SDK::Camera::Set_CameraRot(const SDK::Matrix3& rot) const {

		Driver->Write<SDK::Matrix3>(this->Address + Offsets::Camera::Rotation, rot);
	}

	Vector2 SDK::Camera::FetchViewPort(Vector2 target_screen_pos, Vector2 screen_size) {

		Vector2 result;
		result.x = (int16_t)(2 * (screen_size.x - target_screen_pos.x));
		result.y = (int16_t)(2 * (screen_size.y - target_screen_pos.y));
		return result;
	}

	void SDK::Camera::SetViewPort(SDK::ViewPort Vp) const {

		Driver->Write<SDK::ViewPort>(this->Address + Offsets::Camera::Viewport, Vp);
	}
}
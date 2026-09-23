#include "../../Engine.h"

namespace SDK {

	std::uint64_t SDK::Players::Get_UserID() const {

		std::uint64_t userId = Driver->Read<std::uint64_t>(this->Address + Offsets::Player::UserId);
		return userId;
	}

	std::uint64_t SDK::Players::Get_Team() const {

		return Driver->Read<uint64_t>(this->Address + Offsets::Player::Team);
	}

	SDK::Players SDK::Players::Get_Local_Player() const {

		std::uint64_t local_address = Driver->Read<std::uint64_t>(this->Address + Offsets::Player::LocalPlayer);
		return SDK::Players{ local_address };
	}

	SDK::Instance SDK::Instance::Character() const {

		if (!this->Address)
		return SDK::Instance();
		return Driver->Read<SDK::Players>(this->Address + Offsets::Player::ModelInstance);
	}


	std::string Players::Get_DisplayName() const
	{
		if (!this->Address)
		return "?";
		return Driver->Read_String(this->Address + Offsets::Player::DisplayName);
	}
}
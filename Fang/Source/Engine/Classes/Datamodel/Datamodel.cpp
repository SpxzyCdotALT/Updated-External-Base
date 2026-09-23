#include "../../Engine.h"

namespace SDK {

	std::uint64_t SDK::Datamodel::Get_PlaceID() const {

		return Driver->Read<uint64_t>(this->Address + Offsets::DataModel::PlaceId);
	}

	std::uint64_t SDK::Datamodel::Get_GameID() const {

		return Driver->Read<uint64_t>(this->Address + Offsets::DataModel::GameId);
	}

	std::uint64_t SDK::Datamodel::Get_CreatorID() const {

		return Driver->Read<uint64_t>(this->Address + Offsets::DataModel::CreatorId);
	}

	std::uint64_t SDK::Datamodel::Get_ServerIP() const {

		return Driver->Read<uint64_t>(this->Address + Offsets::DataModel::ServerIP);
	}
}
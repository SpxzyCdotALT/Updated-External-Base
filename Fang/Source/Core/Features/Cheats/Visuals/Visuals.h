#pragma once
#include <vector>
#include <Globals.hxx>

namespace Visuals {

    std::vector<const SDK::Instance*> Get_Bones(const SDK::Player& Player);
    void RunService();
}
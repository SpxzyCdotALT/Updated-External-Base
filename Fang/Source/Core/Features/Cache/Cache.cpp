#include "Cache.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <cmath>
#include <Miscellaneous/Output/Output.h>
#include <Globals.hxx>
#include "PhantomForces/PhantomForces.h"
namespace Cache {

    struct Part_Mapping {

        std::string_view Name;
        SDK::Instance SDK::Player::* Member;
    };

    constexpr Part_Mapping Part_Mappings[] = {

        {"Humanoid", &SDK::Player::Humanoid},
        {"HumanoidRootPart", &SDK::Player::HumanoidRootPart},
        {"Head", &SDK::Player::Head},
        {"Torso", &SDK::Player::Torso},
        {"UpperTorso", &SDK::Player::UpperTorso},
        {"LowerTorso", &SDK::Player::LowerTorso},
        {"Left Arm", &SDK::Player::LeftArm},
        {"Right Arm", &SDK::Player::RightArm},
        {"Left Leg", &SDK::Player::LeftLeg},
        {"Right Leg", &SDK::Player::RightLeg},
        {"LeftUpperLeg", &SDK::Player::LeftUpperLeg},
        {"RightUpperLeg", &SDK::Player::RightUpperLeg},
        {"LeftLowerLeg", &SDK::Player::LeftLowerLeg},
        {"RightLowerLeg", &SDK::Player::RightLowerLeg},
        {"LeftFoot", &SDK::Player::LeftFoot},
        {"RightFoot", &SDK::Player::RightFoot},
        {"LeftHand", &SDK::Player::LeftHand},
        {"RightHand", &SDK::Player::RightHand},
        {"LeftUpperArm", &SDK::Player::LeftUpperArm},
        {"RightUpperArm", &SDK::Player::RightUpperArm},
        {"LeftLowerArm", &SDK::Player::LeftLowerArm},
        {"RightLowerArm", &SDK::Player::RightLowerArm}
    };

    std::unordered_map<std::string, SDK::Instance SDK::Player::*> Create_Part_Lookup() {

        std::unordered_map<std::string, SDK::Instance SDK::Player::*> Map;
        Map.reserve(sizeof(Part_Mappings) / sizeof(Part_Mappings[0]));
        for (const auto& Mapping : Part_Mappings) {
            Map.emplace(Mapping.Name, Mapping.Member);
        }
        return Map;
    }

    const auto Part_Lookup = Create_Part_Lookup();
    std::atomic<bool> References_Updated{ false };
    std::atomic<std::uint64_t> Current_GameID{ 0 };
    std::mutex Cache_Mutex;

    inline float Calculate_Distance(const SDK::Vector3& P1, const SDK::Vector3& P2) {

        float Dx = P1.x - P2.x;
        float Dy = P1.y - P2.y;
        float Dz = P1.z - P2.z;
        return std::sqrt(Dx * Dx + Dy * Dy + Dz * Dz);
    }

    bool Valid_Position(const SDK::Vector3& Pos) {

        return !std::isnan(Pos.x) && !std::isnan(Pos.y) && !std::isnan(Pos.z);
    }

    void Rescan() {
        static std::uint64_t Stored_GameID = 0;

        while (true) {
            try {
                auto FakeDataModel = Driver->Read<std::uint64_t>(Driver->Get_Module() + Offsets::FakeDataModel::Pointer);
                Globals::Datamodel.Address = Driver->Read<std::uint64_t>(FakeDataModel + Offsets::FakeDataModel::RealDataModel);

                if (Globals::Datamodel.Address != 0) {
                    std::uint64_t GameID = Driver->Read<uint64_t>(Globals::Datamodel.Address + Offsets::DataModel::PlaceId);

                    if (GameID != Stored_GameID) {
                        Stored_GameID = GameID;
                        Current_GameID.store(GameID);
                        Globals::GameID = GameID;

                        Globals::Players.Address = Globals::Datamodel.Find_First_Child_Of_Class("Players").Address;
                        auto Lightin = Globals::Datamodel.Find_First_Child_Of_Class("Lighting");
                        Globals::Lighting = SDK::Lighting(Lightin.Address);
                        Globals::Workspace.Address = Globals::Datamodel.Find_First_Child_Of_Class("Workspace").Address;
                        std::this_thread::sleep_for(std::chrono::seconds(5));
                        Globals::Camera.Address = Globals::Workspace.Find_First_Child_Of_Class("Camera").Address;

                        References_Updated.store(true);

                        system("cls");
                        Output::Success("Process Found -> %u", Driver->Get_Process());
                        Output::Success("Attached To Process -> 0x%llx", Driver->Get_Handle());
                        Output::Success("Base Module Address -> 0x%llx", Driver->Get_Module());
                        Output::Fang("Datamodel Address -> 0x%llx", Globals::Datamodel);
                        Output::Fang("VisualEngine Address -> 0x%llx", Globals::VisualEngine);
                        Output::Fang("Players Address -> 0x%llx", Globals::Players);
                        Output::Fang("LocalPlayer Address -> 0x%llx", Globals::LocalPlayer);
                        Output::Fang("Workspace Address -> 0x%llx", Globals::Workspace);
                        Output::Fang("Camera Address -> 0x%llx", Globals::Camera);
                        Output::Fang("Lighting Address -> 0x%llx", Globals::Lighting);
                    }
                }
            }
            catch (...) {}

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void Cache_Data(SDK::Player& Player, const SDK::Vector3& Local_Pos, bool Is_Local) {
        if (Player.Character.Address == 0) return;

        auto Children = Player.Character.Children();
        for (const auto& Part : Children) {

            auto it = Part_Lookup.find(Part.Name());
            if (it != Part_Lookup.end()) {

                Player.*(it->second) = Part;
            }
        }

        if (Player.Humanoid.Address) {

            SDK::Humanoid Humanoid(Player.Humanoid.Address);
            Player.Health = Humanoid.Get_Health();
            Player.MaxHealth = Humanoid.Get_MaxHealth();
            Player.Rig_Type = Humanoid.Get_RigType();
        }

        Player.Tool_Name.clear();
        SDK::Instance Tool = Player.Character.Find_First_Child_Of_Class("Tool");
        if (Tool.Address) {

            Player.Tool_Name = Tool.Name();
        }

        if (!Is_Local && Player.Head.Address != 0 && Globals::Camera.Address != 0) {

            SDK::Part Head(Player.Head.Address);
            SDK::Vector3 Head_Pos = Head.Get_PartPosition();

            SDK::Camera Camera(Globals::Camera.Address);
            SDK::Vector3 Camera_Pos = Camera.Get_CameraPos();

            if (Valid_Position(Head_Pos) && Valid_Position(Camera_Pos)) {
                Player.Distance = Calculate_Distance(Head_Pos, Camera_Pos);
            }
        }
    }

    void Update_Cache(const SDK::Vector3& Local_Pos, const std::string& Local_Name) {
        if (Globals::Players.Address == 0)
            return;

        SDK::Players Local_SDK_Player = Globals::Players.Get_Local_Player();
        SDK::Instance Local_Inst(Local_SDK_Player.Address);

        auto Player_Instances = Globals::Players.Children();

        std::vector<SDK::Player> Players;

        Players.reserve(Player_Instances.size());

        for (const auto& Instance : Player_Instances) {

            SDK::Player Player{};
            Player.Player = SDK::Players(Instance.Address);
            Player.Character = Player.Player.Character();
            Player.Name = Instance.Name();
            Player.UserID = Player.Player.Get_UserID();
            Player.Display_Name = Player.Player.Get_DisplayName();
            Player.Local_Player = false;

            Cache_Data(Player, Local_Pos, false);

            SDK::Players Entity(Instance.Address);
            SDK::Players LocalPlayer(Local_Inst.Address);

            if (Globals::Settings::Team_Check) {

                if (Entity.Get_Team() == LocalPlayer.Get_Team()) {
                    continue;
                }
            }

            if (!Globals::Settings::Client_Check || Player.HumanoidRootPart.Address != Globals::LocalPlayer.HumanoidRootPart.Address) {

                Players.push_back(std::move(Player));
            }
        }

        std::lock_guard<std::mutex> Lock(Cache_Mutex);
        Globals::Player_Cache = std::move(Players);
    }

    void Runtime() {
        if (Globals::Players.Address == 0)
            return;

        SDK::Players LocalPlayerInstance = Globals::Players.Get_Local_Player();
        SDK::Instance LocalPlayer2(LocalPlayerInstance.Address);
        if (LocalPlayer2.Address == 0)
            return;

        SDK::Instance Player_Instance(LocalPlayer2);

        SDK::Player LocalPlayer{};
        LocalPlayer.Player = SDK::Players(Player_Instance.Address);
        LocalPlayer.Character = Player_Instance.Character();
        LocalPlayer.Name = Player_Instance.Name();
        LocalPlayer.Local_Player = true;

        SDK::Vector3 Local_Position{};
        Cache_Data(LocalPlayer, Local_Position, true);

        if (LocalPlayer.HumanoidRootPart.Address != 0) {
            SDK::Part Local_HumanoidRootPart(LocalPlayer.HumanoidRootPart.Address);
            Local_Position = Local_HumanoidRootPart.Get_PartPosition();
        }

        Globals::LocalPlayer = LocalPlayer;
        Globals::GameID = Current_GameID.load();

        if (Globals::GameID == 292439477) {
            std::vector<SDK::Player> players;

            CachePlayers(players, Local_Position, LocalPlayer.Name);
            RescanCache(players, Local_Position, LocalPlayer.Name);
            {
                std::lock_guard<std::mutex> lock(Cache_Mutex);
                Globals::Player_Cache = std::move(players);
            }
        }
        else {
            Update_Cache(Local_Position, LocalPlayer.Name);
        }
    }
}

void Cache::RunService() {

    std::thread(Rescan).detach();

    while (true) {
        try {
            if (References_Updated.exchange(false)) {
                std::lock_guard<std::mutex> lock(Cache_Mutex);
                Globals::Player_Cache.clear();
            }

            Runtime();
        }
        catch (...) {}

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}





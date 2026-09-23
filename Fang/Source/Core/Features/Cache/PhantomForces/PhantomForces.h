#pragma once
#include <iostream>
#include <vector>
#include <mutex>
#include <cmath>
#include <chrono>
#include <thread>
#include "../Cache.h"
#include <Globals.hxx>

std::mutex CMutex;

inline float CalcDistance(const SDK::Vector3& P1, const SDK::Vector3& P2) {
    float Dx = P1.x - P2.x;
    float Dy = P1.y - P2.y;
    float Dz = P1.z - P2.z;
    return std::sqrt(Dx * Dx + Dy * Dy + Dz * Dz);
}

inline bool IsValidPos(const SDK::Vector3& Pos) {
    return !std::isnan(Pos.x) && !std::isnan(Pos.y) && !std::isnan(Pos.z);
}

std::vector<SDK::Instance> GetPlayers() {
    std::vector<SDK::Instance> TargetPlayers;

    const std::uint64_t WorkspaceAddr = Globals::Workspace.Address;
    if (!WorkspaceAddr) return TargetPlayers;

    SDK::Instance Workspace(WorkspaceAddr);
    SDK::Instance PlayersFolder = Workspace.Find_First_Child("Players");
    if (!PlayersFolder.Address) return TargetPlayers;

    auto Teams = PlayersFolder.GetChildren<SDK::Instance>();
    if (Teams.size() != 2) return TargetPlayers;

    const std::uint64_t LocalPlayerAddr = Globals::Players.Get_Local_Player().Address;
    if (!LocalPlayerAddr) return TargetPlayers;

    if (!Globals::Settings::Team_Check) {
        for (auto& Team : Teams) {
            for (auto& Player : Team.GetChildren<SDK::Instance>()) {
                if (Player.Class() == "Model") {
                    TargetPlayers.push_back(Player);
                }
            }
        }
    }
    else {
        SDK::Instance LocalPlayer(LocalPlayerAddr);
        SDK::Instance LocalChar = LocalPlayer.Character();
    }

    return TargetPlayers;
}

void CachePlayers(std::vector<SDK::Player>& Players, const SDK::Vector3& LocalPos, const std::string& LocalName) {
    std::lock_guard<std::mutex> Lock(CMutex);

    Players.clear();

    auto PlayerModels = GetPlayers();

    for (auto& Model : PlayerModels) {
        if (Model.Class() != "Model") continue;

        SDK::Player Player{};
        Player.Character = Model;
        Player.Bones.clear();

        for (auto& Child : Model.GetChildren<SDK::Instance>()) {
            std::string ClassName = Child.Class();

            if (ClassName == "Folder") {
                for (auto& Part : Child.GetChildren<SDK::Instance>()) {
                    std::string PartClass = Part.Class();
                    if (PartClass == "Part" || PartClass == "MeshPart") {
                        Player.Bones.push_back(Part);
                    }
                }
            }
            else if (ClassName == "Part" || ClassName == "MeshPart") {
                Player.Bones.push_back(Child);
            }
        }

        if (!Player.Bones.empty()) {
            Player.HumanoidRootPart = Player.Bones[0];
        }

        for (auto& Part : Player.Bones) {
            for (auto& Child : Part.GetChildren<SDK::Instance>()) {
                if (Child.Class() == "BillboardGui") {
                    Player.Head = Part;
                    SDK::Instance TextLabel = Child.Find_First_Child_Of_Class("TextLabel");
                    if (TextLabel.Address != 0) {
                        Player.Name = TextLabel.Text();
                    }
                    break;
                }
            }
            if (Player.Head.Address != 0) break;
        }

        if (Player.HumanoidRootPart.Address != 0) {
            SDK::Part Root(Player.HumanoidRootPart.Address);
            SDK::Vector3 PlayerPos = Root.Get_PartPosition();
            if (IsValidPos(PlayerPos) && IsValidPos(LocalPos)) {
                Player.Distance = CalcDistance(LocalPos, PlayerPos);
            }
        }

        if (!Player.Name.empty() && Player.Name != LocalName) {
            Players.push_back(std::move(Player));
        }
    }
}

void RescanCache(std::vector<SDK::Player>& Players, const SDK::Vector3& LocalPos, const std::string& LocalName) {
    std::thread([&Players, &LocalPos, &LocalName]() {
        while (true) {
            CachePlayers(Players, LocalPos, LocalName);
            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
        }).detach();
}
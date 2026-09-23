#include "Engine/Engine.h"
#include "Globals.hxx"
#include "Core/Features/Cache/Cache.h"
#include <iostream>

namespace World {

    void SkyboxChanger() {

        while (true)
        {
            if (Globals::World::Skybox)
            {
                auto Sky = Globals::Lighting.Find_First_Child("Sky");

                if (Sky.Address)
                {
                    if (Globals::World::Skybox_Type == 0)
                    {
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxBk, "rbxassetid://12635309703");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxDn, "rbxassetid://12635311686");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxFt, "rbxassetid://12635312870");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxLf, "rbxassetid://12635313718");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxRt, "rbxassetid://12635315817");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxUp, "rbxassetid://12635316856");
                    }
                    else if (Globals::World::Skybox_Type == 1)
                    {
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxBk, "rbxassetid://12064107");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxDn, "rbxassetid://12064152");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxFt, "rbxassetid://12064121");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxLf, "rbxassetid://12063984");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxRt, "rbxassetid://12064115");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxUp, "rbxassetid://12064131");
                    }
                    else if (Globals::World::Skybox_Type == 2)
                    {
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxBk, "rbxassetid://271042516");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxDn, "rbxassetid://271077243");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxFt, "rbxassetid://271042556");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxLf, "rbxassetid://271042310");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxRt, "rbxassetid://271042467");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxUp, "rbxassetid://271077958");
                    }
                    else if (Globals::World::Skybox_Type == 3)
                    {
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxBk, "rbxassetid://1876545003");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxDn, "rbxassetid://1876544331");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxFt, "rbxassetid://1876542941");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxLf, "rbxassetid://1876543392");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxRt, "rbxassetid://1876543764");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxUp, "rbxassetid://1876544642");
                    }
                    else if (Globals::World::Skybox_Type == 4)
                    {
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxBk, "rbxassetid://116758234");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxDn, "rbxassetid://116758314");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxFt, "rbxassetid://116758367");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxLf, "rbxassetid://116758446");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxRt, "rbxassetid://116758478");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxUp, "rbxassetid://116758496");
                    }
                    else if (Globals::World::Skybox_Type == 5)
                    {
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxBk, "rbxassetid://1233158420");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxDn, "rbxassetid://1233158838");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxFt, "rbxassetid://1233157105");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxLf, "rbxassetid://1233157640");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxRt, "rbxassetid://1233157995");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxUp, "rbxassetid://1233159158");
                    }
                    else if (Globals::World::Skybox_Type == 6)
                    {
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxBk, "rbxassetid://1327358");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxDn, "rbxassetid://1327359");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxFt, "rbxassetid://1327355");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxLf, "rbxassetid://1327357");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxRt, "rbxassetid://1327356");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxUp, "rbxassetid://1327360");
                    }
                    else if (Globals::World::Skybox_Type == 7)
                    {
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxBk, "rbxassetid://570555736");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxDn, "rbxassetid://570555964");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxFt, "rbxassetid://570555800");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxLf, "rbxassetid://570555840");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxRt, "rbxassetid://570555882");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxUp, "rbxassetid://570555929");
                    }
                    else if (Globals::World::Skybox_Type == 8)
                    {
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxBk, "rbxassetid://95020137072033");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxDn, "rbxassetid://92862258103959");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxFt, "rbxassetid://107665368823185");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxLf, "rbxassetid://126542804346203");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxRt, "rbxassetid://103716549795832");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxUp, "rbxassetid://131036626982613");
                    }
                    else if (Globals::World::Skybox_Type == 9)
                    {
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxBk, "rbxassetid://169210090");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxDn, "rbxassetid://169210108");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxFt, "rbxassetid://169210121");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxLf, "rbxassetid://169210133");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxRt, "rbxassetid://169210143");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxUp, "rbxassetid://169210149");
                    }
                    else if (Globals::World::Skybox_Type == 10)
                    {
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxBk, "rbxassetid://47974894");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxDn, "rbxassetid://47974690");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxFt, "rbxassetid://47974821");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxLf, "rbxassetid://47974776");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxRt, "rbxassetid://47974859");
                        Driver->Write_String(Sky.Address + Offsets::Sky::SkyboxUp, "rbxassetid://47974909");
                    }

                    if (Globals::World::Rotate) {

                        auto Sky = Globals::Lighting.Find_First_Child_Of_Class("Sky");

                        static float rotY = 0.0f;
                        rotY = (0.0f > 360.0f) ? 0.0f : rotY + Globals::World::Skybox_Rotate_Speed;

                        Driver->Write<SDK::Vector3>(Sky.Address + Offsets::Sky::SkyboxOrientation, { 0.0f, rotY, 0.0f });
                    }

                    SDK::Renderview::InvalidateLighting();
                    SDK::Renderview::ValidateSkybox();
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

    void AtmosphereChanger() {

        while (true)
        {
            if (Globals::World::Ambience)
            {
                SDK::Lighting::SetAmbient(Globals::Lighting.Address, {Globals::World::Colors::Ambience[0], Globals::World::Colors::Ambience[1], Globals::World::Colors::Ambience[2]} );
                SDK::Renderview::InvalidateLighting();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void FogChanger() {

        while (true)
        {
            if (Globals::World::Fog)
            {
                SDK::Lighting::SetFog(Globals::Lighting.Address, Globals::World::Fog_Distance, {Globals::World::Colors::Fog[0], Globals::World::Colors::Fog[1], Globals::World::Colors::Fog[2]} );
                SDK::Renderview::InvalidateLighting();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void BrightnessChanger() {

        while (true)
        {
            if (Globals::World::Brightness)
            {
                SDK::Lighting::SetBrightness(Globals::Lighting.Address, Globals::World::BrightnessI);
                SDK::Renderview::InvalidateLighting();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void ExposureChanger() {

        while (true)
        {
            if (Globals::World::Exposure)
            {
                SDK::Lighting::SetExposure(Globals::Lighting.Address, Globals::World::ExposureI);
                SDK::Renderview::InvalidateLighting();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void FOVChanger() {

        while (true)
        {
            if (Globals::World::FOV)
            {
				SDK::Lighting::SetFOV(Globals::Camera.Address, Globals::World::FOV_Distance);
            }
        }

    }

    void RunService() {

        std::thread(SkyboxChanger).detach();
        std::thread(AtmosphereChanger).detach();
        std::thread(FogChanger).detach();
        std::thread(BrightnessChanger).detach();
        std::thread(ExposureChanger).detach();
		std::thread(FOVChanger).detach();
    }
}

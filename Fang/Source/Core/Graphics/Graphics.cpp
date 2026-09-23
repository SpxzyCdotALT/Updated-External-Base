#include <dwmapi.h>
#include <cstdio>
#include <chrono>
#include <thread>
#include <d3d11.h>
#include <wincodec.h>
#include <vector>

#include "Graphics.h"
#include <Globals.hxx>

#include <imgui/misc/imgui_freetype.h>
#include "imgui/imgui_internal.h"

#include "Fonts/Tahoma.h"
#include "Fonts/Tahoma_Bold.h"
#include "../Features/Cheats/Visuals/Visuals.h"
#include <ImGui/addons/colors/colors.h>
#include "Core/Features/Cheats/Aimbot/Silent/Silent.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND Hwnd, UINT Msg, WPARAM WParam, LPARAM LParam);

LRESULT CALLBACK WndProc(HWND Hwnd, UINT Msg, WPARAM WParam, LPARAM LParam)
{
    if (ImGui_ImplWin32_WndProcHandler(Hwnd, Msg, WParam, LParam))
    {
        return true;
    }

    switch (Msg)
    {
    case WM_SYSCOMMAND:
        if ((WParam & 0xfff0) == SC_KEYMENU)
        {
            return 0;
        }
        break;

    case WM_SYSKEYDOWN:
        if (WParam == VK_F4) {
            DestroyWindow(Hwnd);
            return 0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        return 0;
    }

    return DefWindowProcA(Hwnd, Msg, WParam, LParam);
}

Graphics::Graphics()
{
    Detail = std::make_unique<detail_t>();
}

Graphics::~Graphics()
{
    Destroy_Imgui();
    Destroy_Window();
    Destroy_Device();
}

bool Graphics::Create_Window()
{
    Detail->WindowClass.cbSize = sizeof(Detail->WindowClass);
    Detail->WindowClass.style = CS_CLASSDC;
    Detail->WindowClass.lpszClassName = "Fang";
    Detail->WindowClass.hInstance = GetModuleHandleA(0);
    Detail->WindowClass.lpfnWndProc = WndProc;

    RegisterClassExA(&Detail->WindowClass);

    Detail->Window = CreateWindowExA(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW, Detail->WindowClass.lpszClassName, "Fang", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, 0, Detail->WindowClass.hInstance, 0);

    if (!Detail->Window)
    {
        return false;
    }

    SetLayeredWindowAttributes(Detail->Window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

    RECT ClientArea{};
    RECT WindowArea{};

    GetClientRect(Detail->Window, &ClientArea);
    GetWindowRect(Detail->Window, &WindowArea);

    POINT Diff{};
    ClientToScreen(Detail->Window, &Diff);

    MARGINS Margins
    {
        WindowArea.left + (Diff.x - WindowArea.left),
        WindowArea.top + (Diff.y - WindowArea.top),
        WindowArea.right,
        WindowArea.bottom,
    };

    DwmExtendFrameIntoClientArea(Detail->Window, &Margins);

    ShowWindow(Detail->Window, SW_SHOW);
    UpdateWindow(Detail->Window);

    return true;
}

bool Graphics::Create_Device()
{
    DXGI_SWAP_CHAIN_DESC SwapChainDesc{};

    SwapChainDesc.BufferCount = 2;
    SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

    SwapChainDesc.BufferDesc.Width = 0;
    SwapChainDesc.BufferDesc.Height = 0;
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    SwapChainDesc.OutputWindow = Detail->Window;

    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    SwapChainDesc.Windowed = 1;

    SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.SampleDesc.Quality = 0;

    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    D3D_FEATURE_LEVEL FeatureLevel;
    D3D_FEATURE_LEVEL FeatureLevelList[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

    HRESULT Result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, FeatureLevelList, 2, D3D11_SDK_VERSION, &SwapChainDesc, &Detail->SwapChain, &Detail->Device, &FeatureLevel, &Detail->DeviceContext);

    if (Result == DXGI_ERROR_UNSUPPORTED)
    {
        Result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, FeatureLevelList, 2, D3D11_SDK_VERSION, &SwapChainDesc, &Detail->SwapChain, &Detail->Device, &FeatureLevel, &Detail->DeviceContext);
    }

    if (Result != S_OK)
    {
        MessageBoxA(nullptr, "This software can not run on your computer.", "Critical Problem", MB_ICONERROR | MB_OK);
    }

    ID3D11Texture2D* BackBuffer{ nullptr };
    Detail->SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer));

    if (BackBuffer)
    {
        Detail->Device->CreateRenderTargetView(BackBuffer, nullptr, &Detail->GraphicsTargetView);
        BackBuffer->Release();

        return true;
    }

    return false;
}

bool Graphics::Create_Imgui()
{
    using namespace ImGui;
    CreateContext();
    StyleColorsDark();

    float MainScale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    ImGuiStyle& Style = ImGui::GetStyle();
    ImGuiIO& IO = ImGui::GetIO(); (void)IO;

    IO.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    const unsigned int freetype_flags = ImGuiFreeTypeLoaderFlags_MonoHinting | ImGuiFreeTypeLoaderFlags_Monochrome;
    IO.Fonts->SetFontLoader(ImGuiFreeType::GetFontLoader());
    IO.Fonts->FontLoaderFlags = freetype_flags;

    ImFontConfig font_cfg{};
    font_cfg.PixelSnapH = true;
    font_cfg.OversampleH = 2;
    font_cfg.OversampleV = 1;
    font_cfg.RasterizerMultiply = 1.05f;
    font_cfg.FontLoaderFlags = freetype_flags;

    ImFontConfig font_configuration = font_cfg;
    font_configuration.FontDataOwnedByAtlas = false;

	float Verdana_Size = 13.0f * MainScale;
    float Tahoma_Size = 13.0f * MainScale;
    float Tahoma_Bold_Size = 13.0f * MainScale;

    IO.Fonts->AddFontFromMemoryTTF(const_cast<unsigned char*>(Tahoma), sizeof(Tahoma), Verdana_Size, &font_configuration);

    Tahoma_BoldXP = IO.Fonts->AddFontFromMemoryTTF(const_cast<unsigned char*>(Tahoma_Bold), sizeof(Tahoma_Bold), Verdana_Size, &font_configuration, IO.Fonts->GetGlyphRangesCyrillic());

    if (!ImGui_ImplWin32_Init(Detail->Window))
    {
        return false;
    }

    if (!Detail->Device || !Detail->DeviceContext)
    {
        return false;
    }

    if (!ImGui_ImplDX11_Init(Detail->Device, Detail->DeviceContext))
    {
        return false;
    }

    return true;
}

void Graphics::Destroy_Device()
{
    if (Detail->GraphicsTargetView) Detail->GraphicsTargetView->Release();
    if (Detail->SwapChain) Detail->SwapChain->Release();
    if (Detail->DeviceContext) Detail->DeviceContext->Release();
    if (Detail->Device) Detail->Device->Release();
}

void Graphics::Destroy_Window()
{
    DestroyWindow(Detail->Window);
    UnregisterClassA(Detail->WindowClass.lpszClassName, Detail->WindowClass.hInstance);
}

void Graphics::Destroy_Imgui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Graphics::Start_Render()
{

    MSG Msg;
    while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    if (Globals::Settings::Streamproof)
    {
        SetWindowDisplayAffinity(Detail->Window, WDA_EXCLUDEFROMCAPTURE);
    }
    else
    {
        SetWindowDisplayAffinity(Detail->Window, WDA_NONE);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (GetAsyncKeyState(VK_F1) & 1)
    {
        Running = !Running;

        if (Running)
        {
            SetWindowLong(Detail->Window, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
        }
        else
        {
            SetWindowLong(Detail->Window, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED);
        }
    }
}

void Graphics::End_Render()
{
    ImGui::Render();

    float ClearColor[4]{ 0, 0, 0, 0 };
    Detail->DeviceContext->OMSetRenderTargets(1, &Detail->GraphicsTargetView, nullptr);
    Detail->DeviceContext->ClearRenderTargetView(Detail->GraphicsTargetView, ClearColor);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (Globals::Settings::Performance_Mode == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        Detail->SwapChain->Present(0, 0);
    }
    else if (Globals::Settings::Performance_Mode == 1)
    {
        Detail->SwapChain->Present(1, 0);
    }
    else
    {
        Detail->SwapChain->Present(0, 0);
    }
}

void Graphics::Render_Menu()
{
    ImGuiStyle& Style = ImGui::GetStyle();
    ImGuiIO& Io = ImGui::GetIO(); (void)Io;

    ImGui::SetNextWindowSize(ImVec2(500, 530), ImGuiCond_Once);
    ImGui::SetNextWindowPos(Io.DisplaySize / 2, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

    float HeaderHeight = ImGui::GetFontSize() + 6.0f * 2.0f;
    static ImVec2 LogoSize(25, 25);

    bool MainWindow = ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
    ImGui::PopStyleVar(2);

    if (MainWindow)
    {
        ImVec2 WinPos = ImGui::GetWindowPos();
        ImVec2 WinSize = ImGui::GetWindowSize();
        ImDrawList* DrawList = ImGui::GetWindowDrawList();

        DrawList->AddRectFilled(WinPos, WinPos + WinSize, Menu::Bg);
        DrawList->AddRect(WinPos + ImVec2(1.0f, 1.0f), WinPos + WinSize - ImVec2(1.0f, 1.0f), Menu::Outline);
        DrawList->AddRect(WinPos + ImVec2(2.0f, 2.0f), WinPos + WinSize - ImVec2(2.0f, 2.0f), Menu::Accent);

        float Top = HeaderHeight + Style.WindowBorderSize - 8.0f;
        float Bottom = Style.WindowBorderSize + Style.WindowPadding.y + 1.0f;

        ImVec2 ContentMin = WinPos + ImVec2(Style.WindowBorderSize * 2.0f + Style.WindowPadding.x, Top + Style.WindowPadding.y + 6.0f);
        ImVec2 ContentMax = WinPos + ImVec2(WinSize.x - Style.WindowBorderSize * 2.0f - Style.WindowPadding.x, WinSize.y - Bottom);

        DrawList->AddRectFilled(ContentMin, ContentMax, Menu::InnerBg);
        DrawList->AddRect(ContentMin - ImVec2(1.0f, 1.0f), ContentMax + ImVec2(1.0f, 1.0f), Menu::Outline);
        DrawList->AddRect(ContentMin - ImVec2(2.0f, 2.0f), ContentMax + ImVec2(2.0f, 2.0f), Menu::DarkAccent);

        float TextX = WinPos.x + Style.WindowBorderSize + 8.0f;
        float TextY = WinPos.y + (Top - ImGui::CalcTextSize("Fang.wtf").y) * 0.5f + 8.0f;

        ImVec2 TextPos(TextX, TextY);

        Menu::DrawLabelShadow(DrawList, TextPos, Menu::Accent, "Fang");
        TextPos.x += ImGui::CalcTextSize("Fang").x;
        Menu::DrawLabelShadow(DrawList, TextPos, Menu::Text, " External");

        static int Section = 0;
        const char* TabLabels[] = { "Aimbot", "Visuals", "Rage", "Players", "Explorer", "Settings" };
        const int TabCount = IM_ARRAYSIZE(TabLabels);

        Menu::Tabs(DrawList, WinPos, TextY, Section, TabLabels, TabCount, 10.0f, 8.0f);

        ImGui::SetCursorScreenPos(ContentMin);
        ImGui::PushClipRect(ContentMin, ContentMax, true);
        ImGui::BeginGroup();

        if (Section == 0)
        {
            ImVec2 AvailSize(ContentMax.x - ContentMin.x, ContentMax.y - ContentMin.y);
            float Spacing = Style.ItemSpacing.x;

            float LeftWidth = (AvailSize.x - Spacing) * 0.5f;
            float RightWidth = AvailSize.x - Spacing - LeftWidth;

            float SidePad = 4.0f;
            float HalfHeight = (AvailSize.y - SidePad * 2.0f) * 0.5f;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + SidePad);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + SidePad);

            if (Menu::BeginChild("Aimbot", ImVec2(LeftWidth - SidePad, HalfHeight)))
            {
                Menu::CheckBox("Enabled", &Globals::Aimbot::Enabled);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x + 12.0f);
                Menu::KeyBindEx("Aimbot Key", &Globals::Aimbot::Aimbot_Key, &Globals::Aimbot::Aimbot_Mode);

                Menu::CheckBox("Sticky aim", &Globals::Aimbot::AimbotSticky);
                Menu::CheckBox("Knocked check", &Globals::Aimbot::KnockedCheck);

                Menu::Combo("Aimbot type", &Globals::Aimbot::Aimbot_type, { "Mouse", "Camera" });

                Menu::Combo("HitPart", &Globals::Aimbot::HitPart, { "Head", "Torso", "LowerTorso" });

                if (Globals::Aimbot::Aimbot_type == 0)
                {

                    Menu::SliderFloat("Mouse smoothing X", &Globals::Aimbot::Mouse::Smoothing_X, 0.0f, 12.0f);
                    Menu::SliderFloat("Mouse smoothing Y", &Globals::Aimbot::Mouse::Smoothing_Y, 0.0f, 12.0f);

                    Menu::SliderFloat("Mouse sensitivty", &Globals::Aimbot::Mouse::Mouse_Sensitivty, 0.0f, 5.0f);
                }
                else if (Globals::Aimbot::Aimbot_type == 1)
                {
                    Menu::SliderFloat("Camera smoothing X", &Globals::Aimbot::Camera::Smoothing_X, 0.0f, 12.0f);
                    Menu::SliderFloat("Camera smoothing Y", &Globals::Aimbot::Camera::Smoothing_Y, 0.0f, 12.0f);
                }
                Menu::EndChild();
            }

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + SidePad);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + SidePad);
            float Bottom = HalfHeight - 8;
            if (Menu::BeginChild("Aimbot FOV", ImVec2(LeftWidth - SidePad, Bottom)))
            {
                if (Globals::Aimbot::Enabled)
                {
                    Menu::CheckBox("Draw", &Globals::Aimbot::DrawFov);
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                    Menu::ColorEdit4("FOV Color", Globals::Aimbot::FovColor);
                    if (Globals::Aimbot::DrawFov)
                    {
                        Menu::SliderFloat("Size", &Globals::Aimbot::FovSize, 1.0f, 500.0f);
                        Menu::CheckBox("Spin", &Globals::Aimbot::FovSpin);
                        if (Globals::Aimbot::FovSpin)
                        {
                            Menu::SliderInt("Speed", &Globals::Aimbot::FovSpinSpeed, 1, 5);
						}
                        Menu::CheckBox("Use FOV", &Globals::Aimbot::useFov);
                    }
                }
                Menu::EndChild();
            }

            ImGui::SameLine(0.0f, Spacing);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 248);
            if (Menu::BeginChild("Silent", ImVec2(RightWidth - SidePad, HalfHeight)))
            {
                Menu::CheckBox("Enabled", &Globals::Silent::Enabled);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x + 12.0f);
                Menu::KeyBindEx("Silent Key", &Globals::Silent::Silent_Key, &Globals::Silent::Silent_Mode);

                Menu::CheckBox("Sticky aim", &Globals::Silent::StickyAim);
                Menu::CheckBox("Knocked check", &Globals::Silent::KnockedCheck);

                Menu::Combo("Silent Part", &Globals::Silent::AimPart, { "Head", "Torso", "LowerTorso" });

                Menu::Combo("Method", &Globals::Silent::Method, { "Viewport", "Offset", "Raycast", "Magic Bullet" });

                Menu::EndChild();
            }

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 236);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + SidePad + 240.0f);
            if (Menu::BeginChild("Silent FOV", ImVec2(RightWidth - SidePad, Bottom)))
            {
                if (Globals::Silent::Enabled)
                {
                    Menu::CheckBox("Draw", &Globals::Silent::DrawFov);
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                    Menu::ColorEdit4("FOV Color", Globals::Silent::FovColor);

                    Menu::CheckBox("Gun based FOV", &Globals::Silent::GunBasedFov);

                    if (Globals::Silent::GunBasedFov)
                    {
                        Menu::SliderFloat("Default", &Globals::Silent::Fov, 0.0f, 300.0f);
                        Menu::SliderFloat("Double Barrel", &Globals::Silent::FovDoubleBarrel, 0.0f, 300.0f);
                        Menu::SliderFloat("Tactical", &Globals::Silent::FovTacticalShotgun, 0.0f, 300.0f);
                        Menu::SliderFloat("Revolver", &Globals::Silent::FovRevolver, 0.0f, 300.0f);
                    }
                    else
                    {
                        Menu::SliderFloat("Static FOV", &Globals::Silent::Fov, 0.0f, 500.0f);
                    }

                    Menu::CheckBox("Spin", &Globals::Silent::FovSpin);
                    if (Globals::Silent::FovSpin)
                    {
                        Menu::SliderInt("Spin Speed", &Globals::Silent::FovSpinSpeed, 1, 5);
                    }

                    Menu::CheckBox("Use FOV", &Globals::Silent::UseFov);
                }

                Menu::EndChild();
            }
        }

        if (Section == 1)
        {
            ImVec2 AvailSize(ContentMax.x - ContentMin.x, ContentMax.y - ContentMin.y);
            float Spacing = Style.ItemSpacing.x;

            float LeftWidth = (AvailSize.x - Spacing) * 0.5f;
            float RightWidth = AvailSize.x - Spacing - LeftWidth;

            float SidePad = 4.0f;
            float HalfHeight = (AvailSize.y - SidePad * 2.0f) * 0.5f;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + SidePad);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + SidePad);

            if (Menu::BeginChild("Visuals", ImVec2(LeftWidth - SidePad, HalfHeight)))
            {
                Menu::CheckBox("Enabled", &Globals::Visuals::Enabled);
                Menu::CheckBox("Box", &Globals::Visuals::Box);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                Menu::ColorEdit4("Box Color", Globals::Visuals::Colors::Box);

                if (Globals::Visuals::Box)
                {
                    Menu::CheckBox("Box Fill", &Globals::Visuals::Box_Fill);
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                    Menu::ColorEdit4("Box Fill Top", Globals::Visuals::Colors::BoxFill_Top);
                    if (Globals::Visuals::Box_Fill_Gradient)
                    {
                        ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() - 24);
                        Menu::ColorEdit4("Box Fill Bottom", Globals::Visuals::Colors::BoxFill_Bottom);
                    }
                }

                Menu::CheckBox("Healthbar", &Globals::Visuals::Healthbar);
                if (Globals::Visuals::Healthbar_Type == 0)
                {
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                    Menu::ColorEdit4("Healthbar Color", Globals::Visuals::Colors::Healthbar);
                }
                else if (Globals::Visuals::Healthbar_Type == 1)
                {
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                    Menu::ColorEdit4("Healthbar Top", Globals::Visuals::Colors::Healthbar_Top);
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() - 24);
                    Menu::ColorEdit4("Healthbar Middle", Globals::Visuals::Colors::Healthbar_Middle);
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() - 51);
                    Menu::ColorEdit4("Healthbar Bottom", Globals::Visuals::Colors::Healthbar_Bottom);
                }

                Menu::CheckBox("Health", &Globals::Visuals::Health);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                Menu::ColorEdit4("Health Color", Globals::Visuals::Colors::Health);

                Menu::CheckBox("Name", &Globals::Visuals::Name);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                Menu::ColorEdit4("Name Color", Globals::Visuals::Colors::Name);

                Menu::CheckBox("Distance", &Globals::Visuals::Distance);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                Menu::ColorEdit4("Distance Color", Globals::Visuals::Colors::Distance);

                Menu::CheckBox("Rig Type", &Globals::Visuals::Rig_Type);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                Menu::ColorEdit4("Rig Type Color", Globals::Visuals::Colors::Rig_Type);

                Menu::CheckBox("Tool", &Globals::Visuals::Tool);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                Menu::ColorEdit4("Tool Color", Globals::Visuals::Colors::Tool);

                Menu::CheckBox("Skeleton", &Globals::Visuals::Skeleton);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                Menu::ColorEdit4("Skeleton Color", Globals::Visuals::Colors::Skeleton);

                Menu::CheckBox("Chams", &Globals::Visuals::Chams);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                Menu::ColorEdit4("Chams Color", Globals::Visuals::Colors::Chams);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() - 24);
                Menu::ColorEdit4("Chams Outline", Globals::Visuals::Colors::ChamsOutline);

                Menu::EndChild();
            }

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + SidePad);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + SidePad);
            float Bottom = HalfHeight - 8;
            if (Menu::BeginChild("World", ImVec2(LeftWidth - SidePad, Bottom)))
            {
                Menu::CheckBox("Skybox Changer", &Globals::World::Skybox);
                if (Globals::World::Skybox)
                {
                    Menu::Combo("Skybox Type", &Globals::World::Skybox_Type, {
                        "Fang.wtf", "Space", "Pink Sky", "Minecraft", "Night Cloudy",
                        "Sparkling Night", "Winterness", "Dark Crimson", "Nebula",
                        "Tropical", "Green Sky"
                        });

                    Menu::CheckBox("Skybox Rotation", &Globals::World::Rotate);
                    if (Globals::World::Rotate)
                    {
                        Menu::SliderFloat("Rotation Speed", &Globals::World::Skybox_Rotate_Speed, 0.0f, 5.0f);
                    }
                }

                Menu::CheckBox("Atmosphere", &Globals::World::Ambience);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                Menu::ColorEdit4("Atmosphere Color", Globals::World::Colors::Ambience);

                Menu::CheckBox("Fog", &Globals::World::Fog);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - Menu::GetColorPickerWidth() + Style.ChildPadding.x - 1.0f);
                Menu::ColorEdit4("Fog Color", Globals::World::Colors::Fog);
                if (Globals::World::Fog)
                {
                    Menu::SliderFloat("Fog Distance", &Globals::World::Fog_Distance, 0.0f, 1000.0f);
                }

                Menu::CheckBox("Brightness", &Globals::World::Brightness);
                if (Globals::World::Brightness)
                {
                    Menu::SliderFloat("Brightness Value", &Globals::World::BrightnessI, 0.0f, 10.0f);
                }

                Menu::CheckBox("Exposure", &Globals::World::Exposure);
                if (Globals::World::Exposure)
                {
                    Menu::SliderFloat("Exposure Value", &Globals::World::ExposureI, -3.0f, 3.0f);
                }

                Menu::CheckBox("FOV", &Globals::World::FOV);
                if (Globals::World::FOV)
                {
                    Menu::SliderFloat("FOV Value", &Globals::World::FOV_Distance, 70.0f, 120.0f);
                }

                Menu::EndChild();
            }

            ImGui::SameLine(0.0f, Spacing);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 248);
            if (Menu::BeginChild("Options", ImVec2(RightWidth - SidePad, AvailSize.y - SidePad * 2.0f)))
            {
                Menu::CheckBox("Exclude Client", &Globals::Settings::Client_Check);
                Menu::CheckBox("Exclude Team", &Globals::Settings::Team_Check);

                Menu::SliderFloat("Render Distance", &Globals::Visuals::Render_Distance, 0.0f, 500.0f);

                if (Globals::Visuals::Healthbar)
                {
                    Menu::Combo("Healthbar Style", &Globals::Visuals::Healthbar_Type, { "Static", "Gradient" });
                    Menu::SliderInt("Healthbar Gap", &Globals::Visuals::Gap, 1, 5);
                    Menu::SliderInt("Healthbar Thickness", &Globals::Visuals::Thickness, 1, 5);
                }

                if (Globals::Visuals::Box)
                {
                    Menu::Combo("Box Style", &Globals::Visuals::Box_Type, { "Bounding", "Corner" });
                }

                if (Globals::Visuals::Box_Fill)
                {
                    Menu::CheckBox("Fill Gradient", &Globals::Visuals::Box_Fill_Gradient);

                    if (Globals::Visuals::Box_Fill_Gradient)
                    {
                        Menu::CheckBox("Fill Rotation", &Globals::Visuals::Box_Fill_Gradient_Rotate);
                    }

                    if (Globals::Visuals::Box_Fill_Gradient_Rotate)
                    {
                        Menu::Combo("Rotation Type", &Globals::Visuals::Box_Fill_Type, { "Side", "Bottom", "Spin" });
                        Menu::SliderInt("Rotation Speed", &Globals::Visuals::BoxFillSpeed, 1, 5);
                    }
                }

                if (Globals::Visuals::Name)
                {
                    Menu::Combo("Name Display", &Globals::Visuals::Name_Type, { "Name", "Display Name", "Name & Display Name" });
                }

                if (Globals::Visuals::Chams)
                {
                    Menu::CheckBox("Chams Fade", &Globals::Visuals::ChamsFade);

                    if (Globals::Visuals::ChamsFade)
                    {
                        Menu::SliderInt("Fade Speed", &Globals::Visuals::ChamsFadeSpeed, 1, 5);
                    }
                }

                Menu::EndChild();
            }
        }
        if (Section == 2) {}
        if (Section == 3) {}
        if (Section == 4) {}
        if (Section == 5)
        {
            ImVec2 AvailSize(ContentMax.x - ContentMin.x, ContentMax.y - ContentMin.y);
            float Spacing = Style.ItemSpacing.x;

            float LeftWidth = (AvailSize.x - Spacing) * 0.5f;
            float RightWidth = AvailSize.x - Spacing - LeftWidth;

            float SidePad = 4.0f;
            float HalfHeight = (AvailSize.y - SidePad * 2.0f) * 0.5f;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + SidePad);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + SidePad);

            if (Menu::BeginChild("Settings", ImVec2(RightWidth - SidePad, HalfHeight)))
            {
                Menu::CheckBox("Streamproof", &Globals::Settings::Streamproof);
                Menu::Combo("Performance Type", &Globals::Settings::Performance_Mode, { "Low", "Medium", "High" });

                Menu::EndChild();
            }
        }

        ImGui::EndGroup();
        ImGui::PopClipRect();
    }

    ImGui::End();
}

void Graphics::Render_Visuals()
{
    Visuals::RunService();

}
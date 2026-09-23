#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <memory>
#include <d3d11.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/backends/imgui_impl_dx11.h>

#include <string>
#include <vector> 

inline ImFont* Tahoma_BoldXP = nullptr;

struct detail_t {
	HWND Window = nullptr;
	WNDCLASSEX WindowClass = {};
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;
	ID3D11RenderTargetView* GraphicsTargetView = nullptr;
	IDXGISwapChain* SwapChain = nullptr;
};

class Graphics {
public:
    Graphics();
    ~Graphics();

    bool Running = false;

    void Start_Render();
    void Render_Menu();
    void Render_Visuals();
    void End_Render();

    bool Create_Device();
    bool Create_Window();
    bool Create_Imgui();

    std::unique_ptr<detail_t> Detail = std::make_unique<detail_t>();

private:
    void Destroy_Device();
    void Destroy_Window();
    void Destroy_Imgui();
};

inline std::unique_ptr<Graphics> Graphic = std::make_unique<Graphics>();
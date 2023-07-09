#include "main.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    core::Audio::AudioEngine* pAE = new core::Audio::AudioEngine;
    core::Graphics::GraphicsEngine* pGE = new core::Graphics::GraphicsEngine;
    CGame* pGame;

    auto wndProc = [&](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        pGame->wndProc(hWnd, uMsg, wParam, lParam);

        switch (uMsg) {
        case WM_PAINT:
            pGE->render();
            return 0;
        case WM_ERASEBKGND:
            return 0;
        case WM_DESTROY:
            PostQuitMessage(EXIT_SUCCESS);
            return 0;
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    };
    core::Window window{hInst, "Snake game", "snake_game_main", 512, 512, cmdshow, wndProc};
    
    pGE->onInitDevice.connect([&](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("fonts/vga-8x16.ttf", 32, nullptr, io.Fonts->GetGlyphRangesCyrillic());
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        ImGui::StyleColorsDark();
        auto& style = ImGui::GetStyle();
        style.WindowBorderSize = 0;
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0.45f);
        ImGui_ImplWin32_Init(window.getHWND());
        ImGui_ImplDX11_Init(pDevice, pDeviceContext);
        });

    pGame = new CGame(*pGE, pAE);
    pGE->init(window, 520.f, 520.f);

	auto exitCode = window.run();
    delete pGame;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    delete pGE;
    delete pAE;
	CoUninitialize();
	return exitCode;
}

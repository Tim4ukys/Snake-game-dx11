#include "main.hpp"
#include <directxmath/DirectXMath.h>
#include <directxmath/DirectXColors.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    core::Graphics::GraphicsEngine ge{};

    auto wndProc = [&](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        switch (uMsg) {
        case WM_PAINT:
            ge.render();
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

    core::Graphics::Box* box;
    
    ge.onInitDevice.connect([&box, &ge](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        box = new core::Graphics::Box(&ge, 10.f, 10.f, 512.f - 20.f, 512.f - 20.f, 10.f, DirectX::Colors::Red);
        });
    ge.onRender.connect([&box, &ge](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        ge.setShaders();

        box->draw();
        });
    ge.init(window, 512.f, 512.f);

	auto exitCode = window.run();

    delete box;
	CoUninitialize();
	return exitCode;
}

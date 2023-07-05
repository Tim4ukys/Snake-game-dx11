#include "main.hpp"
#include <directxmath/DirectXMath.h>
#include <directxmath/DirectXColors.h>

core::CUnkown<core::Graphics::GraphicsEngine>* m_pGE;

LRESULT __stdcall wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT:
        (*m_pGE)->render();
        return 0;
    case WM_ERASEBKGND:
        return 0;
    case WM_DESTROY:
        PostQuitMessage(EXIT_SUCCESS);
        return 0;
    }

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	core::Window window{hInst, "Snake game", "snake_game_main", 512, 512, cmdshow, &wndProc};
    m_pGE = new core::CUnkown<core::Graphics::GraphicsEngine>(window, 512, 512);

    core::Graphics::SolidBox* solid1;
    core::Graphics::SolidBox* solid2;
    
    (*m_pGE)->onInitDevice.connect([&solid1, &solid2](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        solid1 = new core::Graphics::SolidBox(*m_pGE, 0.f, 0.f, 25.f, 25.f, DirectX::Colors::Red);
        solid2 = new core::Graphics::SolidBox(*m_pGE, 0.f, 0.f, 25.f, 25.f, DirectX::Colors::DarkBlue);
        });
    (*m_pGE)->onRender.connect([&solid1, &solid2](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        (*m_pGE)->setShaders();

        static int pos{};
        static int cock{1};
        pos += cock;
        if (pos >= (512 - 25) || pos <= 0) {
            cock *= -1;
        }
        solid1->setPos(float(pos), float(pos));
        solid1->draw();

        solid2->setPos(0.0f, float(pos));
        solid2->draw();
        });
    (*m_pGE)->init();

	auto exitCode = window.run();

    delete solid1;
    delete solid2;
    delete m_pGE;
	CoUninitialize();
	return exitCode;
}

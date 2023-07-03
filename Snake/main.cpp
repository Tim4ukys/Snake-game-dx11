#include "main.hpp"
#include <directxmath/DirectXMath.h>
#include <directxmath/DirectXColors.h>

core::Graphics::GraphicsEngine* m_pGE{};

LRESULT __stdcall wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT:
        m_pGE->render();
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
    m_pGE = new core::Graphics::GraphicsEngine(window, 512, 512);

    ID3D11Buffer* vertBuff;
    struct SimpleVertex
    {
        DirectX::XMFLOAT3 m_pos;
        DirectX::XMFLOAT4 m_color;
    };
    m_pGE->onInitDevice.connect([&vertBuff](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        
        SimpleVertex vertices[] =
        {
            {DirectX::XMFLOAT3(512.f - 25.f, 25.f, 0.5f), DirectX::XMFLOAT4(DirectX::Colors::Red)},
            {DirectX::XMFLOAT3(512.f, 25.f, 0.5f), DirectX::XMFLOAT4(DirectX::Colors::Green)},
            {DirectX::XMFLOAT3(512.f - 25.f, 0.f, 0.5f), DirectX::XMFLOAT4(DirectX::Colors::Blue)},
            {DirectX::XMFLOAT3(512.f, 0.f, 0.5f), DirectX::XMFLOAT4(DirectX::Colors::SkyBlue)},
        };

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = sizeof(vertices);
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA Data;
        ZeroMemory(&Data, sizeof(Data));
        Data.pSysMem = vertices;

        pDevice->CreateBuffer(&bd, &Data, &vertBuff);
        });
    m_pGE->onRender.connect([&vertBuff](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        m_pGE->setShaders();

        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        pDeviceContext->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);
        pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        pDeviceContext->Draw(4, 0);
        });
    m_pGE->init();

	auto exitCode = window.run();

    delete m_pGE;
	CoUninitialize();
	return exitCode;
}

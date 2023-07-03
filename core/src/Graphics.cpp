#include "Graphics.hpp"
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <directxmath/DirectXMath.h>
#include <directxmath/DirectXColors.h>

core::Graphics::GraphicsEngine::GraphicsEngine(const Window& window, float widthX, float heightY)
{
    auto [screenWidth, screenHeight] = window.getScreenSize();

    constexpr auto deviceFlags = D3D11_CREATE_DEVICE_DEBUG;
    DXGI_SWAP_CHAIN_DESC swapChainDesc{};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = window.getHWND();
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    /*
    D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags, NULL, NULL,
        D3D11_SDK_VERSION, &m_pDevice, NULL, &m_pDeviceContext);
    UINT qualMultSample;
    if (SUCCEEDED(m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &qualMultSample))) {
        swapChainDesc.SampleDesc.Count = 4;
        swapChainDesc.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
    }

    Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;
    {
        Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
        if (SUCCEEDED(m_pDevice.As(&dxgiDevice)))
        {
            Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
            if (SUCCEEDED(dxgiDevice->GetAdapter(&adapter)))
            {
                adapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
            }
        }

    }
    dxgiFactory->CreateSwapChain(m_pDevice.Get(), &swapChainDesc, &m_pSwapChain);*/

    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags, NULL, NULL, 
        D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);

    ID3D11Texture2D* pBackBuffer;
    m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&pBackBuffer);
    m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
    pBackBuffer->Release();
    m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), NULL);

    D3D11_VIEWPORT vp;
    vp.Width = screenWidth;
    vp.Height = screenHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pDeviceContext->RSSetViewports(1, &vp);

    ID3DBlob* pBuff;
    compilingShader("shader.fx", "VS", "vs_4_0", &pBuff);
    m_pDevice->CreateVertexShader(pBuff->GetBufferPointer(), pBuff->GetBufferSize(), NULL, &m_pVertexShader);
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    constexpr UINT numElements = ARRAYSIZE(layout);
    m_pDevice->CreateInputLayout(layout, numElements, pBuff->GetBufferPointer(), pBuff->GetBufferSize(), &m_pInputLayout);
    pBuff->Release();
    m_pDeviceContext->IASetInputLayout(m_pInputLayout.Get());

    compilingShader("shader.fx", "PS", "ps_4_0", &pBuff);
    m_pDevice->CreatePixelShader(pBuff->GetBufferPointer(), pBuff->GetBufferSize(), NULL, &m_pPixelShader);
    pBuff->Release();

    struct stConstBuffer {
        DirectX::XMMATRIX scaleScreen = DirectX::XMMatrixIdentity();
    } constBuffer;
    constBuffer.scaleScreen.r[0].m128_f32[0] = 2.f / widthX;
    constBuffer.scaleScreen.r[0].m128_f32[3] = -1.f;
    constBuffer.scaleScreen.r[1].m128_f32[1] = 2.f / heightY;
    constBuffer.scaleScreen.r[1].m128_f32[3] = -1.f;

    D3D11_BUFFER_DESC dsConstBuffer;
    dsConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    dsConstBuffer.Usage = D3D11_USAGE_IMMUTABLE;
    dsConstBuffer.CPUAccessFlags = 0;
    dsConstBuffer.MiscFlags = 0;
    dsConstBuffer.ByteWidth = sizeof(stConstBuffer);
    dsConstBuffer.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA subRes;
    subRes.pSysMem = &constBuffer;
    m_pDevice->CreateBuffer(&dsConstBuffer, &subRes, &m_pConstBuffer);
    m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pConstBuffer.GetAddressOf());
}

core::Graphics::GraphicsEngine::~GraphicsEngine()
{
    m_pDeviceContext->ClearState();
    m_pRenderTargetView.Reset();
    m_pSwapChain.Reset();
    m_pDevice.Reset();
}

void core::Graphics::GraphicsEngine::render()
{
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), DirectX::Colors::Black);
    onRender(m_pDevice.Get(), m_pDeviceContext.Get());
    m_pSwapChain->Present(1, 0);
}

void core::Graphics::GraphicsEngine::compilingShader(std::string_view fileName, std::string_view entryPoint, std::string_view shaderModel, ID3DBlob** ppBlobOut)
{
    ID3DBlob* pErrorBlob{};
    const auto hr = D3DX11CompileFromFileA(fileName.data(), NULL, NULL, entryPoint.data(), shaderModel.data(),
        D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
    if (FAILED(hr)) {
        OutputDebugStringA((PCHAR)pErrorBlob->GetBufferPointer());
    }
    if (pErrorBlob) pErrorBlob->Release();
}

void core::Graphics::GraphicsEngine::setShaders()
{
    m_pDeviceContext->VSSetShader(m_pVertexShader.Get(), 0, 0);
    m_pDeviceContext->PSSetShader(m_pPixelShader.Get(), 0, 0);
}
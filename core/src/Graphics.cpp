#include "Graphics.hpp"
#include <D3DX11.h>
#include <D3Dcompiler.h>

core::Graphics::Box::Box(GraphicsEngine* pGE, float x, float y, float width, float height, float sizeLine, DirectX::XMVECTORF32 color)
{
    m_pGE = pGE;
    m_Pos.x = x;
    m_Pos.y = y;

    stVertex vertices[]{
        {DirectX::XMFLOAT3(0.0f, 0.f, 0.5f), color},
        {DirectX::XMFLOAT3(sizeLine, sizeLine, 0.5f), color},
        {DirectX::XMFLOAT3(width, 0.0f, 0.5f), color},
        {DirectX::XMFLOAT3(width - sizeLine, sizeLine, 0.5f), color},
        {DirectX::XMFLOAT3(width, height, 0.5f), color},
        {DirectX::XMFLOAT3(width - sizeLine, height - sizeLine, 0.5f), color},
        {DirectX::XMFLOAT3(0, height, 0.5f), color},
        {DirectX::XMFLOAT3(sizeLine, height - sizeLine, 0.5f), color},
        {DirectX::XMFLOAT3(0.0f, 0.f, 0.5f), color},
        {DirectX::XMFLOAT3(sizeLine, sizeLine, 0.5f), color},
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

    m_pGE->m_pDevice->CreateBuffer(&bd, &Data, &m_pVB);
}

void core::Graphics::Box::draw()
{
    applyPos();

    UINT stride = sizeof(stVertex);
    UINT offset = 0;
    m_pGE->m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVB.GetAddressOf(), &stride, &offset);
    m_pGE->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_pGE->m_pDeviceContext->Draw(10, 0);
}

core::Graphics::SolidRect::SolidRect(GraphicsEngine* pGE, float x, float y, float width, float height, DirectX::XMVECTORF32 color)
{
    m_pGE = pGE;
    m_Pos.x = x;
    m_Pos.y = y;

    stVertex vertices[]{
        {DirectX::XMFLOAT3(0.0f, 0.f, 0.5f), color},
        {DirectX::XMFLOAT3(0.0f, height, 0.5f), color},
        {DirectX::XMFLOAT3(width, 0.0f, 0.5f), color},
        {DirectX::XMFLOAT3(width, height, 0.5f), color},
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

    m_pGE->m_pDevice->CreateBuffer(&bd, &Data, &m_pVB);
}

void core::Graphics::SolidRect::draw()
{
    applyPos();

    UINT stride = sizeof(stVertex);
    UINT offset = 0;
    m_pGE->m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVB.GetAddressOf(), &stride, &offset);
    m_pGE->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_pGE->m_pDeviceContext->Draw(4, 0);
}

void core::Graphics::GraphicsEngine::init(const Window& window, float widthX, float heightY)
{
    auto [screenWidth, screenHeight] = window.getClientScreenSize();

#ifndef NDEBUG
    constexpr auto deviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
    constexpr UINT deviceFlags = 0;
#endif
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

    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags, NULL, NULL, 
        D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);

    ID3D11Texture2D* pBackBuffer;
    m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&pBackBuffer);
    m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
    pBackBuffer->Release();
    m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), NULL);

    D3D11_VIEWPORT vp;
    vp.Width = static_cast<float>(screenWidth);
    vp.Height = static_cast<float>(screenHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pDeviceContext->RSSetViewports(1, &vp);

    ID3DBlob* pBuff;
    compilingShader("shaders/shader.fx", "VS", "vs_4_0", &pBuff);
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

    compilingShader("shaders/shader.fx", "PS", "ps_4_0", &pBuff);
    m_pDevice->CreatePixelShader(pBuff->GetBufferPointer(), pBuff->GetBufferSize(), NULL, &m_pPixelShader);
    pBuff->Release();

    m_constBuffer.scaleScreen.r[0].m128_f32[0] = 2.f / widthX;
    m_constBuffer.scaleScreen.r[0].m128_f32[3] = -1.f;
    m_constBuffer.scaleScreen.r[1].m128_f32[1] = 2.f / heightY;
    m_constBuffer.scaleScreen.r[1].m128_f32[3] = -1.f;

    D3D11_BUFFER_DESC dsConstBuffer;
    dsConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    dsConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
    dsConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    dsConstBuffer.MiscFlags = 0;
    dsConstBuffer.ByteWidth = sizeof(stConstBuffer);
    dsConstBuffer.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA subRes;
    subRes.pSysMem = &m_constBuffer;
    m_pDevice->CreateBuffer(&dsConstBuffer, &subRes, &m_pConstBuffer);
    m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pConstBuffer.GetAddressOf());

    onInitDevice(m_pDevice.Get(), m_pDeviceContext.Get());
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

void core::Graphics::GraphicsEngine::applyConstBuffer()
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    m_pDeviceContext->Map(m_pConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &m_constBuffer, sizeof(stConstBuffer));
    m_pDeviceContext->Unmap(m_pConstBuffer.Get(), 0);
}

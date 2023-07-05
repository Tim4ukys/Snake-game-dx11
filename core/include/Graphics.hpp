#ifndef __CORE_GRAPHICS_HPP__
#define __CORE_GRAPHICS_HPP__

#include <string_view>
#include <Window.hpp>
#include <wrl/client.h>
#include <d3d11.h>
#include <directxmath/DirectXMath.h>
#include <directxmath/DirectXColors.h>
#include <boost/signals2.hpp>
#include <core_engine.hpp>

namespace core {
    namespace Graphics {
        struct stVertex {
            DirectX::XMFLOAT3 m_pos;
            DirectX::XMFLOAT4 m_color;

            stVertex(const DirectX::XMFLOAT3& pos, const DirectX::XMVECTORF32& color)
                : m_pos(pos), m_color(color)
            {}
        };

        class GraphicsEngine {
            Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;
            Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
            Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
            Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;

            struct stConstBuffer {
                DirectX::XMMATRIX scaleScreen = DirectX::XMMatrixIdentity();
                DirectX::XMMATRIX pos = DirectX::XMMatrixIdentity();
            } m_constBuffer;
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstBuffer;
            void applyConstBuffer();

            void compilingShader(std::string_view fileName, std::string_view entryPoint, std::string_view shaderModel, ID3DBlob** ppBlobOut);

        public:
            friend class SolidBox;

            explicit GraphicsEngine(const Window& window, float widthX, float heightY);
            ~GraphicsEngine();

            void setShaders();
            void render();
            inline void init() {
                onInitDevice(m_pDevice.Get(), m_pDeviceContext.Get());
            }

            boost::signals2::signal<void(ID3D11Device*, ID3D11DeviceContext*)> onInitDevice;
            boost::signals2::signal<void(ID3D11Device*, ID3D11DeviceContext*)> onRender;
        };

        class SolidBox {
            CUnkown<GraphicsEngine> m_pGE;
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVB;
            DirectX::XMFLOAT2 m_Pos;

        public:
            explicit SolidBox(CUnkown<GraphicsEngine>& ge, float x, float y, float width, float height, DirectX::XMVECTORF32 color);

            inline void setPos(float x, float y) {
                m_Pos.x = x;
                m_Pos.y = y;
            }
            void draw();
        };
    }
}

#endif
#ifndef __CORE_GRAPHICS_HPP__
#define __CORE_GRAPHICS_HPP__

#include <string_view>
#include <Window.hpp>
#include <wrl/client.h>
#include <d3d11.h>
#include <directxmath/DirectXMath.h>
#include <directxmath/DirectXColors.h>
#include <boost/signals2.hpp>

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
            friend class Primitive;
            friend class SolidRect;
            friend class Box;

            GraphicsEngine() = default;
            ~GraphicsEngine();

            void setShaders();
            void render();
            void init(const Window& window, float widthX, float heightY);

            boost::signals2::signal<void(ID3D11Device*, ID3D11DeviceContext*)> onInitDevice;
            boost::signals2::signal<void(ID3D11Device*, ID3D11DeviceContext*)> onRender;
        };

        class Primitive {
        protected:
            GraphicsEngine* m_pGE;
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVB;
            DirectX::XMFLOAT2 m_Pos;

            inline void applyPos() {
                m_pGE->m_constBuffer.pos.r[0].m128_f32[3] = m_Pos.x;
                m_pGE->m_constBuffer.pos.r[1].m128_f32[3] = m_Pos.y;
                m_pGE->applyConstBuffer();
            }
        public:
            inline void setPos(float x, float y) {
                m_Pos.x = x;
                m_Pos.y = y;
            }
        };

        class SolidRect : public Primitive {
        public:
            explicit SolidRect(GraphicsEngine* pGE, float x, float y, float width, float height, DirectX::XMVECTORF32 color);

            void draw();
        };

        class Box : public Primitive {
        public:
            explicit Box(GraphicsEngine* pGE, float x, float y, float width, float height, float sizeLine, DirectX::XMVECTORF32 color);

            void draw();
        };
    }
}

#endif
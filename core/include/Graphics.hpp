#ifndef __CORE_GRAPHICS_HPP__
#define __CORE_GRAPHICS_HPP__

#include <string_view>
#include <Window.hpp>
#include <wrl/client.h>
#include <d3d11.h>
#include <boost/signals2.hpp>

namespace core {
    namespace Graphics {

        /*class SolidBox {
        private:
            explicit SolidBox(ID3D11Device* pDevice, float x, float y, float width, float height);
            ~SolidBox();

            void draw();
        };*/

        class GraphicsEngine {
            Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;
            Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
            Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
            Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstBuffer;

            void compilingShader(std::string_view fileName, std::string_view entryPoint, std::string_view shaderModel, ID3DBlob** ppBlobOut);

        public:
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
    }
}

#endif
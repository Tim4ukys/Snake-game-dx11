#include "Game.hpp"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <thread>

CGame::CGame(core::Graphics::GraphicsEngine& ge, core::Audio::AudioEngine* pAE)
    : m_AudioMan(pAE)
{
    ge.onInitDevice.connect([&](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        m_pBox = std::make_unique<core::Graphics::Box>(&ge, 0.f, 0.f, 520.f, 520.f, 10.f, DirectX::Colors::Gray);

        });
    ge.onRender.connect([&](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        m_AudioMan.proc();
        ge.setShaders();

        if (m_gameState != GAME_STATE::FirstStart)
            m_pBox->draw();

        renderGUI();
        });
}
CGame::~CGame()
{

}

void CGame::renderGUI() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (m_gameState == GAME_STATE::FirstStart || m_gameState == GAME_STATE::Paused) {
        ImGui::SetNextWindowPos({ 0.0f, 0.0f });
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("main_menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

        if (buttonDraw(m_gameState == GAME_STATE::FirstStart ? "start" : "continue", 1, 2))
            m_gameState = GAME_STATE::Play;
        if (buttonDraw("exit", 2, 2))
            PostQuitMessage(EXIT_SUCCESS);

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool CGame::buttonDraw(std::string_view text, size_t i, size_t count)
{
    auto size = ImGui::CalcTextSize(text.data());
    auto pos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - size.x / 2, (ImGui::GetIO().DisplaySize.y / (count + 1)) * i - size.y / 2);

    ImGui::SetCursorPos(pos);
    bool res = ImGui::InvisibleButton(text.data(), size);
    bool hov = ImGui::IsItemHovered();

    ImGui::SetCursorPos(pos);
    ImGui::TextColored(hov ? COLOR_MENU_HOV : COLOR_MENU_UNHOV, text.data());

    if (hov && !m_btnOldHov[text]) {
        m_btnOldHov[text] = true;
        m_AudioMan.playFile("menu_select");
    }
    else if (res) {
        m_AudioMan.playFile("menu_direct");
    }
    else if (!hov) {
        m_btnOldHov[text] = false;
    }
    return res;
}

void CGame::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_KEYDOWN:
        if (m_gameState == GAME_STATE::Play) m_gameState = GAME_STATE::Paused;
        break;
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE && m_gameState == GAME_STATE::Play)
            m_gameState = GAME_STATE::Paused;
        break;
    }
}

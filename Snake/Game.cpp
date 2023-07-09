#include "Game.hpp"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <thread>

CGame::CGame(core::Graphics::GraphicsEngine& ge, core::Audio::AudioEngine* pAE)
    : m_AudioMan(pAE), m_timerSnakeUpdate(250)
{
    ge.onInitDevice.connect([&](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        m_pBox = std::make_unique<core::Graphics::Box>(&ge, 0.f, 0.f, 520.f, 520.f, 10.f, DirectX::Colors::Gray);
        m_pSnake = std::make_unique<Snake>(ge);
        });
    ge.onRender.connect([&](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        m_AudioMan.proc();
        ge.setShaders();

        if (m_gameState != GAME_STATE::FirstStart) {
            m_pSnake->draw();
            m_pBox->draw();
            if (m_gameState == GAME_STATE::Play && bool(m_timerSnakeUpdate)) {
                m_pSnake->next();
                m_timerSnakeUpdate.reset();
            }
        }

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

        if (buttonDraw(m_gameState == GAME_STATE::FirstStart ? "start" : "continue", 0, 2, m_bMainMenuHovOld)) {
            m_gameState = GAME_STATE::Play;
            m_timerSnakeUpdate.reset();
        }
        if (buttonDraw("exit", 1, 2, m_bMainMenuHovOld))
            PostQuitMessage(EXIT_SUCCESS);

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool CGame::buttonDraw(std::string_view text, size_t i, size_t count, bool* arrBtnOldHov)
{
    auto size = ImGui::CalcTextSize(text.data());
    auto pos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - size.x / 2, (ImGui::GetIO().DisplaySize.y / (count + 1)) * (i + 1) - size.y / 2);

    ImGui::SetCursorPos(pos);
    bool res = ImGui::InvisibleButton(text.data(), size);
    bool hov = ImGui::IsItemHovered();

    ImGui::SetCursorPos(pos);
    ImGui::TextColored(hov ? COLOR_MENU_HOV : COLOR_MENU_UNHOV, text.data());

    if (hov && !arrBtnOldHov[i]) {
        arrBtnOldHov[i] = true;
        m_AudioMan.playFile("menu_select");
    }
    else if (res) {
        m_AudioMan.playFile("menu_direct");
    }
    else if (!hov) {
        arrBtnOldHov[i] = false;
    }
    return res;
}

void CGame::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE && m_gameState == GAME_STATE::Play)
            m_gameState = GAME_STATE::Paused;
        else if (wParam == VK_UP) m_pSnake->setDirect<Snake::DIRECT::UP>();
        else if (wParam == VK_DOWN) m_pSnake->setDirect<Snake::DIRECT::DOWN>();
        else if (wParam == VK_LEFT) m_pSnake->setDirect<Snake::DIRECT::LEFT>();
        else if (wParam == VK_RIGHT) m_pSnake->setDirect<Snake::DIRECT::RIGHT>();
        break;
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE && m_gameState == GAME_STATE::Play)
            m_gameState = GAME_STATE::Paused;
        break;
    }
}

void CGame::Snake::draw() {
    stBox box = m_arrBoxes[0];
    m_head.setPos(float(box.x) * SIZE_BOX + 10.f, float(box.y) * SIZE_BOX + 10.f);
    m_head.draw();
    for (size_t i{1}; i < m_nLen; ++i) {
        box = m_arrBoxes[i];
        m_body.setPos(float(box.x) * SIZE_BOX + 10.f, float(box.y) * SIZE_BOX + 10.f);
        m_body.draw();
    }
}

void CGame::Snake::next() {
    if (m_bGrow) {
        m_arrBoxes[m_nLen] = m_arrBoxes[m_nLen-1];
    }
    for (size_t i{m_nLen-1}; i > 0; --i) {
        m_arrBoxes[i] = m_arrBoxes[i-1];
    }
    if (m_bGrow) {
        m_nLen++;
        m_bGrow = false;
    }
    if (m_nSizeStackDirects) {
        m_direct = m_stackDirects[0];
        for (size_t i{m_nSizeStackDirects-1}; i > 0; --i) {
            m_stackDirects[i-1] = m_stackDirects[i];
        }
        m_nSizeStackDirects--;
    }

    if (m_direct == DIRECT::RIGHT) {
        if (++m_arrBoxes[0].x == COUNT_BOX) m_arrBoxes[0].x = 0;
    } else if (m_direct == DIRECT::LEFT) {
        if (--m_arrBoxes[0].x < 0) m_arrBoxes[0].x = COUNT_BOX-1;
    } else if (m_direct == DIRECT::UP) {
        if (++m_arrBoxes[0].y == COUNT_BOX) m_arrBoxes[0].y = 0;
    } else {
        if (--m_arrBoxes[0].y < 0) m_arrBoxes[0].y = COUNT_BOX-1;
    }
}

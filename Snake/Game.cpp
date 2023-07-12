#include "Game.hpp"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <thread>

void CGame::loadLang() {
    m_Language.setLang(m_conf.get<std::string>(L"langName"));
    m_Language.load("menu", m_wordsMenu);
    m_Language.load("game", m_wordsGame);
}

CGame::CGame(core::Graphics::GraphicsEngine& ge, core::Audio::AudioEngine* pAE)
    : m_AudioMan(pAE), m_timerSnakeUpdate(0), m_pAE(pAE)
{
    m_timerSnakeUpdate.setInterval(static_cast<UINT64>(m_conf.get<int>(L"snakeSpeed")));
    m_wordsMenu.set_empty_key("");
    m_wordsGame.set_empty_key("");
    while (m_conf.get<std::string>(L"langName") != m_Language.getLangsInfo().at(m_nLangID).first) {
        ++m_nLangID;
    }
    pAE->setVolume(m_conf.get<float>(L"soundVolume"));
    loadLang();

    ge.onInitDevice.connect([&](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        m_pBox = std::make_unique<core::Graphics::Box>(&ge, 0.f, 0.f, 520.f, 520.f, 10.f, DirectX::Colors::Gray);
        m_pSnake = std::make_unique<Snake>(&ge);
        m_pApple = std::make_unique<Apple>(&ge, m_pSnake.get());

        auto& io = ImGui::GetIO();
        m_pFontSet = io.Fonts->AddFontFromFileTTF("fonts/vga-8x16.ttf", 22.f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    });
    ge.onRender.connect([&](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
        m_AudioMan.proc();
        ge.setShaders();

        if (m_gameState != GAME_STATE::FirstStart) {
            m_pSnake->draw();
            if (!m_pSnake->isWin()) m_pApple->draw();
            m_pBox->draw();
            if (m_gameState == GAME_STATE::Play && bool(m_timerSnakeUpdate)) {
                if (!m_pSnake->isStop()) {
                    m_pSnake->next();
                    if (m_pApple->isDead(m_pSnake.get())) {
                        m_pSnake->grow();
                        m_AudioMan.playFile("apple");
                        m_pApple->genPosApple(m_pSnake.get());
                    }
                    if (m_pSnake->isCannibal()) {
                        m_AudioMan.playFile("game_over");
                        m_pSnake->stop();
                        m_gameState = GAME_STATE::GameOver;
                    }

                    m_timerSnakeUpdate.reset();
                }
            }
        }

        renderGUI();
        });
}

void CGame::renderGUI() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (m_gameState == GAME_STATE::GameOver) {
        std::string text = m_wordsGame["gameOver"];
        const auto sz = ImGui::CalcTextSize(text.c_str());
        const auto scr_sz = ImGui::GetIO().DisplaySize;
        ImGui::GetBackgroundDrawList()->AddText({(scr_sz.x - sz.x) / 2, (scr_sz.y - sz.y) / 2}, COLOR_GAME_OVER, text.c_str());
    }
    if (m_gameState == GAME_STATE::Play || m_gameState == GAME_STATE::GameOver) {
        const std::string text = m_wordsGame["score"] + std::to_string(m_pSnake->getLen());
        ImGui::PushFont(m_pFontSet);
        const auto sz = ImGui::CalcTextSize(text.c_str());
        const auto scr_sz = ImGui::GetIO().DisplaySize;
        ImGui::GetBackgroundDrawList()->AddText({scr_sz.x - sz.x - 15.f, 15.f}, COLOR_GAME_SCORE, text.c_str());
        ImGui::PopFont();
    }

    if (m_bShowMenu) {
        ImGui::SetNextWindowPos({ 0.0f, 0.0f });
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("main_menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

        if (m_menuHead == MENU::Main) {
            if (buttonDraw(m_gameState == GAME_STATE::FirstStart ? m_wordsMenu["start"] : m_wordsMenu["continue"], 0, 3, m_bMainMenuHovOld)) {
                if (m_gameState == GAME_STATE::FirstStart || m_gameState == GAME_STATE::Paused)
                    m_gameState = GAME_STATE::Play;
                m_bShowMenu = false;
                m_timerSnakeUpdate.reset();
            }
            if (buttonDraw(m_wordsMenu["settings"], 1, 3, m_bMainMenuHovOld))
                m_menuHead = MENU::Settings;
            if (buttonDraw(m_wordsMenu["quit"], 2, 3, m_bMainMenuHovOld))
                PostQuitMessage(EXIT_SUCCESS);
        } else {
            ImGui::TextColored(COLOR_MENU_UNHOV, m_wordsMenu["set_header"].c_str());
            ImGui::PushFont(m_pFontSet);

            ImGui::BeginChild("settings");
            if (ImGui::SliderFloat(m_wordsMenu["set_volume"].c_str(), &m_conf.get<float>(L"soundVolume"), 0.0f, 1.0f)) {
                m_pAE->setVolume(m_conf.get<float>(L"soundVolume"));
            }
            if (ImGui::Button(m_wordsMenu["set_language"].c_str())) {
                if (++m_nLangID >= m_Language.getLangsInfo().size())
                    m_nLangID = 0;
                m_conf.get<std::string>(L"langName") = m_Language.getLangsInfo().at(m_nLangID).first;
                loadLang();
            }
            if (ImGui::SliderInt(m_wordsMenu["set_snakeSpeed"].c_str(), &m_conf.get<int>(L"snakeSpeed"), 0, 10'000)) {
                m_timerSnakeUpdate.setInterval(m_conf.get<int>(L"snakeSpeed"));
            }

            ImGui::PopFont();
            ImGui::EndChild();
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool CGame::buttonDraw(std::string_view text, size_t i, size_t count, bool* arrBtnOldHov)
{
    auto size = ImGui::CalcTextSize(text.data());
    auto pos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - size.x / 2, (ImGui::GetIO().DisplaySize.y / float(count + 1)) * float(i + 1) - size.y / 2);

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
        if (wParam == VK_ESCAPE) {
            m_bShowMenu = true;
            if (m_gameState == GAME_STATE::Play)
                m_gameState = GAME_STATE::Paused;
            if (m_menuHead == MENU::Settings) {
                m_AudioMan.playFile("menu_back");
                m_menuHead = MENU::Main;
            }
        }
        else if (wParam == VK_UP) m_pSnake->setDirect<Snake::DIRECT::UP>();
        else if (wParam == VK_DOWN) m_pSnake->setDirect<Snake::DIRECT::DOWN>();
        else if (wParam == VK_LEFT) m_pSnake->setDirect<Snake::DIRECT::LEFT>();
        else if (wParam == VK_RIGHT) m_pSnake->setDirect<Snake::DIRECT::RIGHT>();
//        else if (wParam == VK_SPACE) m_pSnake->grow();
        break;
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) {
            m_bShowMenu = true;
            if (m_gameState == GAME_STATE::Play)
                m_gameState = GAME_STATE::Paused;
        }
        break;
    }
}

void CGame::Snake::draw() {
    stBox box = m_arrBoxes[0];
    m_head.setPos(float(box.x) * SIZE_BOX + 10.f, float(box.y) * SIZE_BOX + 10.f);
    for (size_t i{1}; i < m_nLen; ++i) {
        box = m_arrBoxes[i];
        m_body.setPos(float(box.x) * SIZE_BOX + 10.f, float(box.y) * SIZE_BOX + 10.f);
        m_body.draw();
    }
    m_head.draw();
}

void CGame::Snake::next() {
    assert(!m_bStop);

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

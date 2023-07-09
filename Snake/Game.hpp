#pragma once

#include <memory>
#include <string_view>
#include <Graphics.hpp>
#include <imgui.h>
#include <functional>

#include "AudioMan.hpp"

class CGame {
    std::unique_ptr<core::Graphics::Box> m_pBox{};

    static constexpr int COUNT_BOX = 20;
    static constexpr float SIZE_BOX = (520.f - 20.f) / float(COUNT_BOX);
    /*class button {
        bool m_bOldHov{};
    public:

        bool draw(std::string_view text, size_t i, size_t count, AudioMan& am);
    };*/
    //bool m_bBtnOldHov{};
    std::map<std::string_view, bool> m_btnOldHov{};
    bool buttonDraw(std::string_view text, size_t i, size_t count);


    enum class GAME_STATE {
        FirstStart,
        Play,
        Paused,
        GameOver
    };
    GAME_STATE m_gameState = GAME_STATE::FirstStart;

    static constexpr ImVec4 COLOR_MENU_HOV{0.827451050f, 0.827451050f, 0.827451050f, 1.f};
    static constexpr ImVec4 COLOR_MENU_UNHOV{0.662745118f, 0.662745118f, 0.662745118f, 1.f};

    void renderGUI();

    AudioMan m_AudioMan;

public:
    explicit CGame(core::Graphics::GraphicsEngine& ge, core::Audio::AudioEngine* pAE);
    ~CGame();

    void wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
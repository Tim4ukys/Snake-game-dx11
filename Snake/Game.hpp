#pragma once

#include <memory>
#include <string_view>
#include <Graphics.hpp>
#include <imgui.h>
#include <functional>

#include "AudioMan.hpp"
#include "Timer.hpp"

class CGame {
    std::unique_ptr<core::Graphics::Box> m_pBox{};

    static constexpr int COUNT_BOX = 34;
    static constexpr float SIZE_BOX = (520.f - 20.f) / float(COUNT_BOX);

    struct stBox {
        int x{}, y{};
    };

    class Snake {
        core::Graphics::SolidRect m_head, m_body;
        size_t m_nLen{};
        stBox m_arrBoxes[COUNT_BOX*COUNT_BOX];
        bool m_bGrow{};

    public:
        enum class DIRECT {
            UP, DOWN, LEFT, RIGHT
        };
        template<DIRECT direct>
        void setDirect() {
            if (m_nSizeStackDirects >= ARRAYSIZE(m_stackDirects)) return;
            if constexpr (direct == DIRECT::UP) {
                if (m_stackDirects[m_nSizeStackDirects != 0 ? m_nSizeStackDirects-1 : 0] != DIRECT::DOWN)
                    m_stackDirects[m_nSizeStackDirects++] = direct;
            } else if constexpr (direct == DIRECT::DOWN) {
                if (m_stackDirects[m_nSizeStackDirects != 0 ? m_nSizeStackDirects-1 : 0] != DIRECT::UP)
                    m_stackDirects[m_nSizeStackDirects++] = direct;
            } else if constexpr (direct == DIRECT::LEFT) {
                if (m_stackDirects[m_nSizeStackDirects != 0 ? m_nSizeStackDirects-1 : 0] != DIRECT::RIGHT)
                    m_stackDirects[m_nSizeStackDirects++] = direct;
            } else {
                if (m_stackDirects[m_nSizeStackDirects != 0 ? m_nSizeStackDirects-1 : 0] != DIRECT::LEFT)
                    m_stackDirects[m_nSizeStackDirects++] = direct;
            }
        }

        explicit Snake(core::Graphics::GraphicsEngine& ge)
            : m_head(&ge, 0, 0, SIZE_BOX, SIZE_BOX, DirectX::Colors::DarkOliveGreen),
              m_body(&ge, 0, 0, SIZE_BOX, SIZE_BOX, DirectX::Colors::DarkSeaGreen) {
            m_nLen = 3;
            m_arrBoxes[0] = { 1, 3 };
            m_arrBoxes[1] = { 1, 2 };
            m_arrBoxes[2] = { 1, 1 };
        }

        void next();
        inline void grow() noexcept { m_bGrow = true; }

        void draw();

    private:
        DIRECT m_direct = DIRECT::LEFT;
        DIRECT m_stackDirects[2]{DIRECT::LEFT};
        size_t m_nSizeStackDirects = 0;
    };
    std::unique_ptr<Snake> m_pSnake{};
    Timer m_timerSnakeUpdate;

    bool m_bMainMenuHovOld[2];
    bool buttonDraw(std::string_view text, size_t i, size_t count, bool* arrBtnOldHov);

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
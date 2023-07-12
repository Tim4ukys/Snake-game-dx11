#pragma once

#include <random>
#include <memory>
#include <string_view>
#include <Graphics.hpp>
#include <imgui.h>
#include <functional>

#include "Config.hpp"
#include "AudioMan.hpp"
#include "Timer.hpp"
#include "Language.hpp"

class CGame {
    std::unique_ptr<core::Graphics::Box> m_pBox{};

    static constexpr int COUNT_BOX = 34;
    static constexpr float SIZE_BOX = (520.f - 20.f) / float(COUNT_BOX);

    struct stBox {
        int x{}, y{};
    };

    class Apple;
    class Snake {
        core::Graphics::SolidRect m_head, m_body;
        size_t m_nLen{};
        stBox m_arrBoxes[COUNT_BOX*COUNT_BOX];
        bool m_bGrow{};
        bool m_bStop{};

    public:
        friend Apple;

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

        inline bool isPosInBody(int x, int y) const noexcept {
            for (size_t i{}; i < m_nLen; ++i) {
                if (m_arrBoxes[i].x == x && m_arrBoxes[i].y == y)
                    return true;
            }
            return false;
        }

        explicit Snake(core::Graphics::GraphicsEngine* pGE)
            : m_head(pGE, 0, 0, SIZE_BOX, SIZE_BOX, DirectX::Colors::DarkOliveGreen),
              m_body(pGE, 0, 0, SIZE_BOX, SIZE_BOX, DirectX::Colors::DarkSeaGreen) {
            m_nLen = 3;
            m_arrBoxes[0] = { 1, 3 };
            m_arrBoxes[1] = { 1, 2 };
            m_arrBoxes[2] = { 1, 1 };
        }

        inline bool isWin() const noexcept {
            return m_nLen >= COUNT_BOX*COUNT_BOX;
        }
        inline bool isCannibal() const noexcept {
            for (size_t i{1}; i < m_nLen; ++i) {
                if (m_arrBoxes[0].x == m_arrBoxes[i].x && m_arrBoxes[0].y == m_arrBoxes[i].y)
                    return true;
            }
            return false;
        }
        inline bool isStop() const noexcept {
            return m_bStop;
        }
        inline void stop() noexcept {
            m_bStop = true;
        }

        void next();
        inline void grow() noexcept { m_bGrow = true; }

        void draw();

        inline auto getLen() const noexcept {
            return m_nLen;
        }

    private:
        DIRECT m_direct = DIRECT::RIGHT;
        DIRECT m_stackDirects[2]{DIRECT::RIGHT};
        size_t m_nSizeStackDirects = 0;
    };
    std::unique_ptr<Snake> m_pSnake{};
    Timer m_timerSnakeUpdate;

    class Apple {
        core::Graphics::SolidRect m_rect;
        std::random_device m_randDevice;
        int m_nX, m_nY;
    public:
        inline void genPosApple(Snake* pSnake) noexcept {
            std::mt19937 rng(m_randDevice());
            std::uniform_int_distribution<int> dist(0,COUNT_BOX-1);
            do {
                m_nX = dist(rng);
                m_nY = dist(rng);
            } while (pSnake->isPosInBody(m_nX, m_nY));
            m_rect.setPos(float(m_nX) * SIZE_BOX + 10.f, float(m_nY) * SIZE_BOX + 10.f);
        }
        explicit Apple(core::Graphics::GraphicsEngine* pGE, Snake* pSnake)
                : m_rect(pGE, 0, 0, SIZE_BOX, SIZE_BOX, DirectX::Colors::DarkRed) {
            genPosApple(pSnake);
        }
        inline bool isDead(Snake* pSnake) const noexcept {
            return pSnake->m_arrBoxes[0].x == m_nX && pSnake->m_arrBoxes[0].y == m_nY;
        };
        inline void draw() noexcept {
            m_rect.draw();
        }
    };
    std::unique_ptr<Apple> m_pApple;

    bool m_bMainMenuHovOld[3]{};
    bool buttonDraw(std::string_view text, size_t i, size_t count, bool* arrBtnOldHov);

    enum class GAME_STATE {
        FirstStart,
        Play,
        Paused,
        GameOver
    };
    GAME_STATE m_gameState = GAME_STATE::FirstStart;

    static constexpr ImU32 COLOR_GAME_OVER = 0x63'43'43'FF;
    static constexpr ImVec4 COLOR_MENU_HOV{0.827451050f, 0.827451050f, 0.827451050f, 1.f};
    static constexpr ImVec4 COLOR_MENU_UNHOV{0.662745118f, 0.662745118f, 0.662745118f, 1.f};
    static constexpr ImU32 COLOR_GAME_SCORE = 0x93'93'93'70;

    bool m_bShowMenu{true};
    enum class MENU {
        Main, Settings
    } m_menuHead = MENU::Main;
    ImFont* m_pFontSet;
    void renderGUI();

    void loadLang();
    Language::hashWords m_wordsMenu;
    Language::hashWords m_wordsGame;
    Language m_Language;
    AudioMan m_AudioMan;
    core::Audio::AudioEngine* m_pAE;
    size_t m_nLangID{};

    ConfigReg m_conf = ConfigReg(HKEY_CURRENT_USER, L"SOFTWARE\\tim4ukys\\SnakeDX11",
                                 ConfigReg::Value<std::string_view>(L"langName", "ru"),
                                 ConfigReg::Value<float>(L"soundVolume", 0.5f),
                                 ConfigReg::Value<int>(L"snakeSpeed", 250));

public:
    explicit CGame(core::Graphics::GraphicsEngine& ge, core::Audio::AudioEngine* pAE);
    ~CGame() = default;

    void wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
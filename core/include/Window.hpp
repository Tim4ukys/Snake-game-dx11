#ifndef __CORE_WINDOW_HPP__
#define __CORE_WINDOW_HPP__

#include <string_view>
#include <Windows.h>
#include <functional>

namespace core {
    class Window {
        Window() = delete;
        Window(Window&) = delete;
        Window(Window&&) = delete;

        HWND m_hWnd;
        const int m_nCmdShow;
        const int m_nScreenWidth;
        const int m_nScreenHeight;

    public:
#ifndef UNICODE
        using str = std::string_view;
#else
        using str = std::wstring_view;
#endif
        explicit Window(HINSTANCE hInstance, const str& windowName, const str& className, int width, int height, int cmdShow, WNDPROC wnd_proc);

        inline auto getHWND() const noexcept {
            return m_hWnd;
        };
        inline std::pair<int, int> getScreenSize() const noexcept {
            return {m_nScreenWidth, m_nScreenHeight};
        }

        int run() const;
    };

}

#endif

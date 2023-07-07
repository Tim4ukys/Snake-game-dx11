#ifndef __CORE_WINDOW_HPP__
#define __CORE_WINDOW_HPP__

#include <string_view>
#include <Windows.h>
#include <functional>
#include <asmjit/asmjit.h>
#include <boost/signals2.hpp>

namespace core {
    class Window {
    public:
#ifndef UNICODE
        using str = std::string_view;
#else
        using str = std::wstring_view;
#endif
    private:

        Window() = delete;
        Window(Window&) = delete;
        Window(Window&&) = delete;

        HWND m_hWnd;
        const int m_nCmdShow;

        str m_szClassName;
        HINSTANCE m_hInstance;

        asmjit::JitRuntime m_rtWndProc;

    public:
        explicit Window(HINSTANCE hInstance, const str& windowName, const str& className, int width, int height, int cmdShow, 
            std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> wnd_proc);
        ~Window();

        inline auto getHWND() const noexcept {
            return m_hWnd;
        };
        inline std::pair<int, int> getClientScreenSize() const noexcept {
            RECT r;
            GetClientRect(m_hWnd, &r);
            return {r.right - r.left, r.bottom - r.top};
        }

        int run() const;
    };

}

#endif

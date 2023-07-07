#include "Window.hpp"
#include <exception>
#include <asmjit/asmjit.h>

namespace core {

    class WNDProcWrapper {
        std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> m_callback;
    public:
        WNDProcWrapper(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> clb) : m_callback(clb) {}

#if defined(__x86_64__) || defined(_M_X64)
        __declspec(noinline) static LRESULT __fastcall proc(WNDProcWrapper* wr, HWND a1, UINT a2, WPARAM a3, LPARAM a4) {
            return wr->m_callback(a1, a2, a3, a4);
        }
#else
        __declspec(noinline) static LRESULT __fastcall proc(WNDProcWrapper* wr, std::uintptr_t, HWND a1, UINT a2, WPARAM a3, LPARAM a4) {
            return wr->m_callback(a1, a2, a3, a4);
        }
#endif
    };

    Window::Window(HINSTANCE hInstance, const str& windowName, const str& className, int width, int height, int cmdShow, 
        std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> wnd_proc)
        : m_nCmdShow(cmdShow), m_szClassName(className), m_hInstance(hInstance) {
        WNDCLASSEX wc{};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hInstance = hInstance;

        using namespace asmjit;
        CodeHolder code;
        code.init(m_rtWndProc.environment());
        x86::Assembler a(&code);

        WNDProcWrapper* wndProcWrapper = new WNDProcWrapper(wnd_proc);
#if defined(__x86_64__) || defined(_M_X64)
        a.mov(x86::qword_ptr(x86::rsp, 32 + 8), x86::r9);
        a.mov(x86::r9, x86::r8);
        a.mov(x86::r8d, x86::edx);
        a.mov(x86::rdx, x86::rcx);
        a.mov(x86::rcx, wndProcWrapper);
#else
        a.mov(x86::ecx, wndProcWrapper);
#endif
        a.jmp((void*)&WNDProcWrapper::proc);
        m_rtWndProc.add(&wc.lpfnWndProc, &code);

        wc.lpszClassName = className.data();
        wc.style = CS_VREDRAW | CS_HREDRAW;

        if (!RegisterClassEx(&wc))
            throw std::exception("RegisterClassEx return pizdec");

        m_hWnd = CreateWindow(wc.lpszClassName, windowName.data(), WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX, 0, 0,
            width, height, nullptr, nullptr, wc.hInstance, nullptr);
        if (m_hWnd == INVALID_HANDLE_VALUE)
            throw std::exception("hWnd == trash");
    }

    Window::~Window()
    {
        DestroyWindow(m_hWnd);
        UnregisterClass(m_szClassName.data(), m_hInstance);
    }

    int Window::run() const {
        ShowWindow(m_hWnd, m_nCmdShow);
        UpdateWindow(m_hWnd);

        MSG msg{};
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return static_cast<int>(msg.wParam);
    }
}
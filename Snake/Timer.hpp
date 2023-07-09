//
// Created by Tim4ukys on 09.07.2023.
//

#ifndef SNAKEDX11_TIMER_HPP
#define SNAKEDX11_TIMER_HPP

#include <Windows.h>

class Timer {
    UINT64 m_nOldTick{};
    UINT64 m_nInterval;
public:
    Timer(UINT64 interval) : m_nInterval(interval) {
    };

    inline bool isEnd() noexcept {
        if (m_nOldTick + m_nInterval < GetTickCount64()) {
            return true;
        }
        return false;
    }

    inline explicit operator bool() noexcept {
        return isEnd();
    }

    inline void reset() noexcept {
        m_nOldTick = GetTickCount64();
    }

    inline void setInterval(UINT64 interval) noexcept {
        m_nInterval = interval;
    }
};

#endif //SNAKEDX11_TIMER_HPP

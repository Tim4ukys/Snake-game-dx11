#ifndef __CORE_CORE_ENGINE_HPP__
#define __CORE_CORE_ENGINE_HPP__

#include <cinttypes>

namespace core {

    template<typename T>
    class CUnkown {
        T* m_pUnk;
        std::uint32_t* m_pCountRef;

    public:
        CUnkown(T* pnew) {
            m_pCountRef = new std::uint32_t(1u);
            m_pUnk = pnew;
        }

        template<typename... Args> 
        CUnkown(Args&&... args) {
            m_pCountRef = new std::uint32_t(1u);
            m_pUnk = new T(args...);
        }

        CUnkown(CUnkown& unk)
            : m_pUnk(unk.m_pUnk), m_pCountRef(unk.m_pCountRef) {
            ++(*m_pCountRef);
        }
        ~CUnkown() {
            if (--(*m_pCountRef) == 0) {
                delete m_pUnk;
                delete m_pCountRef;
            }
        }

        T* operator->() noexcept {
            return m_pUnk;
        }

        inline T* get() noexcept {
            return m_pUnk;
        } 
    };

}

#endif

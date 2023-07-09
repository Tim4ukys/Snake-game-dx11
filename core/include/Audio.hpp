#ifndef __CORE_AUDIO_HPP__
#define __CORE_AUDIO_HPP__

#include <XAudio2.h>
#include <wrl/client.h>
#include <vector>
#include <memory>
#include <string_view>

namespace core {
    namespace Audio {
        class WAVFile {
            WAVEFORMATEX m_waveFormat{};
            std::vector<std::uint8_t> m_data;

        public:
            WAVFile() = default;
            WAVFile(const WAVFile& file) = default;
            WAVFile(WAVFile&&) = default;

            explicit WAVFile(std::string_view text);
            ~WAVFile() = default;

            inline auto& getData() const noexcept {
                return m_data;
            }
            inline auto& getWaveFormat() const noexcept {
                return m_waveFormat;
            }

        };

        class Source {
            IXAudio2SourceVoice* m_pXASourceVoice{};
            XAUDIO2_BUFFER m_XABuffer{};

        public:
            friend class AudioEngine;

            Source() {
                m_XABuffer.Flags = XAUDIO2_END_OF_STREAM;
            }
            ~Source() {
                m_pXASourceVoice->DestroyVoice();
            }

            inline void playSound(const WAVFile& WAVfile) noexcept {
                const auto& file = WAVfile.getData();
                m_XABuffer.AudioBytes = file.size();
                m_XABuffer.pAudioData = file.data();
                m_pXASourceVoice->SubmitSourceBuffer(&m_XABuffer);
                m_pXASourceVoice->Start(0);
            }

            inline bool isBusy() noexcept {
                XAUDIO2_VOICE_STATE st;
                m_pXASourceVoice->GetState(&st);
                return st.BuffersQueued > 0;
            }
        };
        
        class AudioEngine {
            Microsoft::WRL::ComPtr<IXAudio2> m_pXA;
            IXAudio2MasteringVoice* m_pXAMasterVoice;

        public:
            explicit AudioEngine() {
                if (FAILED(XAudio2Create(&m_pXA))) throw std::exception("XAudio2Create failed.");
                if (FAILED(m_pXA->CreateMasteringVoice(&m_pXAMasterVoice))) throw std::exception("IXAudio2::CreateMasteringVoice failed.");
            }
            ~AudioEngine() {
                m_pXAMasterVoice->DestroyVoice();
            }

            void createSource(std::shared_ptr<Source>& source, const WAVEFORMATEX* waveFormat) {
                source = std::make_shared<Source>();
                m_pXA->CreateSourceVoice(&source->m_pXASourceVoice, waveFormat);
            }
        };
    }
}

#endif 
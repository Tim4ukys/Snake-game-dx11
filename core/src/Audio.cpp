#include "Audio.hpp"
#include <exception>
#include <fstream>
#include <cassert>
#include <opusfile.h>

namespace core {
    Audio::WAVFile::WAVFile(std::string_view text)
    {
        std::ifstream file{text.data(), std::ios::binary};
        file.seekg(20);
        file.read(reinterpret_cast<PCHAR>(&m_waveFormat), 16);
        m_waveFormat.cbSize = 0;

        struct RIFFextra {
            char chunkID[4];
            uint32_t chunkSize;
        } riff_extra;
        file.read(reinterpret_cast<PCHAR>(&riff_extra), sizeof(RIFFextra));
        if (memcmp(&riff_extra, "data", 4) != 0) {
            file.seekg(riff_extra.chunkSize, file.cur);
            file.read(reinterpret_cast<PCHAR>(&riff_extra), sizeof(RIFFextra));
        }

        m_data.resize(riff_extra.chunkSize);
        file.read(reinterpret_cast<PCHAR>(m_data.data()), riff_extra.chunkSize);
    }

    Audio::OGGFile::OGGFile(std::string_view text) {
        int err;
        auto* fileHandle = op_test_file(text.data(), &err);
        assert(fileHandle);
        op_test_open(fileHandle);
        const OpusHead* opHead = op_head(fileHandle, 0);

        m_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        m_waveFormat.nChannels = opHead->channel_count;
        m_waveFormat.nSamplesPerSec = opHead->input_sample_rate;
        m_waveFormat.nAvgBytesPerSec = opHead->input_sample_rate*opHead->channel_count*sizeof(opus_int16);
        m_waveFormat.nBlockAlign = opHead->channel_count*sizeof(opus_int16);
        m_waveFormat.wBitsPerSample = sizeof(opus_int16)*8;
        m_waveFormat.cbSize = 0;

        auto frameRem = ogg_uint64_t(op_pcm_total(fileHandle, -1));
        m_data.resize(frameRem*2*sizeof(opus_int16));

        opus_int16* op{ (opus_int16*)m_data.data() };
        while (frameRem > 0) {
            int frameRead = op_read_stereo(fileHandle, op, frameRem*2);
            if (!frameRead)
                break;

            op += frameRead * 2;
            frameRem -= frameRead;
        }

        op_free(fileHandle);
    }
}
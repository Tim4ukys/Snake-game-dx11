#include "Audio.hpp"
#include <exception>
#include <fstream>

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
}
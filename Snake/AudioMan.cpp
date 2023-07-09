#include "AudioMan.hpp"
#include <filesystem>

AudioMan::AudioMan(core::Audio::AudioEngine* pAE) : m_pAE(pAE)
{
    m_musics.set_empty_key("");
    for (auto& file : std::filesystem::directory_iterator("music")) {
        auto name = file.path().filename().string();
        name.erase(name.end() - file.path().extension().string().size(), name.end());
        m_musics.insert({name, core::Audio::WAVFile(file.path().string())});
    }
}

AudioMan::~AudioMan()
{
    while (!m_Sources.empty()) {
        m_Sources[0].reset();
        m_Sources.erase(m_Sources.begin());
    }
}

void AudioMan::proc()
{
    for (size_t i{}; i < m_Sources.size();) {
        auto& src = m_Sources[i];

        if (src->isBusy()) {
            ++i;
        }
        else {
            src.reset();
            m_Sources.erase(m_Sources.begin() + i);
        }
    }
}

void AudioMan::playFile(std::string_view file_name)
{
    auto& file = m_musics[std::string(file_name)];

    std::shared_ptr<core::Audio::Source> sound;
    m_pAE->createSource(sound, &file.getWaveFormat());
    sound->playSound(file);
    m_Sources.emplace_back(std::move(sound));
}

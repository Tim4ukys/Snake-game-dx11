#pragma once 

#include <Audio.hpp>
#include <memory>
#include <string_view>
#include <map>
#include <sparsehash/dense_hash_map>

class AudioMan {
    std::vector<std::shared_ptr<core::Audio::Source>> m_Sources;
    core::Audio::AudioEngine* m_pAE;
    google::dense_hash_map<std::string, core::Audio::WAVFile, std::hash<std::string>> m_musics;

public:
    AudioMan(core::Audio::AudioEngine* pAE);
    ~AudioMan();

    void proc();
    void playFile(std::string_view file_name);
};
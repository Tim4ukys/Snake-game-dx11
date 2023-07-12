//
// Created by Tim4ukys on 11.07.2023.
//

#ifndef SNAKEDX11_LANGUAGE_HPP
#define SNAKEDX11_LANGUAGE_HPP

#include <sparsehash/dense_hash_map>
#include <sol/sol.hpp>
#include <string_view>
#include <vector>

class Language {
public:
    using langData = std::pair<std::string/*fileName*/, std::string/*langName*/>;
private:
    std::string m_langCode;
    std::vector<langData> m_langInfo;

public:
    explicit Language();
    ~Language() = default;

    auto& getLangsInfo() noexcept {
        return m_langInfo;
    }

    inline void setLang(const std::string &langCode) {
        m_langCode = langCode;
    }

    using hashWords = google::dense_hash_map<std::string, std::string, std::hash<std::string>>;
    void load(std::string_view name, hashWords& out);
};

#endif //SNAKEDX11_LANGUAGE_HPP

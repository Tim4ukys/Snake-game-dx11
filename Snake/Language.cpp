//
// Created by Tim4ukys on 11.07.2023.
//
#include "Language.hpp"
#include <cstring>
#include <filesystem>

Language::Language() {
    for (auto& file : std::filesystem::directory_iterator("language")) {
        auto name = file.path().filename().string();
        name.erase(name.end() - file.path().extension().string().size(), name.end());

        sol::state lua;
        sol::table table = lua.do_file(file.path().string());
        m_langInfo.emplace_back(name, table["name"].get<std::string>());
    }
}

template<typename T, size_t N>
constexpr size_t lenString(const T(&)[N]) {
    return N-1;
}

void Language::load(std::string_view name, hashWords& out) {
    if (!out.empty()) out.clear();

    char* buff = new char[m_langCode.length() + lenString("language/") + lenString(".lua") + 1];
    sprintf(buff, "language/%s.lua", m_langCode.c_str());
    sol::state lua;
    sol::table langTable = lua.do_file(buff);

    auto tb = langTable[name].get<sol::table>();
    for (auto& [key, words] : tb) {
        out[key.as<std::string>()] = words.as<std::string>();
    }

    delete[] buff;
}

#pragma once
#include <string>




struct TextureData {
    std::string m_filepath;
    std::string m_fileExtenction;
    int m_width = 0;
    int m_height = 0;
    int m_numChannels = 0;
    void* m_data = nullptr;
};

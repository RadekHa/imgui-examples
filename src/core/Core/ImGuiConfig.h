#pragma once

#include <string>

namespace App
{
    struct ImGuiConfig
    {
        std::string iniFile;
        std::string fontFile;
        float fontSize{18.0f};
    };
}

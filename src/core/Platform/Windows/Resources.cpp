#include "Core/Resources.hpp"

#include <SDL2/SDL.h>
#include <tracy/Tracy.hpp>

#include <filesystem>
#include <string>
#include <string_view>

namespace fs = std::filesystem;
using namespace App;
using namespace std;


static const string BASE_PATH{SDL_GetBasePath ()};

fs::path Resources::resource_path (const fs::path& file_path)
{
    ZoneScoped;

    fs::path font_path{BASE_PATH};
    font_path /= "../share" / file_path;
    return font_path;
}

fs::path Resources::font_path (const string_view& font_file)
{
    ZoneScoped;

    return resource_path ("fonts") / font_file;
}

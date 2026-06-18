#include "SdlPathService.h"
#include "Settings/Project.hpp"

#include <SDL_filesystem.h>

namespace fs = std::filesystem;
using namespace App;
using namespace Sdl;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Factory function.

unique_ptr<IPathService> App::createSdlPathService ()
{
    return make_unique<SDLPathService>();
}

///////////////////////////////////////////////////////////////////////////////
// SDLPathService

SDLPathService::SDLPathService ()
    : m_basePath {SDL_GetBasePath ()}
{
    SdlTextPtr path{SDL_GetPrefPath (COMPANY_NAMESPACE.c_str (), APP_NAME.c_str ())};
    fs::path iniFilePath{path.get ()};
    iniFilePath /= "imgui.ini";

    m_userConfigPath = toString (iniFilePath);
}

const char* SDLPathService::getUserConfigFilePath () const
{
    return m_userConfigPath.c_str ();
}

string SDLPathService::getFontPath (string_view fontFileName) const
{
    fs::path fontPath = getBaseResourcePath () / "fonts" / fontFileName;
    return toString (fontPath.lexically_normal ());
}

string SDLPathService::getResourcePath (string_view filePath) const
{
    fs::path resourcePath = getBaseResourcePath () / filePath;
    return toString (resourcePath.lexically_normal ());
}

fs::path SDLPathService::getBaseResourcePath () const
{
    fs::path resourcePath = fs::path (m_basePath.get ()) / ".." / "share";
    return resourcePath.lexically_normal ();
}

string SDLPathService::toString (const fs::path& filePath) const
{
    u8string fileName = filePath.u8string ();
    return string (begin (fileName), end (fileName));
}

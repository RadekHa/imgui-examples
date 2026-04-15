#include "SdlPathService.h"
#include "Settings/Project.hpp"

#include <SDL_filesystem.h>

namespace fs = std::filesystem;
using namespace App;
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
}

fs::path SDLPathService::getUserConfigPath () const
{
    return SDL_GetPrefPath (COMPANY_NAMESPACE.c_str (), APP_NAME.c_str ());
}

fs::path SDLPathService::getFontPath (string_view fontFileName) const
{
    return getResourcePath ("fonts") / fontFileName;
}

fs::path SDLPathService::getResourcePath (string_view filePath) const
{
    return m_basePath / "../share" / filePath;
}

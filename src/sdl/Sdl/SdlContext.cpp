#include "SdlContext.h"
#include "TraceLog/Log.hpp"

#include <SDL3/SDL.h>

#include <stdexcept>

using namespace App;
using namespace std;

SdlContext::SdlContext ()
{
    // SDL3 handles DPI automatically, no hint needed
    if (!SDL_Init (SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        throw runtime_error (SDL_GetError ());
    }

    // Camera subsystem - optional, don't fail app if unavailable
    if (!SDL_InitSubSystem (SDL_INIT_CAMERA))
    {
        APP_WARN ("SDL camera subsystem unavailable: {}", SDL_GetError ());
    }
}

SdlContext::~SdlContext ()
{
    SDL_Quit ();
}

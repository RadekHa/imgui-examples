#include "SdlContext.h"

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
}

SdlContext::~SdlContext ()
{
    SDL_Quit ();
}

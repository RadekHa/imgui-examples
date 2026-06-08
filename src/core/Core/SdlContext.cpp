#include "SDLContext.h"

#include <SDL2/SDL.h>

#include <stdexcept>

using namespace App;
using namespace std;

SdlContext::SdlContext ()
{
    SDL_SetHint (SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");

    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        throw runtime_error (SDL_GetError ());
    }
}

SdlContext::~SdlContext ()
{
    SDL_Quit ();
}

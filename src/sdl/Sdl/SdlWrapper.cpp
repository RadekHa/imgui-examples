#include "SdlWrapper.h"

#include <SDL3/SDL.h>

void Sdl::destroyWindow (SDL_Window* w) noexcept
{
    SDL_DestroyWindow (w);
}

void Sdl::destroyRenderer (SDL_Renderer* r) noexcept
{
    SDL_DestroyRenderer (r);
}

void Sdl::destroyTexture (SDL_Texture* t) noexcept
{
    SDL_DestroyTexture (t);
}

void Sdl::destroySurface (SDL_Surface* s) noexcept
{
    SDL_DestroySurface (s);
}

void Sdl::freeText (char* p) noexcept
{
    SDL_free (p);
}

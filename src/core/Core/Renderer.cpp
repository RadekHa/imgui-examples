#include "Renderer.h"

#include <SDL2/SDL_render.h>
#include <stdexcept>

using namespace App;
using namespace std;

Renderer::Renderer (SDL_Window* window)
    : m_renderer {}
{
    Uint32 flags = SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED;

    m_renderer = SDL_CreateRenderer (window, -1, flags);

    if (!m_renderer)
    {
        throw runtime_error (SDL_GetError ());
    }
}

Renderer::~Renderer ()
{
    SDL_DestroyRenderer (m_renderer);
}

void Renderer::beginFrame ()
{
    SDL_SetRenderDrawColor (m_renderer, 45, 45, 48, 255);
    SDL_RenderClear (m_renderer);
}

void Renderer::endFrame ()
{
    SDL_RenderPresent (m_renderer);
}

SDL_Renderer* Renderer::native () const
{
    return m_renderer;
}

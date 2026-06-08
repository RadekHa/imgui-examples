#include "FrameContext.h"
#include "SdlRenderer.h"

#include <stdexcept>

using namespace App;
using namespace sdl;
using namespace std;

SdlRenderer::SdlRenderer (SDL_Window* window)
    : m_renderer {}
{
    Uint32 flags = SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED;

    m_renderer = SdlRendererPtr{
        SDL_CreateRenderer (window, -1, flags)
    };

    if (!m_renderer)
    {
        throw runtime_error (SDL_GetError ());
    }
}

SdlRenderer::~SdlRenderer () = default;

void SdlRenderer::beginFrame (const FrameContext& ctx)
{
    SDL_RenderSetScale (m_renderer.get (), ctx.scaleX, ctx.scaleY);
    SDL_SetRenderDrawColor (m_renderer.get (), 100, 100, 100, 255);
    SDL_RenderClear (m_renderer.get ());
}

void SdlRenderer::endFrame ()
{
    SDL_RenderPresent (m_renderer.get ());
}

SDL_Renderer* SdlRenderer::native () const
{
    return m_renderer.get ();
}

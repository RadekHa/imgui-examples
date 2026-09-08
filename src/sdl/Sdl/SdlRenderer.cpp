#include "FrameContext.h"
#include "SdlRenderer.h"
#include "SdlUtils.h"
#include "TraceLog/Log.hpp"

#include <stdexcept>

using namespace App;
using namespace Sdl;
using namespace std;

SdlRenderer::SdlRenderer (SDL_Window* window, const IPathService* paths)
    : m_renderer {}
{
    if (!window)
    {
        throw runtime_error ("Window parameter cannot be null");
    }
    m_renderer = SdlRendererPtr{SDL_CreateRenderer (window, nullptr)};

    if (m_renderer)
    {
        const char* name = SDL_GetRendererName (m_renderer.get ());
        // Enable VSync
        SDL_SetRenderVSync (m_renderer.get (), 1);
        APP_INFO ("Renderer successfully created using backend: {}", name);
    }

    // Final safety check
    if (!m_renderer)
    {
        APP_FATAL ("Fatal error: All available renderer backends failed to initialize: {}", SDL_GetError ());
        throw runtime_error (SDL_GetError ());
    }
    // Load texture.
    m_background = LoadTexture (m_renderer.get (), paths->getResourcePath ("images/image.png").c_str ());
}

SdlRenderer::~SdlRenderer () = default;

void SdlRenderer::beginFrame (const FrameContext& ctx)
{
    SDL_SetRenderScale (m_renderer.get (), ctx.scaleX, ctx.scaleY);
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

void SdlRenderer::update (DataModel& /*model*/)
{
    if (m_background)
    {
        float textureWidth = 0;
        float textureHeight = 0;

        SDL_GetTextureSize (m_background.get (), &textureWidth, &textureHeight);

        SDL_FRect destinationRect{.x = 50.0f, .y = 50.0f, .w = (float) textureWidth, .h = (float) textureHeight};

        SDL_RenderTexture (m_renderer.get (), m_background.get (), nullptr, &destinationRect);
    }
}

#include "FrameContext.h"
#include "Log.hpp"
#include "SdlRenderer.h"
#include "SdlUtils.h"

#include <stdexcept>

using namespace App;
using namespace sdl;
using namespace std;

SdlRenderer::SdlRenderer (SDL_Window* window, const IPathService* paths)
    : m_renderer {}
{
    if (!window)
    {
        throw runtime_error ("Window parameter cannot be null");
    }
    // Define a structured pair for configuration: [renderer flags, description for logging]
    const pair<Uint32, string_view> rendererConfigs [] = {
        {SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC, "Hardware Accelerated + VSync"},
        {SDL_RENDERER_ACCELERATED, "Hardware Accelerated (No VSync)"},
        {SDL_RENDERER_SOFTWARE, "Software Renderer (CPU Fallback)"}
    };

    // Iterate through configurations and attempt to create the renderer
    for (const auto& [flags, description] : rendererConfigs)
    {
        m_renderer = SdlRendererPtr{SDL_CreateRenderer (window, -1, flags)};

        if (m_renderer)
        {
            APP_INFO ("Renderer successfully created using mode: {}", description);
            break; // Found a working renderer, exit the loop
        }
        // Log a warning if the current mode failed, but only if we are not at the end of the chain
        APP_WARN ("Renderer mode '{}' failed ({}).", description, SDL_GetError ());
    }

    // Final safety check to ensure at least the software renderer was initialized
    if (!m_renderer)
    {
        APP_FATAL ("Fatal error: All available renderer modes failed to initialize: {}", SDL_GetError ());
        throw runtime_error (SDL_GetError ());
    }
    // Load texture.
    m_background = LoadTexture (m_renderer.get (), paths->getResourcePath ("images/image.png").c_str ());
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

void SdlRenderer::update (DataModel& model)
{
    int textureWidth = 0;
    int textureHeight = 0;

    SDL_QueryTexture (m_background.get (), nullptr, nullptr, &textureWidth, &textureHeight);

    SDL_Rect destinationRect{.x = 50, .y = 50, .w = textureWidth, .h = textureHeight};

    SDL_RenderCopy (m_renderer.get (), m_background.get (), nullptr, nullptr);
    SDL_RenderCopy (m_renderer.get (), m_background.get (), nullptr, &destinationRect);
}

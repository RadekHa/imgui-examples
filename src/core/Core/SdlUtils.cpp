#include "SdlUtils.h"
#include "TraceLog/Log.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace Sdl;

namespace details
{
    /** A deleter for stb image. */
    struct StbImageDeleter
    {
        void operator() (unsigned char* ptr) const
        {
            if (ptr)
            {
                stbi_image_free (ptr);
            }
        }
    };

    /** RAII smart pointer for stb image. */
    using StbImagePtr = unique_ptr<unsigned char, StbImageDeleter>;
}


SdlTexturePtr App::LoadTexture (SDL_Renderer* renderer, const char* filePath)
{
    SdlTexturePtr texture;
    int width = 0;
    int height = 0;
    int bytesPerPixel = 0;

    // Check parameters.
    if (!renderer)
    {
        APP_ERROR ("LoadTexture: renderer parameter cannot be null");
        return nullptr;
    }

    if (!filePath)
    {
        APP_ERROR ("LoadTexture: filePath parameter cannot be null");
        return nullptr;
    }
    // Load pixels.
    auto rawPixels = details::StbImagePtr{stbi_load (filePath, &width, &height, &bytesPerPixel, STBI_rgb_alpha)};

    if (!rawPixels)
    {
        APP_ERROR ("STB Image failed to load '{}': {}", filePath, stbi_failure_reason ());
        return nullptr;
    }
    APP_INFO ("Loaded image '{}' ({}x{}, original channels: {})", filePath, width, height, bytesPerPixel);

    texture = SdlTexturePtr{
        SDL_CreateTexture (
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STATIC,
            width,
            height
            )
    };

    if (!texture)
    {
        APP_ERROR ("Failed to create texture from '{}': {}", filePath, SDL_GetError ());
        return nullptr;
    }
    int pitch = width * 4;

    if (SDL_UpdateTexture (texture.get (), nullptr, rawPixels.get (), pitch) < 0)
    {
        APP_ERROR ("Failed to update texture from '{}': {}", filePath, SDL_GetError ());
        return nullptr;
    }

    if (SDL_SetTextureBlendMode (texture.get (), SDL_BLENDMODE_BLEND) < 0)
    {
        APP_WARN ("Failed to set blend mode for '{}': {}", filePath, SDL_GetError ());
    }
    return texture;
}

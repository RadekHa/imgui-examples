#include "Log.hpp"
#include "SdlUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace sdl;

namespace details
{
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

    using StbImagePtr = unique_ptr<unsigned char, StbImageDeleter>;
}


SdlTexturePtr App::LoadTexture (SDL_Renderer* renderer, const char* filePath)
{
    int width = 0;
    int height = 0;
    int bytesPerPixel = 0;

    auto rawPixels = details::StbImagePtr{stbi_load (filePath, &width, &height, &bytesPerPixel, STBI_rgb_alpha)};

    if (!rawPixels)
    {
        SDL_LogError (SDL_LOG_CATEGORY_APPLICATION,
                      "STB Image failed to load '%s': %s",
                      filePath,
                      stbi_failure_reason ());
        return nullptr;
    }
    SDL_LogInfo (SDL_LOG_CATEGORY_APPLICATION, "Loaded image '%s' (%dx%d, original channels: %d)",
                 filePath, width, height, bytesPerPixel);

    SDL_Texture* texture = SDL_CreateTexture (
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STATIC,
        width,
        height
        );

    if (texture == nullptr)
    {
        SDL_LogError (SDL_LOG_CATEGORY_RENDER, "Failed to create texture from '%s': %s",
                      filePath, SDL_GetError ());

        return nullptr;
    }
    int pitch = width * 4;
    SDL_UpdateTexture (texture, nullptr, rawPixels.get (), pitch);

    SDL_SetTextureBlendMode (texture, SDL_BLENDMODE_BLEND);

    return SdlTexturePtr{texture};
}

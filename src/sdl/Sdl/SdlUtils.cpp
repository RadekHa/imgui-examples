#include "Image/IImageData.h"
#include "SdlUtils.h"
#include "TraceLog/Log.hpp"

using namespace std;
using namespace Sdl;

///////////////////////////////////////////////////////////////////////////////
// Function definition.

SdlTexturePtr App::LoadTexture (SDL_Renderer* renderer, string_view filePath)
{
    SdlTexturePtr texture;

    // Check parameters.
    if (!renderer)
    {
        APP_ERROR ("LoadTexture: renderer parameter cannot be null");
        return nullptr;
    }

    if (filePath.empty ())
    {
        APP_ERROR ("LoadTexture: filePath parameter cannot be null");
        return nullptr;
    }
    // Load pixels.
    constexpr int desiredChannels = 4;
    auto img = Image::loadImage (filePath, desiredChannels);

    if (!img)
    {
        return nullptr;
    }
    APP_INFO ("Loaded image '{}' ({}x{}, channels: {})",
              filePath,
              img->getWidth (),
              img->getHeight (),
              img->getChannels ());

    texture = SdlTexturePtr{
        SDL_CreateTexture (
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STATIC,
            img->getWidth (),
            img->getHeight ()
            )
    };

    if (!texture)
    {
        APP_ERROR ("Failed to create texture from '{}': {}", filePath, SDL_GetError ());
        return nullptr;
    }
    int pitch = img->getWidth () * desiredChannels;

    if (SDL_UpdateTexture (texture.get (), nullptr, img->getPixels (), pitch) < 0)
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

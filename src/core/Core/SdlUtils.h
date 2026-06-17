#pragma once

#include "SdlWrapper.h"

namespace App
{
    /** Loads an image from the specified file path and creates an SDL texture from it. */
    Sdl::SdlTexturePtr LoadTexture (SDL_Renderer* renderer, const char* filePath);
}

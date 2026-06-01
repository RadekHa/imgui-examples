#pragma once
#include <memory>

#include <SDL.h>

namespace sdl
{
    /** Base template for SDL deleter. */
    template<typename T>
    struct SdlDeleter;

    /** Generic RAII smart pointer for SDL resources. */
    template<typename T>
    using SdlResource = std::unique_ptr<T, SdlDeleter<T> >;

    /** A deleter for SDL text. */
    template<>
    struct SdlDeleter<char>
    {
        void operator() (char* p) const noexcept
        {
            SDL_free (p);
        }
    };

    /** A deleter for SDL surface. */
    template<>
    struct SdlDeleter<SDL_Surface>
    {
        void operator() (SDL_Surface* s) const noexcept
        {
            SDL_FreeSurface (s);
        }
    };

    /** A deleter for SDL texture. */
    template<>
    struct SdlDeleter<SDL_Texture>
    {
        void operator() (SDL_Texture* t) const noexcept
        {
            SDL_DestroyTexture (t);
        }
    };

    /** A deleter for SDL window. */
    template<>
    struct SdlDeleter<SDL_Window>
    {
        void operator() (SDL_Window* w) const noexcept
        {
            SDL_DestroyWindow (w);
        }
    };

    /** Smart pointer type for SDL2 text. */
    using SdlTextPtr = SdlResource<char>;
    /** Smart pointer type for SDL2 surface. */
    using SdlSurfacePtr = SdlResource<SDL_Surface>;
    /** Smart pointer type for SDL2 texture. */
    using SdlTexturePtr = SdlResource<SDL_Texture>;
    /** Smart pointer type for SDL2 window. */
    using SdlWindowPtr = SdlResource<SDL_Window>;
}

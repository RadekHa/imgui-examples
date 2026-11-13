#pragma once
#include <memory>

/** Forward declarations of SDL opaque types, keeping this header SDL-free. */
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Surface;
typedef struct SDL_Window SDL_Window;

namespace Sdl
{
    /** Destruction helpers for SDL resources, implemented in SdlWrapper.cpp. */
    void destroyWindow (SDL_Window* w) noexcept;
    void destroyRenderer (SDL_Renderer* r) noexcept;
    void destroyTexture (SDL_Texture* t) noexcept;
    void destroySurface (SDL_Surface* s) noexcept;
    void freeText (char* p) noexcept;

    /** Base template for SDL deleter. */
    template<typename T>
    struct SdlDeleter;

    /** A deleter for SDL text. */
    template<>
    struct SdlDeleter<char>
    {
        void operator() (char* p) const noexcept
        {
            freeText (p);
        }
    };

    /** A deleter for SDL surface. */
    template<>
    struct SdlDeleter<SDL_Surface>
    {
        void operator() (SDL_Surface* s) const noexcept
        {
            destroySurface (s);
        }
    };

    /** A deleter for SDL texture. */
    template<>
    struct SdlDeleter<SDL_Texture>
    {
        void operator() (SDL_Texture* t) const noexcept
        {
            destroyTexture (t);
        }
    };

    /** A deleter for SDL window. */
    template<>
    struct SdlDeleter<SDL_Window>
    {
        void operator() (SDL_Window* w) const noexcept
        {
            destroyWindow (w);
        }
    };

    /** A deleter for SDL renderer. */
    template<>
    struct SdlDeleter<SDL_Renderer>
    {
        void operator() (SDL_Renderer* r) const noexcept
        {
            destroyRenderer (r);
        }
    };

    /** Generic RAII smart pointer for SDL resources. */
    template<typename T>
    using SdlResource = std::unique_ptr<T, SdlDeleter<T> >;

    /** Smart pointer type for SDL3 text. */
    using SdlTextPtr = SdlResource<char>;
    /** Smart pointer type for SDL3 surface. */
    using SdlSurfacePtr = SdlResource<SDL_Surface>;
    /** Smart pointer type for SDL3 texture. */
    using SdlTexturePtr = SdlResource<SDL_Texture>;
    /** Smart pointer type for SDL3 window. */
    using SdlWindowPtr = SdlResource<SDL_Window>;
    /** Smart pointer type for SDL3 renderer. */
    using SdlRendererPtr = SdlResource<SDL_Renderer>;
}

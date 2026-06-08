#pragma once
#include "SdlWrapper.h"

namespace App
{
    /* Forward declaration of FrameContext struct. */
    struct FrameContext;

    /* Class responsible for managing the SDL renderer and handling rendering operations. */
    class SdlRenderer
    {
    public:
        /* Initializes the renderer with a given SDL window. */
        explicit SdlRenderer (SDL_Window* window);
        /* Cleaning up resources. */
        ~SdlRenderer ();

        /** Begins a new frame for rendering. */
        void beginFrame (const FrameContext& ctx);
        /** Ends the current frame for rendering. */
        void endFrame ();
        /** Returns the native SDL renderer pointer. */
        SDL_Renderer* native () const;

    private:
        /* The native SDL renderer pointer wrapped in a smart pointer for automatic resource management. */
        sdl::SdlRendererPtr m_renderer;
    };
}

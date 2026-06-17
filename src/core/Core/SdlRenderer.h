#pragma once
#include "DataModel.h"
#include "IPathService.h"
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
        SdlRenderer (SDL_Window* window, const IPathService* paths);
        /* Cleaning up resources. */
        ~SdlRenderer ();

        /** Begins a new frame for rendering. */
        void beginFrame (const FrameContext& ctx);
        /** Ends the current frame for rendering. */
        void endFrame ();
        /** Returns the native SDL renderer pointer. */
        SDL_Renderer* native () const;
        /** Draw background images to renderer. */
        void update (DataModel& model);

    private:
        /** The native SDL renderer pointer wrapped in a smart pointer for automatic resource management. */
        Sdl::SdlRendererPtr m_renderer;
        /** Texture with image to draw. */
        Sdl::SdlTexturePtr m_background;
    };
}

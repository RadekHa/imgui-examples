#pragma once
#include "Types/DataModel.h"
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
        /* Initializes the renderer with a given SDL window.
         * @param window The SDL window to create the renderer for.
         * @param paths Path service for loading resources. */
        SdlRenderer (SDL_Window* window, const IPathService* paths);
        /* Cleans up the renderer resources. */
        ~SdlRenderer ();

        /** Begins a new frame for rendering.
         * @param ctx Frame context containing scaling information. */
        void beginFrame (const FrameContext& ctx);
        /** Ends the current frame and presents the rendered output. */
        void endFrame ();
        /** Returns the native SDL renderer pointer.
         * @return Pointer to the SDL_Renderer. */
        SDL_Renderer* native () const;
        /** Draws the background image to the renderer using the data model.
         * @param model Application data model containing camera image info. */
        void update (DataModel& model);

    private:
        /** The native SDL renderer pointer wrapped in a smart pointer for automatic resource management. */
        Sdl::SdlRendererPtr m_renderer;
        /** Texture with the background image to draw. */
        Sdl::SdlTexturePtr m_background;
    };
}

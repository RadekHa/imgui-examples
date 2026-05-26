#pragma once

/** Forward class declaration of SDL window. */
typedef struct SDL_Window SDL_Window;
/** Forward class declaration of SDL renderer. */
struct SDL_Renderer;

namespace App
{
    /* Forward declaration of FrameContext struct. */
    struct FrameContext;

    /* Class responsible for managing the SDL renderer and handling rendering operations. */
    class Renderer
    {
    public:
        /* Initializes the renderer with a given SDL window. */
        explicit Renderer (SDL_Window* window);
        /* Cleaning up resources. */
        ~Renderer ();

        /** Begins a new frame for rendering. */
        void beginFrame (const FrameContext& ctx);
        /** Ends the current frame for rendering. */
        void endFrame ();
        /** Returns the native SDL renderer pointer. */
        SDL_Renderer* native () const;

    private:
        /* The native SDL renderer pointer. */
        SDL_Renderer* m_renderer;
    };
}

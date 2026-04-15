#pragma once

/** Forward class declaration of SDL window. */
typedef struct SDL_Window SDL_Window;
/** Forward class declaration of SDL renderer. */
struct SDL_Renderer;

namespace App
{
    struct FrameContext;

    class Renderer
    {
    public:

        explicit Renderer (SDL_Window* window);

        ~Renderer ();

        void beginFrame (const FrameContext& ctx);

        void endFrame ();

        SDL_Renderer* native () const;

    private:

        SDL_Renderer* m_renderer;
    };
}

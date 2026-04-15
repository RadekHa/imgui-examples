#pragma once
#include "IPathService.h"

/** Forward class declaration of SDL window. */
typedef struct SDL_Window SDL_Window;
/** Forward class declaration of SDL renderer. */
struct SDL_Renderer;

struct ImGuiIO;

namespace App
{
    struct FrameContext;

    struct ImGuiConfig;

    class ImGuiPass
    {
    public:

        ImGuiPass (SDL_Window* window, SDL_Renderer* renderer, const IPathService* paths);

        ~ImGuiPass ();

        void beginFrame ();

        void endFrame ();

        void fillFrameContext (FrameContext& ctx) const;

    private:
        void applyPaths (const IPathService* paths);

        SDL_Renderer* m_renderer;
    };
}

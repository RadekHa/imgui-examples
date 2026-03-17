#pragma once

/** Forward class declaration of SDL window. */
typedef struct SDL_Window SDL_Window;
/** Forward class declaration of SDL renderer. */
struct SDL_Renderer;

namespace App
{
    class ImGuiPass
    {
    public:

        ImGuiPass (SDL_Window* window, SDL_Renderer* renderer);

        ~ImGuiPass ();

        void begin ();

        void end ();

    private:

        SDL_Renderer* m_renderer{};
    };
}

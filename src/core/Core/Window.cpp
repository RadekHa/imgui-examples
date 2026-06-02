#include "DPIHandler.hpp"
#include "Log.hpp"
#include "Window.hpp"

using namespace App;
using namespace std;


Window::Window (const string& title)
    : m_context {new SdlContext}
{
    const auto flags = static_cast<SDL_WindowFlags> (SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    Window::Settings settings;
    const WindowSize size{DPIHandler::get_dpi_aware_window_size (settings)};

    m_window = SDL_CreateWindow (
        title.c_str (),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        size.width,
        size.height,
        flags);

    if (!m_window)
    {
        throw runtime_error (SDL_GetError ());
    }
}

Window::~Window ()
{
    SDL_DestroyWindow (m_window);
}

SDL_Window* Window::native () const
{
    return m_window;
}

void Window::pollEvents (vector<SDL_Event>& events) const
{
    events.clear ();

    SDL_Event e;

    while (SDL_PollEvent (&e))
    {
        events.push_back (e);
    }
}

#include "DpiHandler.h"
#include "Log.hpp"
#include "Window.hpp"

using namespace App;
using namespace std;


Window::Window (const string& title)
    : m_scale {DpiHandler::getScale (0)}
{
    const auto flags = static_cast<SDL_WindowFlags> (SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    Window::Settings settings;

    m_window = SDL_CreateWindow (
        title.c_str (),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        settings.width * m_scale,
        settings.height * m_scale,
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

void Window::resize (float scale)
{
    if (m_scale != scale)
    {
        int width, height;
        SDL_GetWindowSize (m_window, &width, &height);

        int newWidth = static_cast<int> (width * (scale / m_scale));
        int newHeight = static_cast<int> (height * (scale / m_scale));

        APP_INFO ("Scale update: {} -> {}. Resizing window to {}x{}",
                  m_scale, scale, newWidth, newHeight);

        m_scale = scale;
        SDL_SetWindowSize (m_window, newWidth, newHeight);
    }
}

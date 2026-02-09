#define SDL_MAIN_HANDLED

#include <exception>

#include "Core/Application.hpp"
#include "Core/Log.hpp"
#include "tracy/Tracy.hpp"

int main ()
{
    try
    {
        ZoneScoped;

        App::Application app{"App"};
        app.run ();
    }
    catch (std::exception& e)
    {
        APP_ERROR ("Main process terminated with: {}", e.what ());
    }

    return 0;
}

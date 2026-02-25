#define SDL_MAIN_HANDLED

#include "Core/Application.hpp"
#include "Core/Log.hpp"
#include "tracy/Tracy.hpp"

#include <exception>

using namespace App;
using namespace std;

int main ()
{
    ZoneScoped;

    ExitStatus status{ExitStatus::SUCCESS};

    try
    {
        Application app{"App"};

        status = app.run ();
    }
    catch (exception& e)
    {
        APP_ERROR ("Main process terminated with: {}", e.what ());
        status = ExitStatus::FAILURE;
    }
    catch (...)
    {
        APP_ERROR ("Fatal unknown error.");
        status = ExitStatus::FAILURE;
    }

    return static_cast<int> (status);
}

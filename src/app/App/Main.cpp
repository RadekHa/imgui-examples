#define SDL_MAIN_HANDLED
#include "Core/Application.hpp"
#include "Core/IPathService.h"
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
        unique_ptr<IPathService> paths = createSdlPathService ();
        Application app{"App", paths.get ()};
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

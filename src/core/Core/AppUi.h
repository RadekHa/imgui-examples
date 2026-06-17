#pragma once
#include "DataModel.h"

namespace Sdl
{
    /** Forward declaration of sdl camera texture. */
    class SdlCameraTexture;
}

namespace App
{
    /** User Interface for the Application */
    class AppUi
    {
    public:
        /** Updates the UI based on the given data model. */
        void update (DataModel& model, const Sdl::SdlCameraTexture* camera);

    private:
        /** Draws a graph of the FPS over time. */
        void drawFpsGraph ();
    };
}

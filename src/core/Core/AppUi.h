#pragma once
#include "DataModel.h"

#include <memory>

namespace Sdl
{
    /** Forward declaration of sdl camera texture. */
    class SdlCameraTexture;
}

namespace App
{
    /** Forward declaration of the state interface. */
    class IUiState;

    /** User Interface for the Application */
    class AppUi
    {
    public:
        /** Initialize members. */
        AppUi ();
        /** Destructor for deleting an incomplete class pointer. */
        ~AppUi ();

        /** Updates the UI based on the given data model. */
        void update (DataModel& model, const Sdl::SdlCameraTexture* camera);

    private:
        /** Draws a graph of the FPS over time. */
        void drawFpsGraph ();
        /** Current state of the UI. */
        std::unique_ptr<IUiState> m_state;
    };
}

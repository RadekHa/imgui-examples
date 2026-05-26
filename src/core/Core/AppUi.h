#pragma once
#include "DataModel.h"

namespace App
{
    /** User Interface for the Application */
    class AppUi
    {
    public:
        /** Updates the UI based on the given data model. */
        void update (DataModel& model);

    private:
        /** Draws a graph of the FPS over time. */
        void drawFpsGraph ();
    };
}

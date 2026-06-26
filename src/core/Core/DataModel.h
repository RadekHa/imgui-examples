#pragma once

namespace App
{
    /** Struct representing the data model for the application. */
    struct DataModel
    {
        /** Flag indicating whether to show the ImGui demo window. */
        bool showDemo{true};

        /** Start time after closing of the initial dialog. */
        double startTime{};
    };
}

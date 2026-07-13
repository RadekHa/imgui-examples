#pragma once
#include "Ui/ImageInfo.h"

namespace App
{
    /** Struct representing the data model for the application. */
    struct DataModel
    {
        /** Flag indicating whether to show the ImGui demo window. */
        bool showDemo{true};
        /** Camera frame information for UI rendering. */
        Ui::ImageInfo camera{};
    };
}

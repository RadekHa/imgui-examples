#pragma once
#include "Types/CameraInfo.h"
#include "Types/ImageInfo.h"
#include <vector>

// Convention: the Types module holds shared value types with no dependencies.
// Namespaces mirror the logical owner of the type (App, Camera, Ui), not a
// dependency on those modules.

namespace App
{
    /** Struct representing the data model for the application. */
    struct DataModel
    {
        /** Flag indicating whether to show the ImGui demo window. */
        bool showDemo{true};
        /** Camera frame information for UI rendering. */
        Ui::ImageInfo camera{};
        /** List of available cameras. */
        std::vector<Camera::CameraInfo> cameras{};
        /** Currently selected camera index. */
        int selectedCameraIndex{0};
    };
}

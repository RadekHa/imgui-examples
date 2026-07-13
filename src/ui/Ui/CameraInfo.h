#pragma once

#include <cstdint>

namespace Ui
{
    /** Struct representing camera frame information for UI rendering. */
    struct CameraInfo
    {
        /** Pointer to the ImGui texture ID for the camera frame. */
        void* textureId{nullptr};
        /** Width of the camera frame in pixels. */
        int width{};
        /** Height of the camera frame in pixels. */
        int height{};
        /** Flag indicating whether the camera frame data is valid. */
        bool valid{};
    };
}

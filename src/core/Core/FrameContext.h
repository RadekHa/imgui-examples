#pragma once

namespace App
{
    /** Context structure for a single frame, containing scaling factors for rendering. */
    struct FrameContext
    {
        /** Scaling factor for the X-axis. Default is 1.0 (no scaling). */
        float scaleX{1.0f};
        /** Scaling factor for the Y-axis. Default is 1.0 (no scaling). */
        float scaleY{1.0f};
    };
}

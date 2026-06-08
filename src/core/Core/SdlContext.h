#pragma once

namespace App
{
    /* Class responsible for managing the SDL context. */
    struct SdlContext
    {
        /** Initializes the SDL. */
        SdlContext ();
        /** Cleans up the SDL resources. */
        ~SdlContext ();
        /** The copy constructor is deleted to prevent copying of the SdlContext. */
        SdlContext (const SdlContext&) = delete;
        /** The copy assignment operator is deleted to prevent copying of the SdlContext. */
        SdlContext& operator= (const SdlContext&) = delete;
    };
}

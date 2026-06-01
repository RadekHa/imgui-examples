#pragma once

namespace App
{
    /* Class responsible for managing the SDL context. */
    struct SDLContext
    {
        /** Initializes the SDL. */
        SDLContext ();
        /** Cleans up the SDL resources. */
        ~SDLContext ();
        /** The copy constructor is deleted to prevent copying of the SDLContext. */
        SDLContext (const SDLContext&) = delete;
        /** The copy assignment operator is deleted to prevent copying of the SDLContext. */
        SDLContext& operator= (const SDLContext&) = delete;
    };
}

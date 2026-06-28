#pragma once
#include "DataModel.h"
#include "serialib.h"

#include <memory>
#include <vector>

namespace Sdl
{
    /** Forward declaration of sdl camera texture. */
    class SdlCameraTexture;
}

namespace Ui
{
    /** Declaration of the state interface. */
    class IUiState
    {
    public:
        /** Virtual destructor to allow proper cleanup of derived classes. */
        virtual ~IUiState () = default;
        /** Update UI elements. */
        virtual IUiState* update (App::DataModel& model, const Sdl::SdlCameraTexture* camera) = 0;
    };

    /** User Interface for the Application */
    class AppUi
    {
    public:
        /** Initialize members. */
        AppUi ();
        /** Destructor for deleting an incomplete class pointer. */
        ~AppUi ();

        /** Updates the UI based on the given data model. */
        void update (App::DataModel& model, const Sdl::SdlCameraTexture* camera);

    private:
        /** Current state of the UI. */
        std::vector<std::unique_ptr<IUiState> > m_states;
    };
}

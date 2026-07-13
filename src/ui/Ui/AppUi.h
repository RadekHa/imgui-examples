#pragma once
#include "ImageInfo.h"

#include <memory>

namespace App
{
    struct DataModel;
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
        virtual IUiState* update (App::DataModel& model) = 0;
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
        void update (App::DataModel& model);

    private:
        /** Current state of the UI. */
        std::unique_ptr<IUiState> m_state;
    };
}

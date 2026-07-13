#pragma once

namespace App
{
    struct DataModel;
}

namespace Ui
{
    /** Interface for application state machine states. */
    class IUiState
    {
    public:
        /** Virtual destructor for proper cleanup of derived classes. */
        virtual ~IUiState () = default;
        /** Updates the state and returns a pointer to the next state, or nullptr to stay in the current state. */
        virtual IUiState* update (App::DataModel& model) = 0;
    };
}

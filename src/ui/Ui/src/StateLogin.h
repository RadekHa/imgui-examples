#pragma once
#include "IUiState.h"

#include <string_view>

namespace Ui
{
    /** Login dialog state that prompts for user credentials. */
    class StateLogin : public IUiState
    {
    public:
        /** {@inheritDoc} */
        IUiState* update (App::DataModel& model) override;

    private:
        /** Checks if the provided credentials are valid. */
        bool isValid (std::string_view userName, std::string_view password) const;

        /** Buffer for the user name input. */
        char m_username[64]{};
        /** Buffer for the password input. */
        char m_password[64]{};
        /** Flag indicating the last login attempt failed. */
        bool m_loginFailed{};
    };
}

#pragma once
#include "Ui/IAppUi.h"
#include "Ui/ImageInfo.h"

#include <memory>
#include <string_view>

namespace Ui
{
    class IUiState
    {
    public:
        virtual ~IUiState () = default;
        virtual IUiState* update (App::DataModel& model) = 0;
    };

    class AppUi final : public IAppUi
    {
    public:
        AppUi ();
        ~AppUi () override;

        void update (App::DataModel& model) override;

    private:
        std::unique_ptr<IUiState> m_state;
    };
}

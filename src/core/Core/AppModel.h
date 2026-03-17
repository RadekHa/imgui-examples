#pragma once
#include "IDataModel.h"

namespace App
{
    class AppModel : public IDataModel
    {
    public:
        /* Initializes the application data model. */
        AppModel ();

        bool isShowSomePanel () const override;
        bool isShowDemoPanel () const override;
        bool isShowDebugPanel () const override;

        void setShowSomePanel (bool value) override;
        void setShowDemoPanel (bool value) override;
        void setShowDebugPanel (bool value) override;

    private:
        bool m_showSomePanel;
        bool m_showDemoPanel;
        bool m_showDebugPanel;
    };
}

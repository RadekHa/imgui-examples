#include "AppModel.h"

using namespace App;

AppModel::AppModel ()
    : m_showSomePanel {true}
    , m_showDemoPanel {true}
    , m_showDebugPanel {true}
{
}

bool AppModel::isShowSomePanel () const
{
    return m_showSomePanel;
}

bool AppModel::isShowDemoPanel () const
{
    return m_showDemoPanel;
}

bool AppModel::isShowDebugPanel () const
{
    return m_showDebugPanel;
}

void AppModel::setShowSomePanel (bool value)
{
    m_showSomePanel = value;
}

void AppModel::setShowDemoPanel (bool value)
{
    m_showDemoPanel = value;
}

void AppModel::setShowDebugPanel (bool value)
{
    m_showDebugPanel = value;
}

#include "AppUi.h"
#include "Types/DataModel.h"

#include "imgui.h"

#include <string_view>

using namespace App;
using namespace std;
using namespace Ui;


///////////////////////////////////////////////////////////////////////////////
// StateNull

/** Null state. */
class StateNull : public IUiState
{
public:
    /** {@inheritDoc} */
    virtual IUiState* update (App::DataModel& model) override
    {
        return nullptr;
    }
};


///////////////////////////////////////////////////////////////////////////////
// StateLogin

/** Login window. */
class StateLogin : public IUiState
{
public:

    /** {@inheritDoc} */
    virtual IUiState* update (App::DataModel& model) override;

private:
    /** Check if the login is correct. */
    bool isValid (string_view userName, string_view password) const;

    /** User name value.*/
    char m_username [64] = "";
    /** Password value.*/
    char m_password [64] = "";
    /** Flag indicating that login is correct.*/
    bool m_loginFailed{};
};

IUiState* StateLogin::update (App::DataModel& model)
{
    IUiState* state = nullptr;

    constexpr string_view popupTitle = "Přihlášení do systému";

    ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
    ImGui::SetNextWindowPos (center, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));
    ImGui::SetNextWindowFocus ();

    ImGui::OpenPopup (popupTitle.data ());

    if (ImGui::BeginPopupModal (popupTitle.data (), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text ("Uživatelské jméno:");

        if (ImGui::InputText ("##username", m_username, IM_ARRAYSIZE (m_username)))
        {
            m_loginFailed = false;
        }
        ImGui::Text ("Heslo:");

        if (ImGui::InputText ("##password", m_password, IM_ARRAYSIZE (m_password), ImGuiInputTextFlags_Password))
        {
            m_loginFailed = false;
        }
        ImGui::Separator ();

        if (ImGui::Button ("Přihlásit se", ImVec2 (120, 0)))
        {
            if (isValid (m_username, m_password))
            {
                ImGui::CloseCurrentPopup ();
                state = new StateNull;
            }
            else
            {
                m_loginFailed = true;
            }
        }
        ImGui::SetItemDefaultFocus ();

        if (m_loginFailed)
        {
            ImGui::Spacing ();
            ImGui::TextColored (ImVec4 (1, 0.3f, 0.3f, 1), "Neplatné přihlašovací údaje.");
        }
        ImGui::EndPopup ();
    }
    return state;
}

bool StateLogin::isValid (string_view userName, string_view password) const
{
    return userName == "user" && password == "pwd";
}

///////////////////////////////////////////////////////////////////////////////
// StateStart

class StateStart : public IUiState
{
public:
    /** {@inheritDoc} */
    virtual IUiState* update (App::DataModel& model) override;
};

IUiState* StateStart::update (App::DataModel& model)
{
    IUiState* state = nullptr;

    ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
    ImGui::SetNextWindowPos (center, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));
    ImGui::SetNextWindowFocus ();

    ImGui::OpenPopup ("Start");

    if (ImGui::BeginPopupModal ("Start", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text ("All those beautiful files will be deleted.\nThis operation cannot be undone!");
        ImGui::Separator ();

        if (ImGui::Button ("OK", ImVec2 (120, 0)))
        {
            ImGui::CloseCurrentPopup ();

            state = new StateLogin;
        }
        ImGui::EndPopup ();
    }
    return state;
}

///////////////////////////////////////////////////////////////////////////////
// AppUi

AppUi::AppUi ()
    : m_state {new StateStart}
{
}

AppUi::~AppUi () = default;

namespace Ui
{
    IAppUi* createAppUi ()
    {
        return new AppUi;
    }
}

void AppUi::update (App::DataModel& model)
{
    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGui::DockSpaceOverViewport (0, ImGui::GetMainViewport (), dockspaceFlags);

    if (ImGui::BeginMainMenuBar ())
    {
        if (ImGui::BeginMenu ("View"))
        {
            ImGui::MenuItem ("Demo", nullptr, &model.showDemo);
            ImGui::EndMenu ();
        }
        ImGui::EndMainMenuBar ();
    }

    if (model.showDemo)
    {
        ImGui::ShowDemoWindow (&model.showDemo);
    }

    if (model.camera.valid)
    {
        ImGui::Begin ("Camera");

        ImVec2 available = ImGui::GetContentRegionAvail ();

        float texW = float (model.camera.width);
        float texH = float (model.camera.height);

        float scale = std::min (available.x / texW, available.y / texH);
        ImVec2 size = ImVec2 (texW * scale, texH * scale);

        ImVec2 cursor = ImGui::GetCursorPos ();

        ImVec2 pos = ImVec2 (cursor.x + (available.x - size.x) * 0.5f,
                             cursor.y + (available.y - size.y) * 0.5f);

        ImGui::SetCursorPos (pos);

        ImGui::Image (model.camera.textureId,
                      size,
                      ImVec2 (1, 0),
                      ImVec2 (0, 1));
        ImGui::End ();
    }
    IUiState* state = m_state->update (model);

    if (state != nullptr)
    {
        m_state.reset (state);
    }
}

#include "StateLogin.h"
#include "StateNull.h"
#include "Types/DataModel.h"

#include "imgui.h"

using namespace App;
using namespace std;
using namespace Ui;

IUiState* StateLogin::update (App::DataModel& /*model*/)
{
    IUiState* state = nullptr;

    constexpr string_view popupTitle = "P\u0159ihl\u00e1\u0161en\u00ed do syst\u00e9mu";

    ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
    ImGui::SetNextWindowPos (center, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));
    ImGui::SetNextWindowFocus ();

    ImGui::OpenPopup (popupTitle.data ());

    if (ImGui::BeginPopupModal (popupTitle.data (), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text ("U\u017eivatelsk\u00e9 jm\u00e9no:");

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

        if (ImGui::Button ("P\u0159ihl\u00e1sit se", ImVec2 (120, 0)))
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
            ImGui::TextColored (ImVec4 (1, 0.3f, 0.3f, 1), "Neplatn\u00e9 p\u0159ihla\u0161ovac\u00ed \u00fadaje.");
        }
        ImGui::EndPopup ();
    }
    return state;
}

bool StateLogin::isValid (string_view userName, string_view password) const
{
    return userName == "user" && password == "pwd";
}

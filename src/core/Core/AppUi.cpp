#include "AppUi.h"
#include "SdlCameraTexture.h"

#include "imgui.h"

#include <string_view>

using namespace App;
using namespace Sdl;
using namespace std;
using namespace Ui;

namespace details
{
    void HelpMarker (const char* desc)
    {
        ImGui::TextDisabled ("(?)");

        if (ImGui::BeginItemTooltip ())
        {
            ImGui::PushTextWrapPos (ImGui::GetFontSize () * 35.0f);
            ImGui::TextUnformatted (desc);
            ImGui::PopTextWrapPos ();
            ImGui::EndTooltip ();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// StateCounter

class StateCounter : public IUiState
{
public:
    /** {@inheritDoc} */
    virtual IUiState* update (DataModel& model, const SdlCameraTexture* camera) override;
};


///////////////////////////////////////////////////////////////////////////////
// StateNull

/** Null state. */
class StateNull : public IUiState
{
public:
    /** {@inheritDoc} */
    virtual IUiState* update (DataModel& /*model*/, const SdlCameraTexture* /*camera*/) override
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
    virtual IUiState* update (DataModel& model, const SdlCameraTexture* camera) override;

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

///////////////////////////////////////////////////////////////////////////////
// StateStart

class StateStart : public IUiState
{
public:
    /** {@inheritDoc} */
    virtual IUiState* update (DataModel& model, const SdlCameraTexture* camera) override;
};

///////////////////////////////////////////////////////////////////////////////
// StateCamera

class StateCamera : public IUiState
{
public:
    /** {@inheritDoc} */
    virtual IUiState* update (DataModel& model, const SdlCameraTexture* camera) override;
};

///////////////////////////////////////////////////////////////////////////////
// StateSerial

class StateSerial : public IUiState
{
public:
    /** Initialize members. */
    StateSerial ();
    /** {@inheritDoc} */
    virtual IUiState* update (DataModel& model, const SdlCameraTexture* camera) override;
private:
    bool m_isConnected;
    bool m_deviceActive;
    bool m_soundEnabled;
    bool m_ledEnabled;
};


///////////////////////////////////////////////////////////////////////////////
// StateCounter

IUiState* StateCounter::update (DataModel& model, const SdlCameraTexture* /*camera*/)
{
    ImGui::SetNextWindowPos (ImVec2 (0.0f, 0.0f), ImGuiCond_Once);

    int min = 60;
    int sec = 0;

    if (model.startTime > 0)
    {
        auto currentTime = ImGui::GetTime ();
        auto runTime = currentTime - model.startTime;

        int rest = max (0, 60 * 60 - static_cast<int> (runTime));

        min = rest / 60;
        sec = rest % 60;
    }
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
                                   ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoDocking |
                                   ImGuiWindowFlags_AlwaysAutoResize;

    ImGui::Begin ("Odpočet", nullptr, windowFlags);
    ImGui::Text ("%02d:%02d", min, sec);
    ImGui::End ();

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// StateLogin

IUiState* StateLogin::update (DataModel& /*model*/, const SdlCameraTexture* /*camera*/)
{
    IUiState* state = nullptr;

    constexpr float TEXT_WIDTH = 150.0f;

    ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
    ImGui::SetNextWindowPos (center, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));

    constexpr string_view popupTitle = "Přihlášení do systému";
    ImGui::OpenPopup (popupTitle.data ());

    if (ImGui::BeginPopupModal (popupTitle.data (), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::BeginTable ("loginTable", 3), ImGuiTableFlags_SizingFixedFit)
        {
            ImGui::TableSetupColumn ("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn ("", ImGuiTableColumnFlags_WidthFixed, TEXT_WIDTH);
            ImGui::TableSetupColumn ("", ImGuiTableColumnFlags_WidthFixed);

            ImGui::TableNextRow ();
            ImGui::TableNextColumn ();

            ImGui::Text ("Uživatelské jméno:");

            ImGui::TableNextColumn ();
            ImGui::PushItemWidth (TEXT_WIDTH);

            if (ImGui::InputText ("##username", m_username, IM_ARRAYSIZE (m_username)))
            {
                m_loginFailed = false;
            }
            ImGui::PopItemWidth ();

            ImGui::TableNextColumn ();
            details::HelpMarker ("Uživatelské jméno s oprávněním administrátora.");

            ImGui::TableNextRow ();
            ImGui::TableNextColumn ();

            ImGui::Text ("Heslo:");

            ImGui::TableNextColumn ();
            ImGui::PushItemWidth (TEXT_WIDTH);

            if (ImGui::InputText ("##password", m_password, IM_ARRAYSIZE (m_password), ImGuiInputTextFlags_Password))
            {
                m_loginFailed = false;
            }
            ImGui::PopItemWidth ();

            ImGui::TableNextColumn ();
            details::HelpMarker ("Heslo musí obsahovat čísla, velká a malá písmena.");

            ImGui::EndTable ();
        }
        ImGui::Separator ();

        if (ImGui::Button ("Přihlásit se", ImVec2 (-1, 0)))
        {
            if (isValid (m_username, m_password))
            {
                ImGui::CloseCurrentPopup ();
                state = new StateCamera;
            }
            else
            {
                m_loginFailed = true;
            }
        }

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

IUiState* StateStart::update (DataModel& model, const SdlCameraTexture* /*camera*/)
{
    IUiState* state = nullptr;

    ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
    ImGui::SetNextWindowPos (center, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));

    constexpr string_view popupTitle = "Upozornění";

    ImGui::OpenPopup (popupTitle.data ());

    if (ImGui::BeginPopupModal (popupTitle.data (), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text ("Zařízení aktivováno!\nPro deaktivaci se přihlašte.");
        ImGui::Separator ();

        if (ImGui::Button ("OK", ImVec2 (120, 0)))
        {
            model.startTime = ImGui::GetTime ();
            ImGui::CloseCurrentPopup ();

            state = new StateLogin;
        }
        ImGui::EndPopup ();
    }
    return state;
}

///////////////////////////////////////////////////////////////////////////////
// StateCamera

IUiState* StateCamera::update (DataModel& /*model*/, const SdlCameraTexture* camera)
{
    IUiState* state = nullptr;

    if (camera && camera->isValid ())
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport ();

        float width = viewport->Size.x * (2.0f / 3.0f);
        float height = viewport->Size.y * (2.0f / 3.0f);

        ImGui::SetNextWindowSize (ImVec2 (width, height), ImGuiCond_Once);

        ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
        ImGui::SetNextWindowPos (center, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));

        ImGui::Begin ("Verifikace");
        ImGui::Text ("Usměj se :)");

        ImVec2 available = ImGui::GetContentRegionAvail ();

        float texW = float (camera->getWidth ());
        float texH = float (camera->getHeight ());

        float scale = std::min (available.x / texW, available.y / texH);
        ImVec2 size = ImVec2 (texW * scale, texH * scale);

        ImVec2 cursor = ImGui::GetCursorPos ();

        ImVec2 pos = ImVec2 (cursor.x + (available.x - size.x) * 0.5f,
                             cursor.y + (available.y - size.y) * 0.5f);

        ImGui::SetCursorPos (pos);

        ImGui::Image (camera->getImguiTextureId (),
                      size,
                      ImVec2 (1, 0),
                      ImVec2 (0, 1));

        if (ImGui::IsItemHovered () && ImGui::IsMouseDoubleClicked (ImGuiMouseButton_Left))
        {
            state = new StateNull;
        }
        ImGui::End ();
    }
    return state;
}

///////////////////////////////////////////////////////////////////////////////
// StateSerial

StateSerial::StateSerial ()
    : m_isConnected {}
    , m_deviceActive {true}
    , m_soundEnabled {true}
    , m_ledEnabled {}
{
}

IUiState* StateSerial::update (DataModel& model, const SdlCameraTexture* camera)
{
    static char deviceAddress [128] = "192.168.1.50";
    static int devicePort = 8080;

    ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
    ImGui::SetNextWindowPos (center, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));

    ImGui::Begin ("Správce zařízení");
    ImGui::TextDisabled ("1. NASTAVENÍ PŘIPOJENÍ");

    ImGui::BeginDisabled (m_isConnected);
    {
        if (ImGui::BeginTable ("ConnectionTable", 3, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn ("Label");
            ImGui::TableSetupColumn ("Input", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn ("Hint");

            ImGui::TableNextRow ();

            ImGui::TableNextColumn ();
            ImGui::AlignTextToFramePadding ();
            ImGui::Text ("Adresa / IP:");

            ImGui::TableNextColumn ();
            ImGui::SetNextItemWidth (-1.0f);
            ImGui::InputText ("##Adresa", deviceAddress, IM_ARRAYSIZE (deviceAddress));

            ImGui::TableNextColumn ();
            details::HelpMarker ("Sem prijde napoveda.");

            ImGui::TableNextRow ();

            ImGui::TableNextColumn ();
            ImGui::AlignTextToFramePadding ();
            ImGui::Text ("Port:");

            ImGui::TableNextColumn ();
            ImGui::SetNextItemWidth (-1.0f);
            ImGui::InputInt ("##Port", &devicePort, 0, 0);

            ImGui::TableNextColumn ();
            details::HelpMarker ("Sem prijde napoveda.");

            ImGui::TableSetColumnIndex (2);

            ImGui::EndTable ();
        }
    }
    ImGui::EndDisabled ();

    ImGui::Spacing ();

    if (!m_isConnected)
    {
        if (ImGui::Button ("Připojit k zařízení", ImVec2 (-1, 30)))
        {
            m_isConnected = true;
        }
    }
    else
    {
        ImGui::PushStyleColor (ImGuiCol_Button, ImVec4 (0.6f, 0.1f, 0.1f, 1.0f)); // Červené tlačítko pro odpojení

        if (ImGui::Button ("Odpojit", ImVec2 (-1, 30)))
        {
            m_isConnected = false;
        }
        ImGui::PopStyleColor ();
    }
    ImGui::Separator ();

    ImGui::TextDisabled ("2. OVLÁDÁNÍ ZAŘÍZENÍ");

    ImGui::BeginDisabled (!m_isConnected);
    {
        ImGui::Checkbox ("Zařízení aktivní", &m_deviceActive);

        ImGui::Spacing ();

        ImGui::BeginDisabled (!m_deviceActive);
        {
            ImGui::Checkbox ("Zapnout zvuk", &m_soundEnabled);
            ImGui::SameLine ();
            ImGui::Checkbox ("Zapnout LED", &m_ledEnabled);
        }
        ImGui::EndDisabled ();
    }
    ImGui::EndDisabled ();

    ImGui::End ();

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// AppUi

AppUi::AppUi ()
{
    m_states.emplace_back (new StateCounter);
    m_states.emplace_back (new StateSerial);
    m_states.emplace_back (new StateStart);
}

AppUi::~AppUi () = default;

void AppUi::update (DataModel& model, const SdlCameraTexture* camera)
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
    vector<unique_ptr<IUiState> > nextStates;
    nextStates.reserve (m_states.size ());

    for (auto& oldStatePtr : m_states)
    {
        unique_ptr<IUiState> currentState = move (oldStatePtr);

        if (!currentState)
        {
            continue;
        }
        IUiState* result = currentState->update (model, camera);

        if (result == nullptr)
        {
            nextStates.push_back (move (currentState));
        }
        else
        {
            nextStates.emplace_back (result);
        }
    }
    m_states = move (nextStates);
}

#include "AppUi.h"
#include "SdlCameraTexture.h"
#include "TraceLog/Log.hpp"

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

    void sendSerial (serialib* serial, const char* text)
    {
        char buffer [32] = {0};
        serial->writeString (text);
        serial->readString (buffer, '\n', 32, 2000);
    }

    void messageInfo (string_view popupTitle, string_view message)
    {
        // ImGui::OpenPopup (popupTitle.data ());

        ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
        ImGui::SetNextWindowPos (center, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));

        if (ImGui::BeginPopupModal (popupTitle.data (), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text (message.data ());
            ImGui::Separator ();

            if (ImGui::Button ("OK", ImVec2 (-1, 0)))
            {
                ImGui::CloseCurrentPopup ();
            }
            ImGui::EndPopup ();
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
    StateSerial (serialib* serial);
    /** {@inheritDoc} */
    virtual IUiState* update (DataModel& model, const SdlCameraTexture* camera) override;
private:
    bool isValid (string_view address, int port) const;

    bool m_isConnected;
    bool m_deviceActive;
    bool m_soundEnabled;
    bool m_ledEnabled;
    bool m_dispEnabled;
    bool m_loginFailed;
    serialib* m_serial;
};


///////////////////////////////////////////////////////////////////////////////
// StateCounter

IUiState* StateCounter::update (DataModel& model, const SdlCameraTexture* /*camera*/)
{
    ImGui::SetNextWindowPos (ImVec2 (0.0f, 0.0f), ImGuiCond_Once);

    int min = 60;
    int sec = 0;

    auto rest = static_cast<int> (model.startTime - ImGui::GetTime ());

    if (rest > 0)
    {
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
            details::HelpMarker ("Vaše heslo.");

            ImGui::EndTable ();
        }
        ImGui::Separator ();
        ImGui::Spacing ();

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
    return userName == "MATRIX" && password == "START5";
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

        if (ImGui::Button ("OK", ImVec2 (-1, 0)))
        {
            model.startTime = ImGui::GetTime () + 60 * 60;
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
        details::HelpMarker ("Usměj se :)");

        ImVec2 available = ImGui::GetContentRegionAvail ();

        float texW = float (camera->getWidth ());
        float texH = float (camera->getHeight ());

        float scale = (std::min) (available.x / texW, available.y / texH);
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

StateSerial::StateSerial (serialib* serial)
    : m_isConnected {}
    , m_deviceActive {true}
    , m_soundEnabled {}
    , m_ledEnabled {true}
    , m_dispEnabled {}
    , m_loginFailed {}
    , m_serial {serial}
{
}

IUiState* StateSerial::update (DataModel& model, const SdlCameraTexture* camera)
{
    static char deviceAddress [128] = "test";
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

            if (ImGui::InputText ("##Adresa", deviceAddress, IM_ARRAYSIZE (deviceAddress)))
            {
                m_loginFailed = false;
            }
            ImGui::TableNextColumn ();
            details::HelpMarker ("Sem prijde napoveda.");

            ImGui::TableNextRow ();

            ImGui::TableNextColumn ();
            ImGui::AlignTextToFramePadding ();
            ImGui::Text ("Port:");

            ImGui::TableNextColumn ();
            ImGui::SetNextItemWidth (-1.0f);

            if (ImGui::InputInt ("##Port", &devicePort, 0, 0))
            {
                m_loginFailed = false;
            }
            ImGui::TableNextColumn ();
            details::HelpMarker ("Sem prijde napoveda.");

            ImGui::EndTable ();
        }
    }
    ImGui::EndDisabled ();

    if (!m_isConnected)
    {
        if (ImGui::Button ("Připojit k zařízení", ImVec2 (-1, 0)))
        {
            m_isConnected = isValid (deviceAddress, devicePort);

            if (m_isConnected)
            {
                static bool first = true;

                if (first)
                {
                    first = false;
                    ImGui::OpenPopup ("Upozornění##1");

                    details::sendSerial (m_serial, "TOGGLE");
                    model.startTime = ImGui::GetTime () + 120.0;
                }
            }
            else
            {
                m_loginFailed = true;
            }
        }
    }
    else
    {
        ImGui::PushStyleColor (ImGuiCol_Button, ImVec4 (0.6f, 0.1f, 0.1f, 1.0f));

        if (ImGui::Button ("Odpojit", ImVec2 (-1, 0)))
        {
            m_isConnected = false;
        }
        ImGui::PopStyleColor ();
    }

    if (m_loginFailed)
    {
        ImGui::Spacing ();
        ImGui::TextColored (ImVec4 (1, 0.3f, 0.3f, 1), "Neplatné přihlašovací údaje.");
    }
    ImGui::Separator ();

    ImGui::TextDisabled ("2. OVLÁDÁNÍ ZAŘÍZENÍ");

    ImGui::BeginDisabled (!m_isConnected);
    {
        if (ImGui::Checkbox ("Zařízení aktivní", &m_deviceActive))
        {
            m_deviceActive = true;
            ImGui::OpenPopup ("Upozornění##2");
        }
        ImGui::Spacing ();

        ImGui::BeginDisabled (!m_deviceActive);
        {
            if (ImGui::Checkbox ("Zapnout zvuk", &m_soundEnabled))
            {
                if (m_soundEnabled)
                {
                    details::sendSerial (m_serial, "BEEP_ON");
                }
                else
                {
                    details::sendSerial (m_serial, "BEEP_OFF");
                }
            }
            ImGui::SameLine ();

            if (ImGui::Checkbox ("Zapnout LED", &m_ledEnabled))
            {
                if (m_ledEnabled)
                {
                    details::sendSerial (m_serial, "LEDS_ON");
                }
                else
                {
                    details::sendSerial (m_serial, "LEDS_OFF");
                }
            }
            ImGui::SameLine ();

            if (ImGui::Checkbox ("Zapnout Display", &m_dispEnabled))
            {
                if (m_dispEnabled)
                {
                    details::sendSerial (m_serial, "DISP_ON");
                }
                else
                {
                    details::sendSerial (m_serial, "DISP_OFF");
                }
            }
        }
        ImGui::EndDisabled ();
    }

    ImGui::EndDisabled ();

    details::messageInfo ("Upozornění##1", "Detekován podezřelý přístup.\nOdpočet nastaven na 2:00.");
    details::messageInfo ("Upozornění##2", "Dalková deaktivace není možná.\nVypněte na zařízení.");

    ImGui::End ();
    return nullptr;
}

bool StateSerial::isValid (string_view address, int port) const
{
    return address == "test" && port == 8080;
}

///////////////////////////////////////////////////////////////////////////////
// AppUi

AppUi::AppUi ()
{
    m_states.emplace_back (new StateCounter);
    m_states.emplace_back (new StateSerial (&m_serial));
    m_states.emplace_back (new StateStart);

    char status = m_serial.openDevice ("COM4", 9600);

    if (status != 1)
    {
        APP_ERROR ("Cannot open COM port");
    }
}

AppUi::~AppUi ()
{
    m_serial.closeDevice ();
}

void AppUi::update (DataModel& model, const SdlCameraTexture* camera)
{
    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGui::DockSpaceOverViewport (0, ImGui::GetMainViewport (), dockspaceFlags);

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

#include "AppUi.h"
#include "SdlCameraTexture.h"

#include "imgui.h"

#include <stdio.h>
#include <string_view>

using namespace App;
using namespace Sdl;
using namespace std;


/** Declaration of the state interface. */
class App::IUiState
{
public:
    /** Virtual destructor to allow proper cleanup of derived classes. */
    virtual ~IUiState () = default;
    /** Update UI elements. */
    virtual IUiState* update (DataModel& model, const SdlCameraTexture* camera) = 0;
};

/** Null state. */
class StateNull : public IUiState
{
public:
    /** {@inheritDoc} */
    virtual IUiState* update (DataModel& model, const SdlCameraTexture* camera) override
    {
        return nullptr;
    }
};

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


IUiState* StateLogin::update (DataModel& model, const SdlCameraTexture* camera)
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

class StateStart : public IUiState
{
public:
    /** {@inheritDoc} */
    virtual IUiState* update (DataModel& model, const SdlCameraTexture* camera) override;
};

IUiState* StateStart::update (DataModel& model, const SdlCameraTexture* camera)
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

        static bool dont_ask_me_next_time = false;

        ImGui::PushStyleVar (ImGuiStyleVar_FramePadding, ImVec2 (0, 0));
        ImGui::Checkbox ("Don't ask me next time", &dont_ask_me_next_time);
        ImGui::PopStyleVar ();

        if (ImGui::Button ("OK", ImVec2 (120, 0)))
        {
            ImGui::CloseCurrentPopup ();

            state = new StateLogin;
        }
        ImGui::EndPopup ();
    }
    return state;
}

AppUi::AppUi ()
    : m_state {new StateStart}
{
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
            ImGui::MenuItem ("Stats", nullptr, &model.showStats);
            ImGui::EndMenu ();
        }
        ImGui::EndMainMenuBar ();
    }

    if (model.showDemo)
    {
        ImGui::ShowDemoWindow (&model.showDemo);
    }

    if (camera && camera->isValid ())
    {
        ImGui::Begin ("Camera");

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
        ImGui::End ();
    }
    IUiState* state = m_state->update (model, camera);

    if (state != nullptr)
    {
        m_state.reset (state);
    }
}

void AppUi::drawFpsGraph ()
{
    static float values [90]{};
    static int offset{};
    static double nextTime{};

    if (ImGui::GetTime () > nextTime)
    {
        values [offset] = ImGui::GetIO ().Framerate;
        offset = (offset + 1) % IM_ARRAYSIZE (values);
        nextTime = ImGui::GetTime () + 0.1;
    }
    float avg{};

    for (float v : values)
    {
        avg += v;
    }
    avg /= IM_ARRAYSIZE (values);

    char overlay [32];
    snprintf (overlay, sizeof (overlay), "AVG %.1f FPS", avg);

    ImGui::Begin ("Stats");

    ImGui::PlotLines (
        "FPS",
        values,
        IM_ARRAYSIZE (values),
        offset,
        overlay,
        0.0f,
        120.0f,
        ImVec2 (0, 80));

    ImGui::End ();
}

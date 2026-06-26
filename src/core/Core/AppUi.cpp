#include "AppUi.h"
#include "SdlCameraTexture.h"

#include "imgui.h"

#include <string_view>

using namespace App;
using namespace Sdl;
using namespace std;
using namespace Ui;

///////////////////////////////////////////////////////////////////////////////
// StateCounter

/** Null state. */
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

IUiState* StateLogin::update (DataModel& model, const SdlCameraTexture* camera)
{
    IUiState* state = nullptr;

    constexpr string_view popupTitle = "Přihlášení do systému";

    ImVec2 center = ImGui::GetMainViewport ()->GetCenter ();
    ImGui::SetNextWindowPos (center, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));

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
                state = new StateCamera;
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

IUiState* StateCamera::update (DataModel& model, const SdlCameraTexture* camera)
{
    IUiState* state = nullptr;

    if (camera && camera->isValid ())
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport ();

        float width = viewport->Size.x * (2.0f / 3.0f);
        float height = viewport->Size.y * (2.0f / 3.0f);

        ImGui::SetNextWindowPos (ImVec2 ((viewport->Size.x - width) / 2.0f, (viewport->Size.y - height) / 2.0f),
                                 ImGuiCond_Once);
        ImGui::SetNextWindowSize (ImVec2 (width, height), ImGuiCond_Once);

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
// AppUi

AppUi::AppUi ()
{
    m_states.emplace_back (new StateCounter);
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

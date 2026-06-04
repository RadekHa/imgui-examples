#pragma once
#include "IPathService.h"

/** Forward class declaration of SDL window. */
typedef struct SDL_Window SDL_Window;
/** Forward class declaration of SDL renderer. */
struct SDL_Renderer;
/** Forward class declaration of ImGuiIO structure. */
struct ImGuiIO;

namespace App
{
    /** Forward declaration of FrameContext struct. */
    struct FrameContext;
    /** Forward declaration of ImGuiConfig struct. */
    struct ImGuiConfig;

    /** Manages the ImGui rendering pass, including initialization, frame management, and resource cleanup. */
    class ImGuiPass
    {
    public:
        /** Initialize ImGui context and set up SDL renderer bindings. */
        ImGuiPass (SDL_Window* window, SDL_Renderer* renderer, const IPathService* paths);
        /** Clean up ImGui resources and shut down SDL bindings. */
        ~ImGuiPass ();

        /** The copy constructor is deleted to prevent copying of the ImGuiPass instance. */
        ImGuiPass (const ImGuiPass&) = delete;
        /** The copy assignment operator is deleted to prevent copying of the ImGuiPass instance. */
        ImGuiPass& operator= (const ImGuiPass&) = delete;
        /** The move constructor is deleted to prevent moving of the ImGuiPass instance. */
        ImGuiPass (ImGuiPass&&) = delete;
        /** The move assignment operator is deleted to prevent moving of the ImGuiPass instance. */
        ImGuiPass& operator= (ImGuiPass&&) = delete;

        /** Start a new ImGui frame. */
        void beginFrame ();
        /** End the current ImGui frame and render the draw data. */
        void endFrame ();
        /** Fill the provided FrameContext with ImGui-related information for the current frame. */
        void fillFrameContext (FrameContext& ctx) const;
        /** Rebuild ImGui fonts based on the provided scale factor, typically called when DPI scaling changes. */
        void rebuildFonts (float scale);
    private:
        /** Apply paths to ImGui configuration, such as loading fonts. */
        void applyPaths (float scale);

        /** Pointer to the SDL renderer used for rendering ImGui draw data. */
        SDL_Renderer* m_renderer;
        /** Pointer to the IPathService for accessing resource paths. */
        const IPathService* m_paths;
    };
}

#pragma once
#include "IPathService.h"
#include "SdlWrapper.h"

#include <filesystem>

namespace App
{
    /** Implementation of IPathService using SDL functions to determine paths. */
    class SDLPathService : public IPathService
    {
    public:
        /** Initialize the base path from SDL. */
        SDLPathService ();

        /** {@inheritDoc} */
        virtual const char* getUserConfigFilePath () const override;
        /** {@inheritDoc} */
        virtual std::string getFontPath (std::string_view fontFileName) const override;
        /** {@inheritDoc} */
        virtual std::string getResourcePath (std::string_view filePath) const override;
        /** Get the base resource path. */
        std::filesystem::path getBaseResourcePath () const;
    private:
        /** Helper function to convert a filesystem path to a UTF-8 encoded string. */
        std ::string toString (const std::filesystem::path& filePath) const;

        /** Base path obtained from SDL to construct resource paths. */
        sdl::SdlTextPtr m_basePath;
        /** Cached user configuration path to avoid redundant SDL calls. */
        std::string m_userConfigPath;
    };
}

#pragma once
#include "IPathService.h"

namespace App
{
    /** Implementation of IPathService using SDL functions to determine paths. */
    class SDLPathService : public IPathService
    {
    public:
        /** Initialize the base path from SDL. */
        SDLPathService ();

        /** {@inheritDoc} */
        virtual std::filesystem::path getUserConfigPath () const override;
        /** {@inheritDoc} */
        virtual std::filesystem::path getFontPath (std::string_view fontFileName) const override;
        /** {@inheritDoc} */
        virtual std::filesystem::path getResourcePath (std::string_view filePath) const override;
    private:
        /** Base path obtained from SDL to construct resource paths. */
        std::filesystem::path m_basePath;
    };
}

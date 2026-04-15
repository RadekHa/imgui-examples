#pragma once
#include <filesystem>
#include <memory>

namespace App
{
    /** Interface for path services, providing methods to retrieve various application paths. */
    class IPathService
    {
    public:
        /** Virtual destructor to allow proper cleanup of derived classes. */
        virtual ~IPathService () = default;

        /** Get the path to the user configuration directory. */
        virtual std::filesystem::path getUserConfigPath () const = 0;
        /** Get the path to a font file within the application's resources. */
        virtual std::filesystem::path getFontPath (std::string_view fontFileName) const = 0;
        /** Get the path to the application's resources. */
        virtual std::filesystem::path getResourcePath (std::string_view filePath) const = 0;
    };

    /** Factory function to create an instance of IPathService. */
    std::unique_ptr<IPathService> createSdlPathService ();
}

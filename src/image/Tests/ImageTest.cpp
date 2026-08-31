#include "Image/IImageData.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>

using namespace Image;
using namespace std;

TEST_CASE ("Image::loadImage")
{
    const filesystem::path testImagePath = filesystem::current_path () / ".." / ".." / "share" / "images" / "image.png";
    INFO (testImagePath);
    REQUIRE (filesystem::exists (testImagePath));

    SECTION ("loads image with default channels")
    {
        auto image = loadImage (testImagePath.string ());
        REQUIRE (image != nullptr);
        REQUIRE (image->getWidth () > 0);
        REQUIRE (image->getHeight () > 0);
        REQUIRE (image->getChannels () > 0);
        REQUIRE (image->getPixels () != nullptr);
    }

    SECTION ("loads image with desired channels (RGB)")
    {
        auto image = loadImage (testImagePath.string (), 3);
        REQUIRE (image != nullptr);
        REQUIRE (image->getWidth () > 0);
        REQUIRE (image->getHeight () > 0);
        REQUIRE (image->getChannels () == 3);
        REQUIRE (image->getPixels () != nullptr);
    }

    SECTION ("loads image with desired channels (RGBA)")
    {
        auto image = loadImage (testImagePath.string (), 4);
        REQUIRE (image != nullptr);
        REQUIRE (image->getWidth () > 0);
        REQUIRE (image->getHeight () > 0);
        REQUIRE (image->getChannels () == 4);
        REQUIRE (image->getPixels () != nullptr);
    }

    SECTION ("returns null for non-existent file")
    {
        auto image = loadImage ("non_existent_file.png");
        REQUIRE (image == nullptr);
    }
}

TEST_CASE ("Image::ImageData")
{
    const filesystem::path testImagePath = filesystem::current_path () / ".." / ".." / "share" / "images" / "image.png";

    auto image = loadImage (testImagePath.string ());
    REQUIRE (image != nullptr);

    const unsigned char* pixels = image->getPixels ();
    int width = image->getWidth ();
    int height = image->getHeight ();
    int channels = image->getChannels ();

    REQUIRE (pixels != nullptr);
    REQUIRE (width > 0);
    REQUIRE (height > 0);
    REQUIRE (channels > 0);

    SECTION ("pixel data is accessible")
    {
        size_t pixelCount = static_cast<size_t> (width) * height * channels;
        REQUIRE (pixelCount > 0);

        for (size_t i = 0; i < min (pixelCount, size_t (100)); ++i)
        {
            REQUIRE (pixels [i] <= 255);
        }
    }
}

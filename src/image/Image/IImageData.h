#pragma once
#include <memory>
#include <string_view>

namespace Image
{
    /** An interface of a image data class. */
    struct IImageData
    {
        /** Virtual interface destructor. */
        virtual ~IImageData () = default;

        /** Pointer to image data. */
        virtual const unsigned char* getPixels () const = 0;
        /** Get width of the image. */
        virtual int getWidth ()    const = 0;
        /** Get height of the image. */
        virtual int getHeight ()   const = 0;
        /** Get number of channels in the image. */
        virtual int getChannels () const = 0;
    };

    /** Factory function to create an instance of IImageData. */
    std::unique_ptr<IImageData> loadImage (std::string_view filePath, int desiredChannels = 0);
}

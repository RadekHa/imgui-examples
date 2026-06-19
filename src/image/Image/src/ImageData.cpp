#include "ImageData.h"
#include "TraceLog/Log.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace Image;

///////////////////////////////////////////////////////////////////////////////
// Factory function.

unique_ptr<IImageData> Image::loadImage (string_view filePath, int desiredChannels)
{
    int w{}, h{}, ch{};
    unsigned char* raw = stbi_load (filePath.data (), &w, &h, &ch, desiredChannels);

    if (!raw)
    {
        APP_ERROR ("Image failed to load '{}': {}", filePath, stbi_failure_reason ());
        return nullptr;
    }
    const int actualChannels = (desiredChannels != 0) ? desiredChannels : ch;

    return make_unique<ImageData> (raw, w, h, actualChannels);
}

///////////////////////////////////////////////////////////////////////////////
// ImageData

ImageData::ImageData (unsigned char* pixels, int w, int h, int ch)
    : m_pixels {pixels}
    , m_width {w}
    , m_height {h}
    , m_channels {ch}
{
}

ImageData::~ImageData ()
{
    stbi_image_free (m_pixels);
}

const unsigned char* ImageData::getPixels () const
{
    return m_pixels;
}

int ImageData::getWidth () const
{
    return m_width;
}

int ImageData::getHeight () const
{
    return m_height;
}

int ImageData::getChannels () const
{
    return m_channels;
}

#include "Image/IImageData.h"

namespace Image
{
    /** Implementation of IImageData interface. */
    class ImageData : public IImageData
    {
    public:
        /** Initialize member variables. */
        ImageData (unsigned char* pixels, int w, int h, int ch);
        /** Free image data. */
        ~ImageData () override;

        /** {@inheritDoc} */
        const unsigned char* getPixels ()  const override;
        /** {@inheritDoc} */
        int getWidth () const override;
        /** {@inheritDoc} */
        int getHeight () const override;
        /** {@inheritDoc} */
        int getChannels () const override;

    private:
        /** Image data. */
        unsigned char* m_pixels;
        /** Width of the image. */
        int m_width;
        /** Height of the image. */
        int m_height;
        /** Number of channels. */
        int m_channels;
    };
}

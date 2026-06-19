#include "Image/IImageData.h"

namespace Image
{
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
        unsigned char* m_pixels;
        int m_width;
        int m_height;
        int m_channels;
    };
}

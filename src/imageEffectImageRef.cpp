#include "../include/imageEffectImageRef.h"

#include "imageIo.h"
#include "openfx/ofxImageEffect.h"

ImageEffectImageRef::ImageEffectImageRef(const image::Image& image)
{
    int rowBytes = image.width() * sizeof(image::Pixel);
    m_Properties.SetN(kOfxImagePropRowBytes, { rowBytes });
    m_Properties.SetN(kOfxImagePropBounds, { 0, 0, (int)image.width(), (int)image.height() });
    m_Properties.SetPointer(kOfxImagePropData, 0, image.data());
    m_Properties.SetString(kOfxImagePropField, 0, kOfxImageFieldBoth);
    m_Properties.SetDouble(kOfxImagePropPixelAspectRatio, 0, 1.0);

    // Images we load are always 32-bit RGBA.
    m_Properties.SetString(kOfxImageEffectPropPixelDepth, 0, kOfxBitDepthByte);
    m_Properties.SetString(kOfxImageEffectPropComponents, 0, kOfxImageComponentRGBA);
}

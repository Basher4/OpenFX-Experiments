#include "../include/imageEffectImageRef.h"

#include "imageIo.h"
#include "openfx/ofxImageEffect.h"

ImageEffectImageRef::ImageEffectImageRef(const image::Image& image)
{
    int rowBytes = image.width() * sizeof(image::Pixel);
    m_Properties.SetN(kOfxImagePropRowBytes, { rowBytes });
    m_Properties.SetN(kOfxImagePropBounds, { 0, 0, (int)image.width(), (int)image.height() });
    m_Properties.SetPointer(kOfxImagePropData, 0, image.data());
}
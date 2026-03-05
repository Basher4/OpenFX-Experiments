#pragma once

#include "imageEffectImageRef.h"
#include "openfx/ofxImageEffect.h"
#include "propertySet.h"

class ImageEffectClip {
public:
    ImageEffectClip(std::string_view name) : m_Name{name} {  }

    void SetImageRef(ImageEffectImageRef *image) { m_Image = image; }
    ImageEffectImageRef *ImageRef() { return m_Image; }

    PropertySet& Properties() { return m_Properties; }
    OfxImageClipHandle OfxHandle() { return reinterpret_cast<OfxImageClipHandle>(this); }

    void DebugPrint();

private:
    std::string m_Name;
    PropertySet m_Properties {"imageEffectClip"};
    ImageEffectImageRef *m_Image = nullptr;
};

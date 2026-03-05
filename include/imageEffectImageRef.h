#pragma once

#include "propertySet.h"

namespace image {
class Image;
}

// Better make sure that the image we're referencing lives at least as long as this ref.
class ImageEffectImageRef {
public:
    explicit ImageEffectImageRef(const image::Image& image);

    PropertySet& Properties() { return m_Properties; }

private:
    PropertySet m_Properties {"imageEffectImageRef"};
};

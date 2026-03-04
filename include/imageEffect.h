#pragma once

#include "openfx/ofxImageEffect.h"
#include "propertySet.h"
#include "types.h"

#include <memory>
#include <unordered_map>

class ImageEffect {
public:
    OfxResult<OfxPropertySetHandle> ClipDefine(std::string_view name);

    void DebugPrint();

    OfxImageEffectHandle OfxHandle() { return reinterpret_cast<OfxImageEffectHandle>(this); }
    OfxPropertySetHandle PropertySetOfxHandle() { return m_Properties.OfxHandle(); }

private:
    PropertySet m_Properties {"ImageEffect"};
    std::unordered_map<std::string, std::unique_ptr<PropertySet>> m_ClipProperties;
};

#pragma once

#include "imageEffectClip.h"
#include "openfx/ofxImageEffect.h"
#include "parameterSet.h"
#include "propertySet.h"
#include "types.h"

#include <memory>
#include <unordered_map>

class ImageEffect {
public:
    OfxResult<OfxPropertySetHandle> ClipDefine(std::string_view name);
    OfxResult<ImageEffectClip*> GetClip(std::string_view name);

    void DebugPrint();

    OfxImageEffectHandle OfxHandle() { return reinterpret_cast<OfxImageEffectHandle>(this); }

    PropertySet& Properties() { return m_Properties; }
    OfxPropertySetHandle PropertySetOfxHandle() { return m_Properties.OfxHandle(); }

    ParameterSet& Parameters() { return m_ParamSet; }
    OfxParamSetHandle ParamSetOfxHandle() { return m_ParamSet.OfxHandle(); }

private:
    PropertySet m_Properties {"ImageEffect"};
    std::unordered_map<std::string, std::unique_ptr<ImageEffectClip>> m_Clips;

    ParameterSet m_ParamSet;
};

#include "imageEffect.h"

#include <print>

OfxResult<OfxPropertySetHandle> ImageEffect::ClipDefine(std::string_view name)
{
    std::string name_owned{name};
    if (m_ClipProperties.contains(name_owned)) {
        return std::unexpected{kOfxStatErrExists};
    }

    m_ClipProperties[name_owned] = std::make_unique<PropertySet>(name_owned);
    return m_ClipProperties[name_owned]->OfxHandle();
}

void ImageEffect::DebugPrint()
{
    std::print("Effect properties\n");
    m_Properties.DebugPrint();
    std::print("--------------------\n");

    for (const auto& [name, clipProps] : m_ClipProperties) {
        std::print("Clip '{}'\n", name);
        clipProps->DebugPrint();
        std::print("--------------------\n");
    }

    std::print("====================\n\n");
}
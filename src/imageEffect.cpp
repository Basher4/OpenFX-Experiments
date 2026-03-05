#include "imageEffect.h"

#include <print>
#include <ranges>

OfxResult<OfxPropertySetHandle> ImageEffect::ClipDefine(std::string_view name)
{
    std::string name_owned{name};
    if (m_Clips.contains(name_owned)) {
        return std::unexpected{kOfxStatErrExists};
    }

    m_Clips[name_owned] = std::make_unique<ImageEffectClip>(name);
    return m_Clips[name_owned]->Properties().OfxHandle();
}

OfxResult<ImageEffectClip*> ImageEffect::GetClip(std::string_view name)
{
    std::string name_owned{name};
    auto it = m_Clips.find(name_owned);
    if (it == m_Clips.end()) { return std::unexpected(kOfxStatErrUnknown); }
    return it->second.get();
}

void ImageEffect::DebugPrint()
{
    std::print("Effect properties\n");
    m_Properties.DebugPrint();
    std::print("--------------------\n");

    for (const auto& clipProps : m_Clips | std::views::values) {
        clipProps->DebugPrint();
        std::print("--------------------\n");
    }

    std::print("====================\n\n");
}
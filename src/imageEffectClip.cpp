#include "../include/imageEffectClip.h"

#include <print>

void ImageEffectClip::DebugPrint()
{
    std::print("Clip '{}'\n", m_Name);
    m_Properties.DebugPrint();
}
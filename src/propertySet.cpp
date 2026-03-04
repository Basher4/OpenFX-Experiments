#include "propertySet.h"

#include <print>

OfxStatus PropertySet::GetPointer(std::string property, int index, void** value)
{
    return Get(m_PropsPtr, property, index, value);
}

OfxStatus PropertySet::SetPointer(std::string property, int index, void* value)
{
    return Set(m_PropsPtr, property, index, value);
}

OfxStatus PropertySet::GetInt(std::string property, int index, int* value)
{
    return Get(m_PropsInt, property, index, value);
}

OfxStatus PropertySet::SetInt(std::string property, int index, int value)
{
    return Set(m_PropsInt, property, index, value);
}

OfxStatus PropertySet::GetString(std::string property, int index, char** value)
{
    const auto key = std::make_tuple(property, index);
    const auto it = m_PropsString.find(key);
    if (it == m_PropsString.end()) {
        return kOfxStatErrUnknown;
    }

    *value = const_cast<char*>(it->second.c_str());
    return kOfxStatOK;
}

OfxStatus PropertySet::SetString(std::string property, int index, const char* value)
{
    return Set(m_PropsString, property, index, value);
}

OfxStatus PropertySet::GetDouble(std::string property, int index, double* value)
{
    return Get(m_PropsDouble, property, index, value);
}

OfxStatus PropertySet::SetDouble(std::string property, int index, double value)
{
    return Set(m_PropsDouble, property, index, value);
}

void PropertySet::DebugPrint()
{
    std::print("PropertySet[{}]\n", _Id);

    std::print("  Pointer properties ({})\n", m_PropsPtr.size());
    for (const auto& [key, value] : m_PropsPtr) {
        const auto& [property, index] = key;
        std::print("    {}[{}] = {}\n", property, index, value);
    }

    std::print("  Int properties ({})\n", m_PropsInt.size());
    for (const auto& [key, value] : m_PropsInt) {
        const auto& [property, index] = key;
        std::print("    {}[{}] = {}\n", property, index, value);
    }

    std::print("  String properties ({})\n", m_PropsString.size());
    for (const auto& [key, value] : m_PropsString) {
        const auto& [property, index] = key;
        std::print("    {}[{}] = \"{}\"\n", property, index, value);
    }
}

void PropertySet::Clear()
{
    m_PropsPtr.clear();
    m_PropsInt.clear();
    m_PropsString.clear();
    m_PropsDouble.clear();
}

OfxPropertySuiteV1* PropertySet::as_suite()
{
    static OfxPropertySuiteV1 sPropSuite {
        .propSetPointer = &SuiteThunk<&PropertySet::SetPointer, void*>,
        .propSetString = &SuiteThunk<&PropertySet::SetString, const char*>,
        .propSetDouble = &SuiteThunk<&PropertySet::SetDouble, double>,
        .propSetInt = &SuiteThunk<&PropertySet::SetInt, int32_t>,
        .propGetPointer = &SuiteThunk<&PropertySet::GetPointer, void**>,
        .propGetString = &SuiteThunk<&PropertySet::GetString, char**>,
        .propGetDouble = &SuiteThunk<&PropertySet::GetDouble, double*>,
        .propGetInt = &SuiteThunk<&PropertySet::GetInt, int*>,
    };

    return &sPropSuite;
}

#include "propertySet.h"

#include <print>

OfxStatus PropertySet::GetPointer(std::string_view property, int index, void** value)
{
    return Get(m_PropsPtr, property, index, value);
}

OfxStatus PropertySet::SetPointer(std::string_view property, int index, void* value)
{
    return Set(m_PropsPtr, property, index, value);
}

OfxStatus PropertySet::GetInt(std::string_view property, int index, int* value)
{
    return Get(m_PropsInt, property, index, value);
}

OfxStatus PropertySet::SetInt(std::string_view property, int index, int value)
{
    return Set(m_PropsInt, property, index, value);
}

OfxStatus PropertySet::GetString(std::string_view property, int index, char** value)
{
    const auto key = std::make_tuple(std::string{property}, index);
    const auto it = m_PropsString.find(key);
    if (it == m_PropsString.end()) {
        return kOfxStatErrUnknown;
    }

    *value = const_cast<char*>(it->second.c_str());
    return kOfxStatOK;
}

OfxStatus PropertySet::SetString(std::string_view property, int index, const char* value)
{
    return Set(m_PropsString, property, index, value);
}

OfxStatus PropertySet::GetDouble(std::string_view property, int index, double* value)
{
    return Get(m_PropsDouble, property, index, value);
}

OfxStatus PropertySet::SetDouble(std::string_view property, int index, double value)
{
    return Set(m_PropsDouble, property, index, value);
}

OfxStatus PropertySet::GetPointerN(std::string_view property, int count, void** value)
{
    return GetN(m_PropsPtr, property, count, value);
}

OfxStatus PropertySet::SetPointerN(std::string_view property, int count, void* const* value)
{
    return SetN(m_PropsPtr, property, count, value);
}

OfxStatus PropertySet::GetIntN(std::string_view property, int count, int* value)
{
    return GetN(m_PropsInt, property, count, value);
}

OfxStatus PropertySet::SetIntN(std::string_view property, int count, const int* value)
{
    return SetN(m_PropsInt, property, count, value);
}

OfxStatus PropertySet::GetStringN(std::string_view property, int count, char** value)
{
    OfxStatus status = kOfxStatOK;
    for (int index{}; index < count; index++) {
        status |= GetString(property, index, &value[index]);
    }
    return status;
}

OfxStatus PropertySet::SetStringN(std::string_view property, int count, const char* const* value)
{
    return SetN(m_PropsString, property, count, value);
}

OfxStatus PropertySet::GetDoubleN(std::string_view property, int count, double* value)
{
    return GetN(m_PropsDouble, property, count, value);
}

OfxStatus PropertySet::SetDoubleN(std::string_view property, int count, const double* value)
{
    return SetN(m_PropsDouble, property, count, value);
}

OfxStatus PropertySet::SetN(std::string_view property, std::initializer_list<int> values)
{
    return SetIntN(property, static_cast<int>(values.size()), values.begin());
}
OfxStatus PropertySet::SetN(std::string_view property, std::initializer_list<double> values)
{
    return SetDoubleN(property, static_cast<int>(values.size()), values.begin());
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
        .propSetPointerN = &SuiteThunk<&PropertySet::SetPointerN, void* const*>,
        .propSetStringN = &SuiteThunk<&PropertySet::SetStringN, const char* const*>,
        .propSetDoubleN = &SuiteThunk<&PropertySet::SetDoubleN, const double*>,
        .propSetIntN = &SuiteThunk<&PropertySet::SetIntN, const int*>,
        .propGetPointer = &SuiteThunk<&PropertySet::GetPointer, void**>,
        .propGetString = &SuiteThunk<&PropertySet::GetString, char**>,
        .propGetDouble = &SuiteThunk<&PropertySet::GetDouble, double*>,
        .propGetInt = &SuiteThunk<&PropertySet::GetInt, int*>,
        .propGetPointerN = &SuiteThunk<&PropertySet::GetPointerN, void**>,
        .propGetStringN = &SuiteThunk<&PropertySet::GetStringN, char**>,
        .propGetDoubleN = &SuiteThunk<&PropertySet::GetDoubleN, double*>,
        .propGetIntN = &SuiteThunk<&PropertySet::GetIntN, int*>,
    };

    return &sPropSuite;
}

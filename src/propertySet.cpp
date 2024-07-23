//
// Created by user on 7/23/2024.
//

#include "propertySet.h"

OfxStatus PropertySet::GetPointer(std::string property, int index, void** value)
{
    const auto key = std::make_tuple(property, index);
    const auto it = m_PropsPtr.find(key);
    if (it == m_PropsPtr.end()) {
        return kOfxStatErrUnknown;
    }

    *value = it->second;
    return kOfxStatOK;
}

OfxStatus PropertySet::SetPointer(std::string property, int index, void* value)
{
    const auto key = std::make_tuple(property, index);
    m_PropsPtr[key] = value;
    return kOfxStatOK;
}

OfxStatus PropertySet::GetInt(std::string property, int index, int* value)
{
    const auto key = std::make_tuple(property, index);
    const auto it = m_PropsInt.find(key);
    if (it == m_PropsInt.end()) {
        return kOfxStatErrUnknown;
    }

    *value = it->second;
    return kOfxStatOK;
}

OfxStatus PropertySet::SetInt(std::string property, int index, int value)
{
    const auto key = std::make_tuple(property, index);
    m_PropsInt[key] = value;
    return kOfxStatOK;
}

OfxStatus PropertySet::GetString(std::string property, int index, char** value)
{
    const auto key = std::make_tuple(property, index);
    const auto it = m_PropsString.find(key);
    if (it == m_PropsString.end()) {
        return kOfxStatErrUnknown;
    }

    // Not sure why the API says this is a modifiable string. How does that make any sense?
    *value = const_cast<char*>(it->second.c_str());
    return kOfxStatOK;
}

OfxStatus PropertySet::SetString(std::string property, int index, const char* value)
{
    const auto key = std::make_tuple(property, index);
    m_PropsString[key] = value;
    return kOfxStatOK;
}

OfxPropertySuiteV1* PropertySet::as_suite()
{
    static OfxPropertySuiteV1 sPropSuite = {};
    sPropSuite.propGetPointer = [](OfxPropertySetHandle properties, const char *property, int index, void** value) {
        return reinterpret_cast<PropertySet*>(properties)->GetPointer(property, index, value);
    };
    sPropSuite.propSetPointer = [](OfxPropertySetHandle properties, const char *property, int index, void* value) {
        return reinterpret_cast<PropertySet*>(properties)->SetPointer(property, index, value);
    };
    sPropSuite.propGetInt = [](OfxPropertySetHandle properties, const char *property, int index, int* value) {
        return reinterpret_cast<PropertySet*>(properties)->GetInt(property, index, value);
    };
    sPropSuite.propSetInt = [](OfxPropertySetHandle properties, const char *property, int index, int value) {
        return reinterpret_cast<PropertySet*>(properties)->SetInt(property, index, value);
    };
    sPropSuite.propGetString = [](OfxPropertySetHandle properties, const char *property, int index, char** value) {
        return reinterpret_cast<PropertySet*>(properties)->GetString(property, index, value);
    };
    sPropSuite.propSetString = [](OfxPropertySetHandle properties, const char *property, int index, const char* value) {
        return reinterpret_cast<PropertySet*>(properties)->SetString(property, index, value);
    };

    return &sPropSuite;
}
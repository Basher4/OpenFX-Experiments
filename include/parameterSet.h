#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <openfx/ofxParam.h>

#include <propertySet.h>
#include <unordered_map>

enum class ParameterType {
    Integer,
    Double,
    Boolean,
    Choice,
    RGBA,
    RGB,
    Double2D,
    Integer2D,
    Double3D,
    Integer3D,
    String,
    Custom,
    Group,
    Page,
    PushButton,
};

constexpr std::string_view ToOfxParamTypeString(ParameterType type)
{
    switch (type) {
    case ParameterType::Integer:
        return kOfxParamTypeInteger;
    case ParameterType::Double:
        return kOfxParamTypeDouble;
    case ParameterType::Boolean:
        return kOfxParamTypeBoolean;
    case ParameterType::Choice:
        return kOfxParamTypeChoice;
    case ParameterType::RGBA:
        return kOfxParamTypeRGBA;
    case ParameterType::RGB:
        return kOfxParamTypeRGB;
    case ParameterType::Double2D:
        return kOfxParamTypeDouble2D;
    case ParameterType::Integer2D:
        return kOfxParamTypeInteger2D;
    case ParameterType::Double3D:
        return kOfxParamTypeDouble3D;
    case ParameterType::Integer3D:
        return kOfxParamTypeInteger3D;
    case ParameterType::String:
        return kOfxParamTypeString;
    case ParameterType::Custom:
        return kOfxParamTypeCustom;
    case ParameterType::Group:
        return kOfxParamTypeGroup;
    case ParameterType::Page:
        return kOfxParamTypePage;
    case ParameterType::PushButton:
        return kOfxParamTypePushButton;
    }

    return {};
}

constexpr std::optional<ParameterType> ParameterTypeFromString(const std::string_view type)
{
    if (type == kOfxParamTypeInteger) return ParameterType::Integer;
    if (type == kOfxParamTypeDouble) return ParameterType::Double;
    if (type == kOfxParamTypeBoolean) return ParameterType::Boolean;
    if (type == kOfxParamTypeChoice) return ParameterType::Choice;
    if (type == kOfxParamTypeRGBA) return ParameterType::RGBA;
    if (type == kOfxParamTypeRGB) return ParameterType::RGB;
    if (type == kOfxParamTypeDouble2D) return ParameterType::Double2D;
    if (type == kOfxParamTypeInteger2D) return ParameterType::Integer2D;
    if (type == kOfxParamTypeDouble3D) return ParameterType::Double3D;
    if (type == kOfxParamTypeInteger3D) return ParameterType::Integer3D;
    if (type == kOfxParamTypeString) return ParameterType::String;
    if (type == kOfxParamTypeCustom) return ParameterType::Custom;
    if (type == kOfxParamTypeGroup) return ParameterType::Group;
    if (type == kOfxParamTypePage) return ParameterType::Page;
    if (type == kOfxParamTypePushButton) return ParameterType::PushButton;

    return std::nullopt;
}

class Parameter {
public:
    Parameter(const std::string_view name, ParameterType type)
        : m_Name(name), m_Type(type), m_Properties(std::string { name })
    {
    }

    OfxParamHandle OfxHandle() { return reinterpret_cast<OfxParamHandle>(this); }
    OfxPropertySetHandle PropertySetOfxHandle() { return m_Properties.OfxHandle(); }

    void DebugPrint();

private:
    std::string m_Name;
    ParameterType m_Type;
    PropertySet m_Properties {"Parameter"};
};

class ParameterSet {
public:
    OfxStatus Define(std::string_view name, ParameterType type, OfxPropertySetHandle *propertySet);
    std::optional<Parameter*> GetParameter(std::string_view name);

    OfxParamSetHandle OfxHandle() { return reinterpret_cast<OfxParamSetHandle>(this); }
    static OfxParameterSuiteV1* get_suite();

    void DebugPrint();

private:
    std::unordered_map<std::string, std::unique_ptr<Parameter>> m_Parameters;

    PropertySet m_Properties {"ParameterSet"};
};

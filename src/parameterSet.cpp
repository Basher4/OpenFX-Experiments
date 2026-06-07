#include "parameterSet.h"
#include "propertySet.h"

#include <algorithm>
#include <cstdarg>
#include <print>
#include <ranges>

namespace {
OfxStatus ParamGetValueV(OfxParamHandle handle, va_list args)
{
    const auto param = reinterpret_cast<ParameterBase*>(handle);
    param->GetToVariadic(args);

    return kOfxStatOK;
}

OfxStatus ParamSetValueV(OfxParamHandle handle, va_list args)
{
    const auto param = reinterpret_cast<ParameterBase*>(handle);
    param->SetFromVariadic(args);

    return kOfxStatOK;
}

OfxStatus ParamGetValue(OfxParamHandle handle, ...)
{
    va_list args;
    va_start(args, handle);

    const OfxStatus status = ParamGetValueV(handle, args);

    va_end(args);

    return status;
}

// No keyframe support, always time 0;
OfxStatus ParamGetValueAtTime(OfxParamHandle handle, OfxTime time, ...)
{
    va_list args;
    va_start(args, time);

    const OfxStatus status = ParamGetValueV(handle, args);

    va_end(args);

    return status;
}

OfxStatus UnsupportedParamGetDerivative(OfxParamHandle, OfxTime, ...)
{
    return kOfxStatErrUnsupported;
}

OfxStatus UnsupportedParamGetIntegral(OfxParamHandle, OfxTime, OfxTime, ...)
{
    return kOfxStatErrUnsupported;
}

OfxStatus ParamSetValue(OfxParamHandle handle, ...)
{
    va_list args;
    va_start(args, handle);

    const OfxStatus status = ParamSetValueV(handle, args);

    va_end(args);

    return status;
}

OfxStatus ParamSetValueAtTime(OfxParamHandle handle, OfxTime time, ...)
{
    va_list args;
    va_start(args, time);

    const OfxStatus status = ParamSetValueV(handle, args);

    va_end(args);

    return status;
}
}

void ParameterBase::DebugPrint()
{
    std::print("Parameter '{}' ({})\n", m_Name, m_Type.ToOfxName());
    m_Properties.DebugPrint();
    std::print("\n");
}

std::optional<ParameterBase*> ParameterSet::GetParameter(std::string_view name)
{
    const auto it = m_Parameters.find(std::string{name});
    if (it == m_Parameters.end()) {
        return {};
    }

    return it->second.get();
}

void ParameterSet::DebugPrint()
{
    std::print("ParameterSet ({} parameters)\n", m_Parameters.size());

    for (const auto& param : m_Parameters | std::views::values) {
        param->DebugPrint();
    }
}

OfxParameterSuiteV1* ParameterSet::as_suite()
{
    static OfxParameterSuiteV1 sParamSuite {
        .paramDefine = [](OfxParamSetHandle handle, const char* paramType, const char* paramName, OfxPropertySetHandle* propertySet) {
            const auto paramSet = reinterpret_cast<ParameterSet*>(handle);
            const auto type = ParameterType::FromOfxName(paramType);
            if (!type)
            {
                return kOfxStatErrUnknown;
            }

            switch (*type)
            {
            case ParameterType::Integer:
                return paramSet->Define<ParameterType::Integer>(paramName, propertySet);
            case ParameterType::Double:
                return paramSet->Define<ParameterType::Double>(paramName, propertySet);
            case ParameterType::Boolean:
                return paramSet->Define<ParameterType::Boolean>(paramName, propertySet);
            case ParameterType::Choice:
                return paramSet->Define<ParameterType::Choice>(paramName, propertySet);
            case ParameterType::RGBA:
                return paramSet->Define<ParameterType::RGBA>(paramName, propertySet);
            case ParameterType::RGB:
                return paramSet->Define<ParameterType::RGB>(paramName, propertySet);
            case ParameterType::Double2D:
                return paramSet->Define<ParameterType::Double2D>(paramName, propertySet);
            case ParameterType::Integer2D:
                return paramSet->Define<ParameterType::Integer2D>(paramName, propertySet);
            case ParameterType::Double3D:
                return paramSet->Define<ParameterType::Double3D>(paramName, propertySet);
            case ParameterType::Integer3D:
                return paramSet->Define<ParameterType::Integer3D>(paramName, propertySet);
            case ParameterType::String:
                return paramSet->Define<ParameterType::String>(paramName, propertySet);
            case ParameterType::Custom:
                return paramSet->Define<ParameterType::Custom>(paramName, propertySet);
            case ParameterType::Group:
                return paramSet->Define<ParameterType::Group>(paramName, propertySet);
            case ParameterType::Page:
                return paramSet->Define<ParameterType::Page>(paramName, propertySet);
            case ParameterType::PushButton:
                return paramSet->Define<ParameterType::PushButton>(paramName, propertySet);
            default:
                return kOfxStatErrUnknown;
            }
        },
        .paramGetHandle = [](OfxParamSetHandle handle, const char* name, OfxParamHandle* out_paramHandle, OfxPropertySetHandle* out_propSetHandle) {
            auto paramSet = reinterpret_cast<ParameterSet*>(handle);
            auto param = paramSet->GetParameter(name);
            if (!param.has_value()) {
                return kOfxStatErrBadHandle;
            }

            if (out_paramHandle) {
                *out_paramHandle = (*param)->OfxHandle();
            }
            if (out_propSetHandle) {
                *out_propSetHandle = (*param)->PropertySetOfxHandle();
            }
            return kOfxStatOK;
        },
        .paramSetGetPropertySet = [](OfxParamSetHandle, OfxPropertySetHandle*) { return kOfxStatErrUnsupported; },
        .paramGetPropertySet = [](OfxParamHandle, OfxPropertySetHandle*) { return kOfxStatErrUnsupported; },
        .paramGetValue = ParamGetValue,
        .paramGetValueAtTime = ParamGetValueAtTime,
        .paramGetDerivative = UnsupportedParamGetDerivative,
        .paramGetIntegral = UnsupportedParamGetIntegral,
        .paramSetValue = ParamSetValue,
        .paramSetValueAtTime = ParamSetValueAtTime,
        .paramGetNumKeys = [](OfxParamHandle, unsigned int*) { return kOfxStatErrUnsupported; },
        .paramGetKeyTime = [](OfxParamHandle, unsigned int, OfxTime*) { return kOfxStatErrUnsupported; },
        .paramGetKeyIndex = [](OfxParamHandle, OfxTime, int, int*) { return kOfxStatErrUnsupported; },
        .paramDeleteKey = [](OfxParamHandle, OfxTime) { return kOfxStatErrUnsupported; },
        .paramDeleteAllKeys = [](OfxParamHandle) { return kOfxStatErrUnsupported; },
        .paramCopy = [](OfxParamHandle, OfxParamHandle, OfxTime, const OfxRangeD*) { return kOfxStatErrUnsupported; },
        .paramEditBegin = [](OfxParamSetHandle, const char*) { return kOfxStatErrUnsupported; },
        .paramEditEnd = [](OfxParamSetHandle) { return kOfxStatErrUnsupported; },
    };

    return &sParamSuite;
}

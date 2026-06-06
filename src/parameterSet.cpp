#include "parameterSet.h"
#include "propertySet.h"

#include <algorithm>
#include <print>
#include <ranges>
#include <vector>

namespace {
OfxStatus UnsupportedParamGetValue(OfxParamHandle, ...)
{
    return kOfxStatErrUnsupported;
}

OfxStatus UnsupportedParamGetValueAtTime(OfxParamHandle, OfxTime, ...)
{
    return kOfxStatErrUnsupported;
}

OfxStatus UnsupportedParamGetDerivative(OfxParamHandle, OfxTime, ...)
{
    return kOfxStatErrUnsupported;
}

OfxStatus UnsupportedParamGetIntegral(OfxParamHandle, OfxTime, OfxTime, ...)
{
    return kOfxStatErrUnsupported;
}

OfxStatus UnsupportedParamSetValue(OfxParamHandle, ...)
{
    return kOfxStatErrUnsupported;
}

OfxStatus UnsupportedParamSetValueAtTime(OfxParamHandle, OfxTime, ...)
{
    return kOfxStatErrUnsupported;
}
}

void Parameter::DebugPrint()
{
    std::print("Parameter '{}' ({})\n", m_Name, ToOfxParamTypeString(m_Type));
    m_Properties.DebugPrint();
    std::print("\n");
}

OfxStatus ParameterSet::Define(std::string_view name, ParameterType type, OfxPropertySetHandle* propertySet)
{
    const std::string name_owned { name };
    if (m_Parameters.contains(name_owned)) {
        return kOfxStatErrExists;
    }

    const auto [it, _] = m_Parameters.emplace(name_owned, std::make_unique<Parameter>(name_owned, type));
    if (propertySet) {
        *propertySet = it->second->PropertySetOfxHandle();
    }

    return kOfxStatOK;
}

std::optional<Parameter*> ParameterSet::GetParameter(std::string_view name)
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

    for (const auto& parameter : m_Parameters) {
        parameter.second->DebugPrint();
    }
}

OfxParameterSuiteV1* ParameterSet::get_suite()
{
    static OfxParameterSuiteV1 sParamSuite {
        .paramDefine = [](OfxParamSetHandle handle, const char* paramType, const char* paramName, OfxPropertySetHandle* propertySet) {
            auto paramSet = reinterpret_cast<ParameterSet*>(handle);
            const auto type = ParameterTypeFromString(paramType);
            if (!type) {
                return kOfxStatErrUnknown;
            }

            return paramSet->Define(paramName, *type, propertySet); },
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
        .paramGetValue = UnsupportedParamGetValue,
        .paramGetValueAtTime = UnsupportedParamGetValueAtTime,
        .paramGetDerivative = UnsupportedParamGetDerivative,
        .paramGetIntegral = UnsupportedParamGetIntegral,
        .paramSetValue = UnsupportedParamSetValue,
        .paramSetValueAtTime = UnsupportedParamSetValueAtTime,
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

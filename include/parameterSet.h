#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include <openfx/ofxParam.h>

#include <propertySet.h>
#include <stdarg.h>
#include <unordered_map>

struct ParameterType
{
    enum Value : std::size_t
    {
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

        // Marker
        Count,
    };

    struct Info
    {
        std::string_view ofx_type;
    };

    constexpr ParameterType(Value value) : m_Value(value) {}
    constexpr operator Value() const { return m_Value; }

    constexpr std::string_view ToOfxName() const
    {
        return std::get<0>(k_Info[m_Value]).ofx_type;
    }

    static constexpr std::optional<ParameterType> FromOfxName(std::string_view type)
    {
        for (const auto& [name, val] : k_Info)
        {
            if (name.ofx_type == type) return val;
        }

        return std::nullopt;
    }

private:
    Value m_Value;

    static constexpr std::array<std::tuple<Info, Value>, Count> k_Info{{
        {{kOfxParamTypeInteger}, Integer},
        {{kOfxParamTypeDouble}, Double},
        {{kOfxParamTypeBoolean}, Boolean},
        {{kOfxParamTypeChoice}, Choice},
        {{kOfxParamTypeRGBA}, RGBA},
        {{kOfxParamTypeRGB}, RGB},
        {{kOfxParamTypeDouble2D}, Double2D},
        {{kOfxParamTypeInteger2D}, Integer2D},
        {{kOfxParamTypeDouble3D}, Double3D},
        {{kOfxParamTypeInteger3D}, Integer3D},
        {{kOfxParamTypeString}, String},
        {{kOfxParamTypeCustom}, Custom},
        {{kOfxParamTypeGroup}, Group},
        {{kOfxParamTypePage}, Page},
        {{kOfxParamTypePushButton}, PushButton},
    }};
};

template <ParameterType::Value Type>
struct ParameterTraits;

#define PARAMETER_TRAIT(TYPE, OFX_TYPE, ...)            \
    template <>                                         \
    struct ParameterTraits<ParameterType::TYPE>         \
    {                                                   \
        using value_type = __VA_ARGS__;                 \
        static constexpr std::string_view ofx_type =    \
            OFX_TYPE;                                   \
    }

PARAMETER_TRAIT(Integer,    kOfxParamTypeInteger,     int32_t);
PARAMETER_TRAIT(Double,     kOfxParamTypeDouble,      double);
PARAMETER_TRAIT(Boolean,    kOfxParamTypeBoolean,     int32_t);
PARAMETER_TRAIT(Choice,     kOfxParamTypeChoice,      int32_t);
PARAMETER_TRAIT(RGBA,       kOfxParamTypeRGBA,        std::array<double, 4>);
PARAMETER_TRAIT(RGB,        kOfxParamTypeRGB,         std::array<double, 3>);
PARAMETER_TRAIT(Double2D,   kOfxParamTypeDouble2D,    std::array<double, 2>);
PARAMETER_TRAIT(Integer2D,  kOfxParamTypeInteger2D,   std::array<int32_t, 2>);
PARAMETER_TRAIT(Double3D,   kOfxParamTypeDouble3D,    std::array<double, 3>);
PARAMETER_TRAIT(Integer3D,  kOfxParamTypeInteger3D,   std::array<int32_t, 3>);
PARAMETER_TRAIT(String,     kOfxParamTypeString,      std::string);
PARAMETER_TRAIT(Custom,     kOfxParamTypeCustom,      std::string);
PARAMETER_TRAIT(Group,      kOfxParamTypeGroup,       std::monostate);
PARAMETER_TRAIT(Page,       kOfxParamTypePage,        std::monostate);
PARAMETER_TRAIT(PushButton, kOfxParamTypePushButton,  std::monostate);

#undef PARAMETER_TRAIT

template <ParameterType::Value Type>
using parameter_value_t = typename ParameterTraits<Type>::value_type;


class ParameterBase
{
public:
    ParameterBase(const std::string_view name, ParameterType type)
        : m_Name(name), m_Type(type), m_Properties(std::string{name})
    {
    }

    virtual ~ParameterBase() = default;

    virtual void GetToVariadic(va_list args) = 0;
    virtual void SetFromVariadic(va_list args) = 0;

    OfxParamHandle OfxHandle() { return reinterpret_cast<OfxParamHandle>(this); }
    OfxPropertySetHandle PropertySetOfxHandle() { return m_Properties.OfxHandle(); }

    void DebugPrint();

private:
    const std::string m_Name;
    const ParameterType m_Type;
    PropertySet m_Properties{"Parameter"};
};

template <typename T>
class Parameter : public ParameterBase
{
public:
    Parameter(const std::string_view name, const ParameterType type) : ParameterBase(name, type)
    {
    }

    T Get() { return m_Value; }
    void Set(T value) { m_Value = value; }

    void GetToVariadic(va_list args) override
    {
        T* dest = va_arg(args, T*);
        *dest = m_Value;
    }

    void SetFromVariadic(va_list args) override
    {
        T* src = va_arg(args, T*);
        m_Value = *src;
    }

private:
    T m_Value{};
};

class ParameterSet
{
public:
    template <ParameterType::Value Type>
    OfxStatus Define(std::string_view name, OfxPropertySetHandle* propertySet)
    {
        return DefineEx<parameter_value_t<Type>>(name, ParameterType{Type}, propertySet);
    }

    std::optional<ParameterBase*> GetParameter(std::string_view name);

    template <ParameterType::Value Type>
    Parameter<parameter_value_t<Type>>* Find(std::string_view name)
    {
        auto it = m_Parameters.find(std::string{name});
        if (it == m_Parameters.end())
        {
            return nullptr;
        }

        return dynamic_cast<Parameter<parameter_value_t<Type>>*>(it->second.get());
    }

    template <ParameterType::Value Type>
    Parameter<parameter_value_t<Type>>& Get(std::string_view name)
    {
        auto* param = Find<Type>(name);
        assert(param && "Parameter not found or wrong type");
        return *param;
    }

    OfxParamSetHandle OfxHandle() { return reinterpret_cast<OfxParamSetHandle>(this); }
    static OfxParameterSuiteV1* as_suite();

    void DebugPrint();

private:
    template <typename T>
    OfxStatus DefineEx(std::string_view name, ParameterType type, OfxPropertySetHandle* propertySet)
    {
        const std::string name_owned{name};
        if (m_Parameters.contains(name_owned))
        {
            return kOfxStatErrExists;
        }

        const auto [it, _] = m_Parameters.emplace(name_owned, std::make_unique<Parameter<T>>(name_owned, type));
        if (propertySet)
        {
            *propertySet = it->second->PropertySetOfxHandle();
        }

        return kOfxStatOK;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ParameterBase>> m_Parameters;

    PropertySet m_Properties{"ParameterSet"};
};

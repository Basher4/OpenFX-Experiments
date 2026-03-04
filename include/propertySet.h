#pragma once

#include <map>
#include <string>
#include <string_view>

#include <openfx/ofxProperty.h>

class PropertySet {
public:
    PropertySet() = default;
    PropertySet(std::string id) : _Id(std::move(id)) {}

    OfxStatus GetPointer(std::string property, int index, void** value);
    OfxStatus SetPointer(std::string property, int index, void* value);
    OfxStatus GetInt(std::string property, int index, int* value);
    OfxStatus SetInt(std::string property, int index, int value);
    OfxStatus GetString(std::string property, int index, char** value);
    OfxStatus SetString(std::string property, int index, const char* value);
    OfxStatus GetDouble(std::string property, int index, double* value);
    OfxStatus SetDouble(std::string property, int index, double value);

    OfxPropertySetHandle OfxHandle() { return reinterpret_cast<OfxPropertySetHandle>(this); }

    void DebugPrint();
    void Clear();

public:
    static OfxPropertySuiteV1* as_suite();

private:
    using Key = std::tuple<std::string, int>;

    template <typename T>
    OfxStatus Get(const std::map<Key, T>& props, std::string_view property, int index, T* out)
    {
        const auto it = props.find(Key{property, index});
        if (it == props.end()) return kOfxStatErrUnknown;
        *out = it->second;
        return kOfxStatOK;
    }

    template <typename T, typename U>
    OfxStatus Set(std::map<Key, T>& props, std::string_view property, int index, U value)
    {
        props[Key{property, index}] = std::move(value);
        return kOfxStatOK;
    }

    template <auto Method, typename... Args>
    static OfxStatus SuiteThunk(OfxPropertySetHandle handle, const char *prop, int idx, Args... args)
    {
        return (reinterpret_cast<PropertySet*>(handle)->*Method)(prop, idx, args...);
    }

    // Hashmap would be nicer, but I'd need to tell it how to hash tuples.
    // In the proper version we can do that. Or perhaps Qt can do something?
    std::map<Key, void*> m_PropsPtr;
    std::map<Key, int> m_PropsInt;
    std::map<Key, std::string> m_PropsString;
    std::map<Key, double> m_PropsDouble;

    // Just for debug purposes.
    std::string _Id = "uninitialized";
};

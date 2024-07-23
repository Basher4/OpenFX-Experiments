#pragma once

#include <map>
#include <string>

#include <openfx/ofxProperty.h>

class PropertySet {
public:
    OfxStatus GetPointer(std::string property, int index, void** value);
    OfxStatus SetPointer(std::string property, int index, void* value);
    OfxStatus GetInt(std::string property, int index, int* value);
    OfxStatus SetInt(std::string property, int index, int value);
    OfxStatus GetString(std::string property, int index, char** value);
    OfxStatus SetString(std::string property, int index, const char* value);

public:
    static OfxPropertySuiteV1* as_suite();

private:
    // Hashmap would be nicer, but I'd need to tell it how to hash tuples.
    // In the proper version we can do that. Or perhaps Qt can do something?
    std::map<std::tuple<std::string, int>, void*> m_PropsPtr;
    std::map<std::tuple<std::string, int>, int> m_PropsInt;
    std::map<std::tuple<std::string, int>, std::string> m_PropsString;
};

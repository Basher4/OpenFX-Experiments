#pragma once

#include <string>
#include <unordered_map>

#include "openfx/ofxCore.h"
#include "propertySet.h"

class Host {
public:
    Host();

    OfxHost* OfxHandle() { return &m_OfxHost; };

private:
    static const void* s_hostFetchSuite(OfxPropertySetHandle handle, const char *suiteName, int suiteVersion);

private:
    OfxHost m_OfxHost {};
    PropertySet m_HostProps;
};

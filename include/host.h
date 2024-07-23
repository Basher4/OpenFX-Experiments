#pragma once

#include "openfx/ofxCore.h"

class Host {
public:
    Host();

public:
    OfxHost* OfxHandle() { return &m_OfxHost; };

private:
    void* fetchSuite(const char *suiteName, int suiteVersion);

    static const void* s_hostFetchSuite(OfxPropertySetHandle handle, const char *suiteName, int suiteVersion);

private:
    OfxHost m_OfxHost {};
};

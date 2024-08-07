//
// Created by user on 7/3/2024.
//

#include "host.h"

#include <cstdio>
#include <string>

#include <openfx/ofxMemory.h>
#include <openfx/ofxProperty.h>

Host::Host()
{
    m_OfxHost.host = reinterpret_cast<OfxPropertySetHandle>(&m_HostProps);
    m_OfxHost.fetchSuite = &Host::s_hostFetchSuite;

    // Initialize hashmap of property suties. We'll ignore versions.
    m_HostProps.SetPointer(kOfxPropertySuite, 1, PropertySet::as_suite());
    m_HostProps.SetPointer(kOfxMemorySuite, 1, MemorySuite::get_suite());
}

const void* Host::s_hostFetchSuite(OfxPropertySetHandle handle, const char* suiteName, int suiteVersion)
{
    printf("Plugin requesting %s version %d - ", suiteName, suiteVersion);

    const auto props = reinterpret_cast<PropertySet*>(handle);
    void *suite;
    if (const OfxStatus err = props->GetPointer(suiteName, suiteVersion, &suite); err != kOfxStatOK) {
        printf("Failed\n");
        return nullptr;
    }

    printf("OK\n");
    return suite;
}
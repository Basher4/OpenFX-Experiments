//
// Created by user on 7/3/2024.
//

#include "host.h"

#include <cstdio>

Host::Host()
{
    m_OfxHost.host = reinterpret_cast<OfxPropertySetHandle>(this);
    m_OfxHost.fetchSuite = &Host::s_hostFetchSuite;
}

void* Host::fetchSuite(const char* suiteName, int suiteVersion)
{
    printf("Plugin requesting %s version %d\n", suiteName, suiteVersion);
    return nullptr;
}

const void* Host::s_hostFetchSuite(OfxPropertySetHandle handle, const char* suiteName, int suiteVersion)
{
    const auto host = reinterpret_cast<Host*>(handle);
    return host->fetchSuite(suiteName, suiteVersion);
}
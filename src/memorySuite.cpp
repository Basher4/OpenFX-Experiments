#include <cstdlib>

#include "memorySuite.h"

OfxMemorySuiteV1 g_MemorySuite;

OfxStatus MemorySuite::memoryAlloc(void *handle, size_t nBytes, void **allocatedData)
{
    *allocatedData = malloc(nBytes);
    return *allocatedData ? kOfxStatOK : kOfxStatErrMemory;
}

OfxStatus MemorySuite::memoryFree(void *allocatedData)
{
    free(allocatedData);
    return kOfxStatOK;
}

OfxMemorySuiteV1* MemorySuite::get_suite()
{
    g_MemorySuite.memoryAlloc = MemorySuite::memoryAlloc;
    g_MemorySuite.memoryFree = MemorySuite::memoryFree;
    return &g_MemorySuite;
}

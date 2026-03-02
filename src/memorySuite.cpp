#include <cstdlib>

#include "memorySuite.h"

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
    static OfxMemorySuiteV1 g_MemorySuite {
        .memoryAlloc = memoryAlloc,
        .memoryFree = memoryFree,
    };
    return &g_MemorySuite;
}

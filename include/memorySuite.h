#pragma once

#include <openfx/ofxCore.h>
#include <openfx/ofxMemory.h>

class MemorySuite {
public:
    static OfxStatus memoryAlloc(void *handle, size_t nBytes, void **allocatedData);
    static OfxStatus memoryFree(void *allocatedData);

    static OfxMemorySuiteV1* as_suite();
};

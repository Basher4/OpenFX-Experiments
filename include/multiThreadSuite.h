#pragma once

#include <openfx/ofxCore.h>
#include <openfx/ofxMultiThread.h>

class MultiThreadSuite {
public:
    static OfxMultiThreadSuiteV1* get_suite();

private:
    // Ofx exports
    static OfxStatus spawn(OfxThreadFunctionV1 func,
                               unsigned int nThreads,
                               void *customArg);
    static OfxStatus multiThreadNumCPUs(unsigned int *nCPUs);
    static OfxStatus multiThreadIndex(unsigned int *threadIndex);
    static int multiThreadIsSpawnedThread(void);
    static OfxStatus mutexCreate(OfxMutexHandle *mutex, int lockCount);
    static OfxStatus mutexDestroy(const OfxMutexHandle mutex);
    static OfxStatus mutexLock(const OfxMutexHandle mutex);
    static OfxStatus mutexUnLock(const OfxMutexHandle mutex);
    static OfxStatus mutexTryLock(const OfxMutexHandle mutex);
};

#include "multiThreadSuite.h"

#include <cassert>
#include <mutex>
#include <mutex>

OfxStatus MultiThreadSuite::spawn(OfxThreadFunctionV1 func, unsigned int nThreads, void* customArg)
{
    return kOfxStatFailed;
}

OfxStatus MultiThreadSuite::multiThreadNumCPUs(unsigned int* nCPUs)
{
    *nCPUs = std::thread::hardware_concurrency();
    return kOfxStatOK;
}

OfxStatus MultiThreadSuite::multiThreadIndex(unsigned int* threadIndex)
{
    assert(false && "Not implemented");
}

int MultiThreadSuite::multiThreadIsSpawnedThread()
{
    assert(false && "Not implemented");
}

OfxStatus MultiThreadSuite::mutexCreate(OfxMutexHandle* mutex, int lockCount)
{
    // The description of the mutex is so strange. LockCount makes no sense to me.
    // The description of `mutexLock` says that after lock we need to prevent another
    // lock to succeed (regardless of lock count value?). And lock count can be negative...
    // Just map it to a simple mutex and see if something breaks.
    if (lockCount != 0 && lockCount != 1) {
        // Not true to spec. I could just crash...
        return kOfxStatFailed;
    }

    auto *_mutex = new std::mutex();
    if (lockCount == 1) {
        _mutex->lock();
    }

    *mutex = reinterpret_cast<OfxMutexHandle>(_mutex);
    return kOfxStatOK;
}

OfxStatus MultiThreadSuite::mutexDestroy(const OfxMutexHandle mutex)
{
    // Assume the handle is actually a mtuex.
    const auto _mutex = reinterpret_cast<std::mutex*>(mutex);
    delete _mutex;

    return kOfxStatOK;
}

OfxStatus MultiThreadSuite::mutexLock(const OfxMutexHandle mutex)
{
    auto _mutex = reinterpret_cast<std::mutex*>(mutex);
    _mutex->lock();

    return kOfxStatOK;
}

OfxStatus MultiThreadSuite::mutexUnLock(const OfxMutexHandle mutex)
{
    auto _mutex = reinterpret_cast<std::mutex*>(mutex);
    _mutex->unlock();

    return kOfxStatOK;
}

OfxStatus MultiThreadSuite::mutexTryLock(const OfxMutexHandle mutex)
{
    auto _mutex = reinterpret_cast<std::mutex*>(mutex);
    return _mutex->try_lock() ? kOfxStatOK : kOfxStatFailed;
}

OfxMultiThreadSuiteV1* MultiThreadSuite::get_suite()
{
    static OfxMultiThreadSuiteV1 g_MultiThreadSuite {
        .multiThread = spawn,
        .multiThreadNumCPUs = multiThreadNumCPUs,
        .multiThreadIndex = multiThreadIndex,
        .multiThreadIsSpawnedThread = multiThreadIsSpawnedThread,
        .mutexCreate = mutexCreate,
        .mutexDestroy = mutexDestroy,
        .mutexLock = mutexLock,
        .mutexUnLock = mutexUnLock,
        .mutexTryLock = mutexTryLock,
    };

    return &g_MultiThreadSuite;
}

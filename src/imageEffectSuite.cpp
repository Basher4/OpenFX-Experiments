#include "imageEffectSuite.h"

#include "imageEffect.h"

OfxImageEffectSuiteV1* ImageEffectSuite::as_suite()
{
    static OfxImageEffectSuiteV1 sSuite = {
        .getPropertySet = [](OfxImageEffectHandle imageEffect, OfxPropertySetHandle *propHandle) {
            auto effect = reinterpret_cast<ImageEffect*>(imageEffect);
            *propHandle = effect->PropertySetOfxHandle();
            return kOfxStatOK;
        },
        .getParamSet = [](OfxImageEffectHandle imageEffect, OfxParamSetHandle *paramSet) {
            return kOfxStatErrFatal;
        },
        .clipDefine = [](OfxImageEffectHandle imageEffect, const char *name, OfxPropertySetHandle *propertySet) {
            auto effect = reinterpret_cast<ImageEffect*>(imageEffect);
            auto propHandle = effect->ClipDefine(name);
            if (!propHandle) {
                return propHandle.error();
            }
            *propertySet = *propHandle;
            return kOfxStatOK;
        },
        .clipGetHandle = [](OfxImageEffectHandle imageEffect, const char* name, OfxImageClipHandle* clip, OfxPropertySetHandle* propertySet) {
            return kOfxStatErrFatal;
        },
        .clipGetPropertySet = [](OfxImageClipHandle clip, OfxPropertySetHandle *propHandle) {
            return kOfxStatErrFatal;
        },
        .clipGetImage = [](OfxImageClipHandle clip, OfxTime time, const OfxRectD *region, OfxPropertySetHandle *imageHandle) {
            return kOfxStatErrFatal;
        },
        .clipReleaseImage = [](OfxPropertySetHandle imageHandle) {
            return kOfxStatErrFatal;
        },
        .clipGetRegionOfDefinition = [](OfxImageClipHandle clip, OfxTime time, OfxRectD *bounds) {
            return kOfxStatErrFatal;
        },
        .abort = [](OfxImageEffectHandle imageEffect) {
            return 0;
        },
        .imageMemoryAlloc = [](OfxImageEffectHandle instanceHandle, size_t nBytes, OfxImageMemoryHandle *memoryHandle) {
            *memoryHandle = reinterpret_cast<OfxImageMemoryHandle>(new char[nBytes]);
            return kOfxStatOK;
        },
        .imageMemoryFree = [](OfxImageMemoryHandle memoryHandle) {
            delete reinterpret_cast<char*>(memoryHandle);
            return kOfxStatOK;
        },
        .imageMemoryLock = [](OfxImageMemoryHandle memoryHandle, void **returnedPtr) {
            *returnedPtr = reinterpret_cast<void*>(memoryHandle);
            return kOfxStatOK;
        },
        .imageMemoryUnlock = [](OfxImageMemoryHandle memoryHandle) {
            return kOfxStatOK;
        }
    };

    return &sSuite;
}
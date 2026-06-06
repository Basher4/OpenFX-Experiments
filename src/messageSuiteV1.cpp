#include "messageSuiteV1.h"

#include <cstdarg>
#include <cstdio>

OfxMessageSuiteV1 g_MessageSuite;

OfxStatus MessageSuiteV1::message(void *handle, const char *messageType, const char *messageId, const char *format, ...)
{
    std::printf("%s %s: ", messageType, messageId);

    va_list args;
    va_start(args, format);
    std::vprintf(format, args);
    va_end(args);

    return kOfxStatOK;
}

OfxMessageSuiteV1* MessageSuiteV1::as_suite()
{
    g_MessageSuite.message = MessageSuiteV1::message;
    return &g_MessageSuite;
}

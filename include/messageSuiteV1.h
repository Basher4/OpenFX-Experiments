#pragma once

#include <openfx/ofxMessage.h>

class MessageSuiteV1 {
public:
    static OfxStatus message(void *handle, const char *messageType, const char *messageId, const char *format, ...);
    static OfxMessageSuiteV1* get_suite();
};

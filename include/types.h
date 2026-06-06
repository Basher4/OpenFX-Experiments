#pragma once

#include "openfx/ofxCore.h"

#include <expected>

template<typename T>
using OfxResult = std::expected<T, OfxStatus>;

#define OFX_RETURN_IF_ERROR(expr)   \
    do {                            \
        auto __res = (expr);        \
        if (!__res) { return __res; } \
    } while (0)

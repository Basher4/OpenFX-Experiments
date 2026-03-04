#pragma once

#include "openfx/ofxCore.h"

#include <expected>

template<typename T>
using OfxResult = std::expected<T, OfxStatus>;
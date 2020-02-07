#pragma once

#include "PixelFormat.h"

#include <string_view>

namespace Zuazo {

constexpr std::string_view toString(PixelFormat format);

}

#include "StringConversions.inl"
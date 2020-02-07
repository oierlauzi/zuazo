#pragma once

#include "PixelFormat.h"
#include "PixelEncoding.h"

#include <string_view>

namespace Zuazo {

constexpr std::string_view toString(PixelFormat format);
constexpr std::string_view toString(PixelEncoding format);

}

#include "StringConversions.inl"
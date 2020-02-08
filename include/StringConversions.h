#pragma once

#include "ColorFormat.h"
#include "ColorEncoding.h"

#include <string_view>

namespace Zuazo {

constexpr std::string_view toString(ColorFormat format);
constexpr std::string_view toString(ColorEncoding format);

}

#include "StringConversions.inl"
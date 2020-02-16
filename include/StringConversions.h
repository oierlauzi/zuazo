#pragma once

#include "ColorFormat.h"
#include "ColorTransferFunction.h"

#include <string_view>

namespace Zuazo {

constexpr std::string_view toString(ColorFormat format);
constexpr std::string_view toString(ColorTransferFunction format);

}

#include "StringConversions.inl"
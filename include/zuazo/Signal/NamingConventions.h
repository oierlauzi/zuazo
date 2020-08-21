#pragma once

#include <string>
#include <string_view>

namespace Zuazo::Signal {

template<typename T>
constexpr std::string_view makeInputName();
template<typename T>
std::string makeInputName(size_t index);

template<typename T>
constexpr std::string_view makeOutputName();
template<typename T>
std::string makeOutputName(size_t index);

}

#include "NamingConventions.inl"
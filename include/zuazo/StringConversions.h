#pragma once

#include <string>
#include <ostream>

#include <string_view>
#include <cstddef>
#include <typeindex>
#include <utility>
#include <tuple>
#include <unordered_map>

namespace Zuazo {

//Default operators. At least one needs to be specialized
template<typename T>
std::string toString(const T& x);

//Numeric types
constexpr std::string_view toString(bool x);
size_t fromString(std::string_view str, bool& x) noexcept;
std::string	toString(char x);
size_t fromString(std::string_view str, char& x) noexcept;
std::string	toString(uint8_t x);
size_t fromString(std::string_view str, uint8_t& x) noexcept;
std::string	toString(int8_t x);
size_t fromString(std::string_view str, int8_t& x) noexcept;
std::string	toString(uint16_t x);
size_t fromString(std::string_view str, uint16_t& x) noexcept;
std::string	toString(int16_t x);
size_t fromString(std::string_view str, int16_t& x) noexcept;
std::string	toString(uint32_t x);
size_t fromString(std::string_view str, uint32_t& x) noexcept;
std::string	toString(int32_t x);
size_t fromString(std::string_view str, int32_t& x) noexcept;
std::string	toString(uint64_t x);
size_t fromString(std::string_view str, uint64_t& x) noexcept;
std::string	toString(int64_t x);
size_t fromString(std::string_view str, int64_t& x) noexcept;
std::string	toString(float x);
size_t fromString(const std::string& str, float& x) noexcept;
size_t fromString(std::string_view str, float& x) noexcept;
std::string	toString(double x);
size_t fromString(const std::string& str, double& x) noexcept;
size_t fromString(std::string_view str, double& x) noexcept;

//Provide a specialization for string
const std::string& toString(const std::string& str);
std::string_view toString(std::string_view str);
size_t fromString(std::string_view str, std::string& x) noexcept;
size_t fromString(std::string_view str, std::string_view& x) noexcept;

//Typeinfo-s
std::string_view toString(const std::type_info& tinfo) noexcept;
std::ostream& operator<<(std::ostream& os, const std::type_info& tinfo);
std::string_view toString(std::type_index tid) noexcept;
std::ostream& operator<<(std::ostream& os, std::type_index tid);

//Pair (key and value)
template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& pair);

//Tuple
template <typename... Types>
std::ostream& operator<<(std::ostream& os, const std::tuple<Types...>& tuple);

//Helper functions
template<typename T>
size_t enumFromString(std::string_view str, T& e);

template<typename T, typename F>
size_t enumFromString(std::string_view str, T& e, F&& toStringFunc);

template<typename T>
std::ostream& quote(std::ostream& os, T&& x);

std::string_view removeLeadingBlankCharacters(std::string_view str) noexcept;
std::string_view removeTrailingBlankCharacters(std::string_view str) noexcept;
std::string_view removeBlankCharacters(std::string_view str) noexcept;

template<typename... Types>
std::ostream& printAsTuple(std::ostream& os, const Types&... elements);

template<typename InputIt>
std::ostream& toOstream(std::ostream& os, InputIt begin, InputIt end);

}

#include "StringConversions.inl"
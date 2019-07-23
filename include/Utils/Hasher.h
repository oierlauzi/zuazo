#pragma once

#include <boost/container_hash/hash.hpp>

namespace Zuazo::Utils {

template <typename T>
using Hasher = boost::hash<T>;

template <size_t aligment = 1>
constexpr size_t defualt_hash_func(const void* ptr, size_t size);

template<typename T>
constexpr size_t defualt_hash_func(const T& x);

#define ZUAZO_HASH_FUNC hash_value

#define ZUAZO_DEFAULT_HASH(tname) \
constexpr auto ZUAZO_HASH_FUNC = defualt_hash_func<tname>

}

#include "Hasher.inl"
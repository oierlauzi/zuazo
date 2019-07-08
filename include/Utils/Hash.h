#pragma once

#include <sys/types.h>

namespace Zuazo::Utils {

typedef size_t hash_t;

template<typename T>
struct Hasher {
    constexpr hash_t operator()(const T& h) const;
};

constexpr hash_t hashCombine(hash_t h1, hash_t h2);

}

#include "Hash.inl"
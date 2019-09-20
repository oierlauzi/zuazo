#pragma once

#include "Math/Vector.h"
#include "Math/Rational.h"
#include "Utils/Hasher.h"

#include <sys/types.h>

namespace Zuazo {

struct Resolution{
    union {
    	uint32_t width, x;
    };

	union {
    	uint32_t height, y;
    };

    constexpr Resolution();
    constexpr Resolution(uint32_t width, uint32_t height);
    template<typename T>
    constexpr Resolution(const Math::Vec2<T>& res);
    Resolution(const Resolution& other) = default;
    ~Resolution() = default;

    template<typename T>
    constexpr operator Math::Vec2<T>() const;
    constexpr operator bool() const;

	constexpr Math::Rational_t getAspectRatio() const;

    friend size_t ZUAZO_HASH_FUNC(const Resolution& res);
};

}

#include "Resolution.inl"
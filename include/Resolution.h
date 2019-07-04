#pragma once

#include "Math/Vector.h"
#include "Math/Rational.h"

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
    constexpr Resolution(uint width, uint height);
    template<typename T>
    constexpr Resolution(const Math::Vec2<T>& res);
    Resolution(const Resolution& other) = default;
    ~Resolution() = default;

    template<typename T>
    constexpr operator Math::Vec2<T>() const;
    constexpr operator bool() const;

	constexpr Math::Rational_t getAspectRatio() const;
};

}

#include "Resolution.inl"
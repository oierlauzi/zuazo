#pragma once

#include "Math/Vector.h"
#include "Math/Rational.h"
#include "Math/Functions.h"

#include <sys/types.h>
#include <string>

namespace Zuazo {

using AspectRatio = Math::Rational<uint32_t>;

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
    constexpr Resolution(const Resolution& other) = default;
    ~Resolution() = default;

    constexpr Resolution& operator=(const Resolution& other) = default;

    constexpr bool operator==(const Resolution& other) const;
    constexpr bool operator!=(const Resolution& other) const;
    constexpr bool operator<(const Resolution& other) const;
    constexpr bool operator<=(const Resolution& other) const;
    constexpr bool operator>(const Resolution& other) const;
    constexpr bool operator>=(const Resolution& other) const;

    template<typename T>
    constexpr operator Math::Vec2<T>() const;
    constexpr operator bool() const;

	constexpr AspectRatio getAspectRatio() const;
};

std::string toString(Resolution res);

}

namespace Zuazo::Math {

constexpr Resolution min(Resolution a, Resolution b);
constexpr Resolution max(Resolution a, Resolution b);

}

#include "Resolution.inl"
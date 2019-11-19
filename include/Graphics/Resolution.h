#pragma once

#include "Vulkan.h"
#include "../Math/Vector.h"
#include "../Math/Rational.h"

#include <sys/types.h>

namespace Zuazo::Graphics {

struct Resolution{
    using Size = int32_t;

    union {
    	Size width, x;
    };

	union {
    	Size height, y;
    };

    constexpr Resolution();
    constexpr Resolution(uint32_t width, uint32_t height);
    template<typename T>
    constexpr Resolution(const Math::Vec2<T>& res);
    constexpr Resolution(vk::Extent2D res);
    constexpr Resolution(const Resolution& other) = default;
    ~Resolution() = default;

    constexpr Resolution& operator=(const Resolution& other) = default;

    constexpr bool operator==(const Resolution& other) const;
    constexpr bool operator!=(const Resolution& other) const;

    template<typename T>
    constexpr operator Math::Vec2<T>() const;
    constexpr operator vk::Extent2D() const;
    constexpr operator bool() const;

	constexpr Math::Rational<Size> getAspectRatio() const;
};

}

#include "Resolution.inl"
#pragma once

#include "Math/Vector.h"
#include "Math/Rational.h"

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

    constexpr Resolution() noexcept;
    constexpr Resolution(uint32_t width, uint32_t height) noexcept;
    template<typename T>
    explicit constexpr Resolution(const Math::Vec2<T>& res) noexcept;
    constexpr Resolution(const Resolution& other) noexcept = default;
    ~Resolution() = default;

    template<typename T>
    constexpr Resolution& operator=(const Math::Vec2<T>& res) noexcept;
    constexpr Resolution& operator=(const Resolution& other) noexcept = default;

    constexpr Resolution operator==(const Resolution& other) const noexcept;
    constexpr Resolution operator!=(const Resolution& other) const noexcept;
    constexpr Resolution operator<(const Resolution& other) const noexcept;
    constexpr Resolution operator<=(const Resolution& other) const noexcept;
    constexpr Resolution operator>(const Resolution& other) const noexcept;
    constexpr Resolution operator>=(const Resolution& other) const noexcept;

    constexpr Resolution operator+(const Resolution& other) const noexcept;
    constexpr Resolution operator-(const Resolution& other) const noexcept;
    constexpr Resolution operator*(const Resolution& other) const noexcept;
    constexpr Resolution operator/(const Resolution& other) const noexcept;
    constexpr Resolution operator%(const Resolution& other) const noexcept;

    constexpr Resolution& operator+=(const Resolution& other) noexcept;
    constexpr Resolution& operator-=(const Resolution& other) noexcept;
    constexpr Resolution& operator*=(const Resolution& other) noexcept;
    constexpr Resolution& operator/=(const Resolution& other) noexcept;
    constexpr Resolution& operator%=(const Resolution& other) noexcept;


    template<typename T>
    constexpr operator Math::Vec2<T>() const noexcept;
    constexpr operator bool() const noexcept;

	constexpr AspectRatio getAspectRatio() const noexcept;

    static constexpr Resolution min();
    static constexpr Resolution max();
};

std::ostream& operator<<(std::ostream& os, Resolution res);

size_t fromString(std::string_view str, Resolution& res) noexcept;

namespace Math {

constexpr Resolution min(const Resolution& a, const Resolution& b) noexcept;
constexpr Resolution max(const Resolution& a, const Resolution& b) noexcept;
constexpr Resolution mod(const Resolution& num, const Resolution& den) noexcept;
constexpr Resolution gcd(const Resolution& a, const Resolution& b) noexcept;
constexpr Resolution lcm(const Resolution& a, const Resolution& b) noexcept;

}

}

#include "Resolution.inl"
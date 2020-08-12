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

    constexpr Resolution();
    constexpr Resolution(uint32_t width, uint32_t height);
    template<typename T>
    explicit constexpr Resolution(const Math::Vec2<T>& res);
    constexpr Resolution(const Resolution& other) = default;
    ~Resolution() = default;

    template<typename T>
    constexpr Resolution& operator=(const Math::Vec2<T>& res);
    constexpr Resolution& operator=(const Resolution& other) = default;

    constexpr Resolution operator==(const Resolution& other) const;
    constexpr Resolution operator!=(const Resolution& other) const;
    constexpr Resolution operator<(const Resolution& other) const;
    constexpr Resolution operator<=(const Resolution& other) const;
    constexpr Resolution operator>(const Resolution& other) const;
    constexpr Resolution operator>=(const Resolution& other) const;

    constexpr Resolution operator+(const Resolution& other) const;
    constexpr Resolution operator-(const Resolution& other) const;
    constexpr Resolution operator*(const Resolution& other) const;
    constexpr Resolution operator/(const Resolution& other) const;
    constexpr Resolution operator%(const Resolution& other) const;

    constexpr Resolution& operator+=(const Resolution& other);
    constexpr Resolution& operator-=(const Resolution& other);
    constexpr Resolution& operator*=(const Resolution& other);
    constexpr Resolution& operator/=(const Resolution& other);
    constexpr Resolution& operator%=(const Resolution& other);


    template<typename T>
    constexpr operator Math::Vec2<T>() const;
    constexpr operator bool() const;

	constexpr AspectRatio getAspectRatio() const;
};

std::ostream& operator<<(std::ostream& os, Resolution res);

namespace Math {

constexpr Resolution min(const Resolution& a, const Resolution& b);
constexpr Resolution max(const Resolution& a, const Resolution& b);
constexpr Resolution mod(const Resolution& num, const Resolution& den);
constexpr Resolution gcd(const Resolution& a, const Resolution& b);
constexpr Resolution lcm(const Resolution& a, const Resolution& b);

}

namespace Utils {

template<typename T>
class Any;

constexpr Resolution lowest(const Any<Resolution>& any);
constexpr Resolution highest(const Any<Resolution>& any);

}

}

#include "Resolution.inl"
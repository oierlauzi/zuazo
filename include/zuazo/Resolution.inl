#include "Resolution.h"

#include "Math/Functions.h"

#include <limits>

namespace Zuazo {

constexpr Resolution::Resolution() noexcept
	: x(0)
	, y(0)
{
}

constexpr Resolution::Resolution(uint32_t width, uint32_t height) noexcept
	: x(width)
	, y(height)
{
}

template<typename T>
constexpr Resolution::Resolution(const Math::Vec2<T>& res) noexcept
	: x(static_cast<uint32_t>(res.x))
	, y(static_cast<uint32_t>(res.y))
{
}



template<typename T>
constexpr Resolution& Resolution::operator=(const Math::Vec2<T>& res) noexcept {
	return *this = Resolution(res);
}



constexpr Resolution Resolution::operator==(const Resolution& other) const noexcept {
	return Resolution(x == other.x, y == other.y);
}

constexpr Resolution Resolution::operator!=(const Resolution& other) const noexcept {
	return Resolution(x != other.x, y != other.y);
}

constexpr Resolution Resolution::operator<(const Resolution& other) const noexcept {
	return Resolution(x < other.x, y < other.y);
}

constexpr Resolution Resolution::operator<=(const Resolution& other) const noexcept {
	return Resolution(x <= other.x, y <= other.y);
}

constexpr Resolution Resolution::operator>(const Resolution& other) const noexcept {
	return Resolution(x > other.x, y > other.y);
}

constexpr Resolution Resolution::operator>=(const Resolution& other) const noexcept {
	return Resolution(x >= other.x, y >= other.y);
}



constexpr Resolution Resolution::operator+(const Resolution& other) const noexcept {
	return Resolution(x + other.x, y + other.y);
}

constexpr Resolution Resolution::operator-(const Resolution& other) const noexcept {
	return Resolution(x - other.x, y - other.y);
}

constexpr Resolution Resolution::operator*(const Resolution& other) const noexcept {
	return Resolution(x * other.x, y * other.y);
}

constexpr Resolution Resolution::operator/(const Resolution& other) const noexcept {
	return Resolution(x / other.x, y / other.y);
}

constexpr Resolution Resolution::operator%(const Resolution& other) const noexcept {
	return Resolution(x % other.x, y % other.y);
}



constexpr Resolution& Resolution::operator+=(const Resolution& other) noexcept {
	x += other.x; y += other.y;
	return *this;
}

constexpr Resolution& Resolution::operator-=(const Resolution& other) noexcept {
	x -= other.x; y -= other.y;
	return *this;
}

constexpr Resolution& Resolution::operator*=(const Resolution& other) noexcept {
	x *= other.x; y *= other.y;
	return *this;
}

constexpr Resolution& Resolution::operator/=(const Resolution& other) noexcept {
	x /= other.x; y /= other.y;
	return *this;
}

constexpr Resolution& Resolution::operator%=(const Resolution& other) noexcept {
	x %= other.x; y %= other.y;
	return *this;
}


template<typename T>
constexpr Resolution::operator Math::Vec2<T>() const noexcept {
    return Math::Vec2<T>(
        static_cast<T>(x),
        static_cast<T>(y)
    );
}

constexpr Resolution::operator bool() const noexcept {
    return (x > 0) && (y > 0);
}

constexpr AspectRatio Resolution::getAspectRatio() const noexcept {
    return AspectRatio(x, y);
}



inline std::ostream& operator<<(std::ostream& os, Resolution res) {
    return os << res.width << 'x' << res.height;
}


namespace Math {

constexpr Resolution min(const Resolution& a, const Resolution& b) noexcept {
    return Resolution(
        Math::min(a.x, b.x),
        Math::min(a.y, b.y)
    );
}

constexpr Resolution max(const Resolution& a, const Resolution& b) noexcept {
    return Resolution(
        Math::max(a.x, b.x),
        Math::max(a.y, b.y)
    );
}

constexpr Resolution mod(const Resolution& num, const Resolution& den) noexcept {
	return num % den;
}

constexpr Resolution gcd(const Resolution& a, const Resolution& b) noexcept {
	return Resolution(
        Math::gcd(a.x, b.x),
        Math::gcd(a.y, b.y)
    );
}

constexpr Resolution lcm(const Resolution& a, const Resolution& b) noexcept {
	return Resolution(
        Math::lcm(a.x, b.x),
        Math::lcm(a.y, b.y)
    );
}

}

namespace Utils {

constexpr Resolution lowest(const Any<Resolution>&) noexcept {
	return Resolution(0, 0);
}

constexpr Resolution highest(const Any<Resolution>&) noexcept {
	constexpr auto MAX_SIZE = std::numeric_limits<uint32_t>::max();
	return Resolution(MAX_SIZE, MAX_SIZE);
}

}

}

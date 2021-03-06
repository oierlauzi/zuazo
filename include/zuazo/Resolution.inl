#include "Resolution.h"

#include "Math/Comparisons.h"
#include "Math/Factor.h"

#include <limits>
#include <charconv>

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



constexpr Resolution Resolution::min() {
	constexpr auto MAX_SIZE = std::numeric_limits<uint32_t>::max();
	return Resolution(MAX_SIZE, MAX_SIZE);
}

constexpr Resolution Resolution::max() {
	return Resolution(0, 0);
}



inline std::ostream& operator<<(std::ostream& os, Resolution res) {
    return os << res.width << 'x' << res.height;
}

inline size_t fromString(std::string_view str, Resolution& res) noexcept {
	size_t read, result = 0;
	uint width, height;
	char separator;
	
	//Parse the width
	result += (read = fromString(str.substr(result), width));
	if(!read) {
		return 0;
	}

	//Expect a cross afterwards
	result += (read = fromString(str.substr(result), separator));
	if(!read || separator != 'x') {
		return 0;
	}

	//Parse the height
	result += (read = fromString(str.substr(result), height));
	if(!read) {
		return 0;
	}

	//Elaborate the result
	res = Resolution(width, height);
	return result;
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

}

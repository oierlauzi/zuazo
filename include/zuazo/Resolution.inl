#include "Resolution.h"

#include "Math/Functions.h"

#include <limits>

namespace Zuazo {

constexpr Resolution::Resolution() :
	x(0),
	y(0)
{
}

constexpr Resolution::Resolution(uint32_t width, uint32_t height) :
	x(width),
	y(height)
{
}

template<typename T>
constexpr Resolution::Resolution(const Math::Vec2<T>& res) :
	x(static_cast<uint32_t>(res.x)),
	y(static_cast<uint32_t>(res.y))
{
}



template<typename T>
constexpr Resolution& Resolution::operator=(const Math::Vec2<T>& res) {
	return *this = Resolution(res);
}



constexpr Resolution Resolution::operator==(const Resolution& other) const {
	return Resolution(x == other.x, y == other.y);
}

constexpr Resolution Resolution::operator!=(const Resolution& other) const {
	return Resolution(x != other.x, y != other.y);
}

constexpr Resolution Resolution::operator<(const Resolution& other) const {
	return Resolution(x < other.x, y < other.y);
}

constexpr Resolution Resolution::operator<=(const Resolution& other) const {
	return Resolution(x <= other.x, y <= other.y);
}

constexpr Resolution Resolution::operator>(const Resolution& other) const {
	return Resolution(x > other.x, y > other.y);
}

constexpr Resolution Resolution::operator>=(const Resolution& other) const {
	return Resolution(x >= other.x, y >= other.y);
}



constexpr Resolution Resolution::operator+(const Resolution& other) const {
	return Resolution(x + other.x, y + other.y);
}

constexpr Resolution Resolution::operator-(const Resolution& other) const {
	return Resolution(x - other.x, y - other.y);
}

constexpr Resolution Resolution::operator*(const Resolution& other) const {
	return Resolution(x * other.x, y * other.y);
}

constexpr Resolution Resolution::operator/(const Resolution& other) const {
	return Resolution(x / other.x, y / other.y);
}

constexpr Resolution Resolution::operator%(const Resolution& other) const {
	return Resolution(x % other.x, y % other.y);
}



constexpr Resolution& Resolution::operator+=(const Resolution& other) {
	x += other.x; y += other.y;
	return *this;
}

constexpr Resolution& Resolution::operator-=(const Resolution& other) {
	x -= other.x; y -= other.y;
	return *this;
}

constexpr Resolution& Resolution::operator*=(const Resolution& other) {
	x *= other.x; y *= other.y;
	return *this;
}

constexpr Resolution& Resolution::operator/=(const Resolution& other) {
	x /= other.x; y /= other.y;
	return *this;
}

constexpr Resolution& Resolution::operator%=(const Resolution& other) {
	x %= other.x; y %= other.y;
	return *this;
}


template<typename T>
constexpr Resolution::operator Math::Vec2<T>() const {
    return Math::Vec2<T>(
        static_cast<T>(x),
        static_cast<T>(y)
    );
}

constexpr Resolution::operator bool() const{
    return (x > 0) && (y > 0);
}

constexpr AspectRatio Resolution::getAspectRatio() const {
    return AspectRatio(x, y);
}



inline std::ostream& operator<<(std::ostream& os, Resolution res) {
    return os << res.width << 'x' << res.height;
}


namespace Math {

constexpr Resolution min(const Resolution& a, const Resolution& b) {
    return Resolution(
        Math::min(a.x, b.x),
        Math::min(a.y, b.y)
    );
}

constexpr Resolution max(const Resolution& a, const Resolution& b) {
    return Resolution(
        Math::max(a.x, b.x),
        Math::max(a.y, b.y)
    );
}

constexpr Resolution mod(const Resolution& num, const Resolution& den) {
	return num % den;
}

constexpr Resolution gcd(const Resolution& a, const Resolution& b) {
	return Resolution(
        Math::gcd(a.x, b.x),
        Math::gcd(a.y, b.y)
    );
}

constexpr Resolution lcm(const Resolution& a, const Resolution& b) {
	return Resolution(
        Math::lcm(a.x, b.x),
        Math::lcm(a.y, b.y)
    );
}

}

namespace Utils {

constexpr Resolution lowest(const Any<Resolution>&) {
	return Resolution(0, 0);
}

constexpr Resolution highest(const Any<Resolution>&) {
	constexpr auto MAX_SIZE = std::numeric_limits<uint32_t>::max();
	return Resolution(MAX_SIZE, MAX_SIZE);
}

}

}

#include "Resolution.h"

#include <tuple>

namespace Zuazo {

constexpr Resolution::Resolution() :
    width(0),
    height(0)
{
}

constexpr Resolution::Resolution(uint32_t width, uint32_t height) :
    width(width),
    height(height)
{
}

template<typename T>
constexpr Resolution::Resolution(const Math::Vec2<T>& res) :
    width(static_cast<uint32_t>(res.x)),
    height(static_cast<uint32_t>(res.y))
{
}

constexpr bool Resolution::operator==(const Resolution& other) const {
    return std::tie(x, y) == std::tie(other.x, other.y);
}

constexpr bool Resolution::operator!=(const Resolution& other) const {
    return std::tie(x, y) != std::tie(other.x, other.y);
}

constexpr bool Resolution::operator<(const Resolution& other) const {
    return std::tie(x, y) < std::tie(other.x, other.y);
}

constexpr bool Resolution::operator<=(const Resolution& other) const {
    return std::tie(x, y) <= std::tie(other.x, other.y);
}

constexpr bool Resolution::operator>(const Resolution& other) const {
    return std::tie(x, y) > std::tie(other.x, other.y);
}

constexpr bool Resolution::operator>=(const Resolution& other) const {
    return std::tie(x, y) >= std::tie(other.x, other.y);
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



inline std::string toString(Resolution res) {
    return toString(res.width) + "x" + toString(res.height);
}

}



namespace Zuazo::Utils {
    
constexpr Resolution min(Resolution a, Resolution b) {
    return Resolution(
        min(a.x, b.x),
        min(a.y, b.y)
    );
}

constexpr Resolution max(Resolution a, Resolution b) {
    return Resolution(
        max(a.x, b.x),
        max(a.y, b.y)
    );
}

constexpr Resolution clamp(Resolution val, Resolution lo, Resolution hi) {
    return Resolution(
        clamp(val.x, lo.x, hi.x),
        clamp(val.y, lo.y, hi.y)
    );
}

}
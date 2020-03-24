#include "Resolution.h"

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
    return (width == other.width) && (height == other.height);
}

constexpr bool Resolution::operator!=(const Resolution& other) const {
    return !(*this == other);
}

template<typename T>
constexpr Resolution::operator Math::Vec2<T>() const {
    return Math::Vec2<T>(
        static_cast<T>(width),
        static_cast<T>(height)
    );
}

constexpr Resolution::operator bool() const{
    return (width > 0) && (height > 0);
}

constexpr AspectRatio Resolution::getAspectRatio() const {
    return AspectRatio(
        width,
        height
    );
}

inline std::string toString(Resolution res) {
    return std::to_string(res.width) + "x" + std::to_string(res.height);
}

}
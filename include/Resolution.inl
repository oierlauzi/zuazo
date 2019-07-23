namespace Zuazo {

constexpr Resolution::Resolution() :
    width(0),
    height(0)
{
}

constexpr Resolution::Resolution(uint width, uint height) :
    width(width),
    height(height)
{
}

template<typename T>
constexpr Resolution::Resolution(const Math::Vec2<T>& res) :
    width(static_cast<uint>(res.x)),
    height(static_cast<uint>(res.y))
{
}

template<typename T>
constexpr Resolution::operator Math::Vec2<T>() const {
    return Math::Vec2<T>(
        static_cast<T>(width),
        static_cast<T>(height)
    );
}

constexpr Resolution::operator bool() const{
    return width && height;
}

constexpr Math::Rational_t Resolution::getAspectRatio() const {
    return Math::Rational_t(
        static_cast<Math::Rational_t::Integer>(width),
        static_cast<Math::Rational_t::Integer>(height)
    );
}

constexpr size_t ZUAZO_HASH_FUNC(const Resolution& res){
    return res.x << 32 | res.y;
}

}
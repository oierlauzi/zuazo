namespace Zuazo{

constexpr PixelComponent::PixelComponent() :
    type(NONE),
    depth(0),
    plane(0),
    flags(0),
    subsampling(1)
{
}

constexpr PixelComponent::PixelComponent(PixelComponentType type, uint depth, uint plane = 0, uint32_t flags = 0, Math::Rational32_t subs = 1) :
    type(type),
    depth(depth),
    plane(plane),
    flags(flags),
    subsampling(subs)
{
}

}
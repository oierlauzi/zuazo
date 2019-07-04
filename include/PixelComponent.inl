namespace Zuazo {

constexpr PixelComponent::Subsampling::Subsampling() :
	x(1, 1),
	y(1, 1)
{
}

constexpr PixelComponent::Subsampling::Subsampling(uint16_t x, uint16_t y) :
	x(1, static_cast<Rational::Integer>(x)),
	y(1, static_cast<Rational::Integer>(y))
{
}

constexpr PixelComponent::PixelComponent() :
	type(NONE),
	depth(0),
	plane(0),
	subsampling(),
	flags(0)
{
}

constexpr PixelComponent::PixelComponent(uint depth) :
	type(NONE),
	depth(depth),
	plane(0),
	subsampling(),
	flags(0)
{
}

constexpr PixelComponent::PixelComponent(PixelComponentType type, uint depth, uint plane, const Subsampling& subs, Flags flags) :
	type(type),
	depth(depth),
	plane(plane),
	subsampling(subs),
	flags(flags)
{
}

constexpr PixelComponent::PixelComponent(const PixelComponent& other, uint plane, const Subsampling& subs) : 
	type(other.type),
	depth(other.depth),
	plane(plane),
	subsampling(subs),
	flags(other.flags)
{
}

constexpr PixelComponent::operator bool() const{
	return depth && type;
}

}
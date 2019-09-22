#include "PixelComponent.h"

namespace Zuazo::Graphics {

constexpr PixelComponent::PixelComponent() :
	type(NONE),
	depth(0),
	plane(0),
	subsampling(),
	flags(EMPTY)
{
}

constexpr PixelComponent::PixelComponent(uint depth) :
	type(NONE),
	depth(depth),
	plane(0),
	subsampling(),
	flags(EMPTY)
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
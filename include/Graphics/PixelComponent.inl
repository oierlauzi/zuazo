#include "PixelComponent.h"

namespace Zuazo::Graphics {

constexpr bool PixelComponent::operator==(const PixelComponent& other) const{
	return 
		type == other.type &&
		depth == other.depth &&
		flags == other.flags &&
		plane == other.plane &&
		subsampling.x == other.subsampling.x &&
		subsampling.y == other.subsampling.y;
}

constexpr bool PixelComponent::operator!=(const PixelComponent& other) const{
	return !operator==(other);
}


constexpr PixelComponent::operator bool() const{
	return depth && ( type != Type::NONE );
}

constexpr PixelComponent modifyType(const PixelComponent& component, PixelComponent::Type newType){
	return PixelComponent {
		newType,
		component.depth,
		component.flags,
		component.plane,
		component.subsampling
	};
}

constexpr PixelComponent modifyDepth(const PixelComponent& component, uint32_t newDepth){
	return PixelComponent {
		component.type,
		newDepth,
		component.flags,
		component.plane,
		component.subsampling
	};
}

constexpr PixelComponent modifyFlags(const PixelComponent& component, PixelComponent::Flags newFlags){
	return PixelComponent {
		component.type,
		component.depth,
		newFlags,
		component.plane,
		component.subsampling
	};
}

constexpr PixelComponent modifyPlane(const PixelComponent& component, uint32_t newPlane){
	return PixelComponent {
		component.type,
		component.depth,
		component.flags,
		newPlane,
		component.subsampling
	};
}

constexpr PixelComponent modifySubsampling(const PixelComponent& component, PixelComponent::Subsampling newSubsampling){
	return PixelComponent {
		component.type,
		component.depth,
		component.flags,
		component.plane,
		newSubsampling
	};
}


}
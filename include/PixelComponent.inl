#include "PixelComponent.h"

namespace Zuazo {

constexpr bool PixelComponent::operator==(const PixelComponent& other) const{
	return 
		type == other.type &&
		depth == other.depth &&
		plane == other.plane &&
		subsampling.x == other.subsampling.x &&
		subsampling.y == other.subsampling.y &&
		flags == other.flags;
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
		component.plane,
		component.subsampling,
		component.flags
	};
}

constexpr PixelComponent modifyDepth(const PixelComponent& component, uint32_t newDepth){
	return PixelComponent {
		component.type,
		newDepth,
		component.plane,
		component.subsampling,
		component.flags
	};
}

constexpr PixelComponent modifyPlane(const PixelComponent& component, uint32_t newPlane){
	return PixelComponent {
		component.type,
		component.depth,
		newPlane,
		component.subsampling,
		component.flags
	};
}

constexpr PixelComponent modifySubsampling(const PixelComponent& component, PixelComponent::Subsampling newSubsampling){
	return PixelComponent {
		component.type,
		component.depth,
		component.plane,
		newSubsampling,
		component.flags
	};
}

constexpr PixelComponent modifyFlags(const PixelComponent& component, PixelComponent::Flags newFlags){
	return PixelComponent {
		component.type,
		component.depth,
		component.plane,
		component.subsampling,
		newFlags
	};
}

}
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

constexpr bool areCompatible(PixelComponent a, PixelComponent b){
	PixelComponent::Flags flags = {};

	//Ignore forced endianess if needed
	#if ZUAZO_IS_LITTLE_ENDIAN
		flags |= PixelComponent::Flags::FORCE_LITTLE_ENDIAN;
	#elif ZUAZO_IS_BIG_ENDIAN
		flags |= PixelComponent::Flags::FORCE_BIG_ENDIAN;
	#endif

	PixelComponent::Flags mask = ~flags;

	a = modifyFlags(a, a.flags & mask);	
	b = modifyFlags(b, b.flags & mask);	

	//Ignore empty componets
	if(a.depth == 0){
		a = {};
	}

	if(b.depth == 0){
		b = {};
	}

	return a == b;
}

}
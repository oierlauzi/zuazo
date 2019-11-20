#include "PixelFormat.h"

namespace Zuazo::Graphics {


constexpr PixelFormat::operator bool() const{
	for(const auto& comp : components){
		if(comp){
			return true;
		}
	}

	return false;
}

constexpr bool PixelFormat::operator==(const PixelFormat& other) const{
	for(size_t i = 0; i < MAX_COMPONENTS; i++){
		if(components[i] != other.components[i]){
			return false;
		}
	}

	return true;
}

constexpr bool PixelFormat::operator!=(const PixelFormat& other) const{
	return !operator==(other);
}




constexpr bool PixelFormat::isRGB() const{
	for(const auto& comp : components){
		if(	comp.type == PixelComponent::Type::Y ||
			comp.type == PixelComponent::Type::U ||
			comp.type == PixelComponent::Type::V )
		{
			return false;
		}
	}

	return true;
}

constexpr bool PixelFormat::isYUV() const{
	for(const auto& comp : components){
		if(	comp.type == PixelComponent::Type::R ||
			comp.type == PixelComponent::Type::G ||
			comp.type == PixelComponent::Type::B )
		{
			return false;
		}
	}

	return true;
}

constexpr bool PixelFormat::isPacked() const{
	for(size_t plane = 0; plane < MAX_COMPONENTS; plane++){
		size_t componentCount = 0;

		for(const auto& comp : components){
			if(comp.plane == plane){
				componentCount++;
			} 
		}

		if(componentCount > 1){
			return true; //There is more than one component on this plane
		}
	}

	return false;
}

constexpr bool PixelFormat::isPlanar() const{
	for(const auto& comp : components){
		if(comp.plane > 0) {
			return true;
		}
	}

	return false;
}

constexpr bool PixelFormat::hasColor() const{
	for(const auto& comp : components){
		if(	comp.type != PixelComponent::Type::NONE ||
			comp.type != PixelComponent::Type::Y ||
			comp.type != PixelComponent::Type::A )
		{
			return true;
		}
	}

	return false;
}

constexpr bool PixelFormat::hasAlpha() const{
	for(const auto& comp : components){
		if(	comp.type == PixelComponent::Type::A ){
			return true;
		}
	}

	return false;
}

constexpr size_t PixelFormat::getComponentCount() const{
	size_t count = 0;

	for(const auto& comp : components){
		if(comp){
			count++;
		}
	}

	return count;
}

constexpr size_t PixelFormat::getPlaneCount() const{
	size_t maxPlane = 0;

	for(const auto& comp : components){
		if(comp.plane > maxPlane){
			maxPlane = comp.plane;
		}
	}

	return maxPlane;
}

}
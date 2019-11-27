#include "PixelFormat.h"

namespace Zuazo {


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

	return wordSize == other.wordSize;
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

constexpr PixelFormat modifyComponents(const PixelFormat& fmt, const PixelFormat::ComponentArray& newComponents){
	return PixelFormat {
		newComponents,
		fmt.wordSize
	};
}

constexpr PixelFormat modifyWordSize(const PixelFormat& fmt, uint32_t newWordSize) {
	return PixelFormat {
		fmt.components,
		newWordSize
	};
}

constexpr PixelFormat getMemoryRepresentation(const PixelFormat& fmt){
	#if ZUAZO_IS_LITTLE_ENDIAN

		if(fmt.wordSize == 0){
			return fmt; //Already a memory representation
		} 

		if(fmt.isPlanar()){
			return fmt; //Only packed formats are supported. //FIXME Add support for semiplanar formats
		}

		if(fmt.wordSize % ZUAZO_BYTE_SIZE){
			return fmt; //Word size not supported by CPU
		}

		for(const auto& comp : fmt.components){
			if(comp.depth == 0){
				continue;
			}

			if((comp.depth % ZUAZO_BYTE_SIZE) && (ZUAZO_BYTE_SIZE % comp.depth)){ 
				return fmt; //Size not multiple of a byte nor nibble
			}
		}

		PixelFormat result = {};
		uint32_t size = 0;

		for(size_t i = 0; i < PixelFormat::MAX_COMPONENTS;) {
			size_t count = 0;
			size = 0;

			//Count the components corresponding to this iteration
			for(size_t j = i; j < PixelFormat::MAX_COMPONENTS && size < fmt.wordSize; j++){
				count++;
				size += fmt.components[j].depth;
			}

			if(size == 0){
				break; //Nothing else here
			}

			if(size != fmt.wordSize){
				return fmt; //Incompatible size
			}

			//Reverse components
			for(size_t j = 0; j < count;){

				//Get if a byte contains multiple components
				size_t components = 0;
				size = 0;
				for(size_t h = 0; size < ZUAZO_BYTE_SIZE; h++){
					components++;
					size += fmt.components[i + h].depth;
				}

				for(size_t h = 0; h < components; h++){
					result.components[i + count - components - j + h] = fmt.components[i + j + h];
				}

				j += components;
			}

			i += count; //Advance
		}


		return result;

	#elif ZUAZO_IS_BIG_ENDIAN
		return modifyWordSize(fmt, 0);
	#else
		#pragma warning ("System endianess not supported")
		return fmt;
	#endif
}

constexpr PixelFormat reverse(const PixelFormat& fmt){
	PixelFormat::ComponentArray comp;

	for(size_t i = 0; i < PixelFormat::MAX_COMPONENTS; i++){
		comp[i] = fmt.components[PixelFormat::MAX_COMPONENTS - i - 1];
	}

	return modifyComponents(fmt, comp);
}

}
namespace Zuazo {

constexpr PixelFormat::PixelFormat(const std::initializer_list<PixelComponent>& comp){
	auto inIte = comp.begin();
	auto outIte = m_components.begin();

	while(inIte != comp.end() && outIte != m_components.end()){
		*(outIte++) = *(inIte++); 
	}
}

constexpr PixelFormat::operator bool() const{
	bool result = false;

	for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
		if(*ite){
			result = true;
			break;
		}
	}
	
	return result;
}

constexpr const PixelFormat::ComponentArray& PixelFormat::getComponents() const{
	return m_components;
}

constexpr uint PixelFormat::getComponentCount() const{
	uint count = 0;

	for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
		if(*ite){
			++count;
		}
	}

	return count;
}

constexpr uint PixelFormat::getLength() const{
	uint count = 0;

	for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
		if(ite->depth){
			++count;
		}
	}

	return count;
}

constexpr uint PixelFormat::getPlaneCount() const{
	uint count = 0;

	for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
		if(ite->plane > count){
			count = ite->plane; //Get the highest plane reference 
		}
	}

	return count + 1; //Planes start form 0, so +1
}

constexpr Math::Rational32_t PixelFormat::getSize() const{
	Math::Rational32_t size = 0;

	for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
		size += ite->depth * ite->subsampling.x * ite->subsampling.y;
	}

	return size;
}

constexpr Math::Rational32_t PixelFormat::getPlaneSize(uint plane) const{
	Math::Rational32_t size = 0;

	for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
		if(ite->plane == plane){
			size += ite->depth * ite->subsampling.x * ite->subsampling.y;
		}
	}

	return size;
}

constexpr PixelFormat::PlanarType PixelFormat::getPlanarType() const{
	std::array<uint, MAX_PIXEL_COMOPONENTS> useCount = {0};

	//Start counting
	for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
		if(ite->plane < m_components.size()){ //This condition should be always true
			++useCount[ite->plane];
		}
	}

	PlanarType result = NONE;

	//It is packed as long as a plane has more than 1 components
	for(auto ite = useCount.begin(); ite != useCount.end(); ++ite){
		if(*ite > 1){
			result |= PACKED;
			break;
		}
	}

	//It is packed as long as there are more than 1 planes
	for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
		if(ite->plane > 0){
			result |= PLANAR;
			break;
		}
	}

	return result;
}

constexpr uint PixelFormat::getOffset(ComponentArray::const_iterator el) const{
	uint result = 0;

	//TODO

	return result;
}

constexpr bool PixelFormat::hasColor() const{
	bool has_color = false;

	for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
		if(ite->type && ite->type != PixelComponent::ALPHA && ite->type != PixelComponent::LUMINANCE){
			has_color = true;
			break;
		}
	}
	
	return has_color;
}

constexpr bool PixelFormat::hasAlpha() const{
	bool has_alpha = false;

	for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
		if(ite->type == PixelComponent::ALPHA){
			has_alpha = true;
			break;
		}
	}
	
	return has_alpha;
}

namespace PixelFormats{
	#define ZUAZO_FCC_PAIR(x, y) { ZUAZO_FCC2INT(x), y }

	constexpr std::pair<uint32_t, PixelFormat> _fccConversionTable[] = {
		// ADD HERE

		{0, 		NONE}
	};

	constexpr uint32_t _fcc2uint(char c0, char c1, char c2, char c3){
		uint32_t fcc = c0 << 3;
		fcc |= c1 << 2;
		fcc |= c2 << 1;
		fcc |= c3 << 0;
		return fcc;
	}

	constexpr PixelFormat fourcc(uint32_t fcc){
		PixelFormat	result = NONE;

		for(uint i = 0; _fccConversionTable[i]->first; ++i){
			if(_fccConversionTable[i]->first == fcc){
				result = _fccConversionTable[i]->second;
				break;
			}
		}

		return result;
	}

	constexpr PixelFormat fourcc(char c0, char c1, char c2, char c3){
		return fourcc(_fcc2uint(c0, c1, c2, c3));
	}

	PixelFormat fourcc(const std::string& str){
		PixelFormat result = NONE;

		if(str.size() == 4){
			result = fourcc(str[0], str[1], str[2], str[3]);
		}

		return result;
	}
}

}
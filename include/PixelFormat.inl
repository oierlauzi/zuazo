namespace Zuazo {

constexpr PixelFormat::operator bool() const{
	bool result = false;

	for(auto ite = cbegin(); ite != cend(); ++ite){
		if(*ite){
			result = true;
			break;
		}
	}
	
	return result;
}

constexpr uint PixelFormat::getComponentCount() const{
	uint count = 0;

	for(auto ite = cbegin(); ite != cend(); ++ite){
		++count;
	}

	return count;
}

constexpr uint PixelFormat::getPlaneCount() const{
	uint count = 0;

	for(auto ite = cbegin(); ite != cend(); ++ite){
		if(ite->plane > count){
			count = ite->plane; //Get the highest plane reference 
		}
	}

	return count + 1; //Planes start form 0, so +1
}

constexpr Math::Rational32_t PixelFormat::getSize() const{
	Math::Rational32_t size = 0;

	for(auto ite = cbegin(); ite != cend(); ++ite){
		size += ite->depth * ite->subsampling.x * ite->subsampling.y;
	}

	return size;
}

constexpr Math::Rational32_t PixelFormat::getPlaneSize(uint plane) const{
	Math::Rational32_t size = 0;

	for(auto ite = cbegin(); ite != cend(); ++ite){
		if(ite->plane == plane){
			size += ite->depth * ite->subsampling.x * ite->subsampling.y;
		}
	}

	return size;
}

constexpr PixelFormat::PlanarType PixelFormat::getPlanarType() const{
	uint useCount[size()];

	//Set the use count array to 0s
	for(uint i = 0; i < size(); ++i){
		useCount[i] = 0;
	}

	//Start counting
	for(auto ite = cbegin(); ite != cend(); ++ite){
		if(ite->plane < size()){ //This condition should be always true
			++useCount[ite->plane];
		}
	}

	PlanarType result = NONE;

	//It is packed as long as a plane has more than 1 components
	for(uint i = 0; i < size(); ++i){
		if(useCount[i] > 1){
			result |= PACKED;
			break;
		}
	}

	//It is packed as long as there are more than 1 planes
	for(auto ite = cbegin(); ite != cend(); ++ite){
		if(ite->plane > 0){
			result |= PLANAR;
			break;
		}
	}

	return result;
}

constexpr bool PixelFormat::hasColor() const{
	bool has_color = false;

	for(auto ite = cbegin(); ite != cend(); ++ite){
		if(ite->type && ite->type != PixelComponent::ALPHA && ite->type != PixelComponent::LUMINANCE){
			has_color = true;
			break;
		}
	}
	
	return has_color;
}

constexpr bool PixelFormat::hasAlpha() const{
	bool has_alpha = false;

	for(auto ite = cbegin(); ite != cend(); ++ite){
		if(ite->type == PixelComponent::ALPHA){
			has_alpha = true;
			break;
		}
	}
	
	return has_alpha;
}

}
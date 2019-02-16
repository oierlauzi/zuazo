#pragma once

#include <bits/stdint-intn.h>
#include <GL/glew.h>
#include <sys/types.h>

#include "../../Utils/PixelTypes.h"

namespace Zuazo::Graphics::GL{

template <typename T>
constexpr GLenum GLType									=GL_NONE;
template <>
constexpr GLenum GLType<float> 							=GL_FLOAT;
template <>
constexpr GLenum GLType<double>							=GL_DOUBLE;
template <>
constexpr GLenum GLType<int16_t> 						=GL_SHORT;
template <>
constexpr GLenum GLType<int32_t> 						=GL_INT;
template <>
constexpr GLenum GLType<u_int8_t> 						=GL_UNSIGNED_BYTE;
template <>
constexpr GLenum GLType<u_int16_t> 						=GL_UNSIGNED_SHORT;
template <>
constexpr GLenum GLType<u_int32_t> 						=GL_UNSIGNED_INT;

template <Utils::PixelTypes T>
constexpr GLenum GLPixel 								=GL_NONE;
template <>
constexpr GLenum GLPixel<Utils::PixelTypes::RED> 		=GL_RED;
template <>
constexpr GLenum GLPixel<Utils::PixelTypes::GREEN> 		=GL_GREEN;
template <>
constexpr GLenum GLPixel<Utils::PixelTypes::BLUE> 		=GL_BLUE;
template <>
constexpr GLenum GLPixel<Utils::PixelTypes::ALPHA> 		=GL_ALPHA;
template <>
constexpr GLenum GLPixel<Utils::PixelTypes::RGB> 		=GL_RGB;
template <>
constexpr GLenum GLPixel<Utils::PixelTypes::RGBA> 		=GL_RGBA;
template <>
constexpr GLenum GLPixel<Utils::PixelTypes::LUMA> 		=GL_LUMINANCE;
template <>
constexpr GLenum GLPixel<Utils::PixelTypes::LUMA_ALPHA>	=GL_LUMINANCE_ALPHA;

constexpr GLenum toGLenum(Utils::PixelTypes type){
	GLenum result=0;
	switch(type){
	case Utils::PixelTypes::RED:
		result=GLPixel<Utils::PixelTypes::RED>;
		break;
	case Utils::PixelTypes::GREEN:
		result=GLPixel<Utils::PixelTypes::GREEN>;
		break;
	case Utils::PixelTypes::BLUE:
		result=GLPixel<Utils::PixelTypes::BLUE>;
		break;
	case Utils::PixelTypes::ALPHA:
		result=GLPixel<Utils::PixelTypes::ALPHA>;
		break;
	case Utils::PixelTypes::RGB:
		result=GLPixel<Utils::PixelTypes::RGB>;
		break;
	case Utils::PixelTypes::RGBA:
		result=GLPixel<Utils::PixelTypes::RGBA>;
		break;
	case Utils::PixelTypes::LUMA:
		result=GLPixel<Utils::PixelTypes::LUMA>;
		break;
	case Utils::PixelTypes::LUMA_ALPHA:
		result=GLPixel<Utils::PixelTypes::LUMA_ALPHA>;
		break;
	default:
		result=GL_NONE;
		break;
	}
	return result;
}

}

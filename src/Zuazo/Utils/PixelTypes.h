#pragma once

#include <GL/glew.h>
#include <sys/types.h>

namespace Zuazo::Utils{

/**
 * @brief Pixel types supported by Zuazo
 */
enum class PixelTypes{
	RED				=GL_RED,
	GREEN			=GL_GREEN,
	BLUE			=GL_BLUE,
	ALPHA			=GL_ALPHA,
	RGB				=GL_RGB,
	RGBA			=GL_RGBA,
	LUMA			=GL_LUMINANCE,
	LUMA_ALPHA		=GL_LUMINANCE_ALPHA,
};

/**
 * @brief The size of the templated pixel in bytes,
 * assuming 1 byte per component
 */
template<PixelTypes type>
size_t PIXEL_SIZE=1;
template<>
size_t PIXEL_SIZE<PixelTypes::RED>			=1 * sizeof(u_int8_t);
template<>
size_t PIXEL_SIZE<PixelTypes::GREEN>			=1 * sizeof(u_int8_t);
template<>
size_t PIXEL_SIZE<PixelTypes::BLUE>			=1 * sizeof(u_int8_t);
template<>
size_t PIXEL_SIZE<PixelTypes::ALPHA>			=1 * sizeof(u_int8_t);
template<>
size_t PIXEL_SIZE<PixelTypes::RGB>			=3 * sizeof(u_int8_t);
template<>
size_t PIXEL_SIZE<PixelTypes::RGBA>			=4 * sizeof(u_int8_t);
template<>
size_t PIXEL_SIZE<PixelTypes::LUMA>			=1 * sizeof(u_int8_t);
template<>
size_t PIXEL_SIZE<PixelTypes::LUMA_ALPHA>	=2 * sizeof(u_int8_t);

}

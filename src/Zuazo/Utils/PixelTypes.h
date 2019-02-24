#pragma once

#include <glad/glad.h>
#include <sys/types.h>

namespace Zuazo::Utils{

/**
 * @brief Pixel types supported by Zuazo
 */
enum class PixelTypes{
	NONE	=GL_NONE,
	RED		=GL_RED,
	GREEN	=GL_GREEN,
	BLUE	=GL_BLUE,
	ALPHA	=GL_ALPHA,
	RGB		=GL_RGB,
	BGR		=GL_BGR,
	RGBA	=GL_RGBA,
	BGRA	=GL_BGRA,
};

/**
 * @brief The size of the templated pixel in bytes,
 * assuming 1 byte per component
 */
template<PixelTypes type>
constexpr size_t PIXEL_SIZE=0;
template<>
constexpr size_t PIXEL_SIZE<PixelTypes::NONE>			=0;
template<>
constexpr size_t PIXEL_SIZE<PixelTypes::RED>			=1 * sizeof(u_int8_t);
template<>
constexpr size_t PIXEL_SIZE<PixelTypes::GREEN>			=1 * sizeof(u_int8_t);
template<>
constexpr size_t PIXEL_SIZE<PixelTypes::BLUE>			=1 * sizeof(u_int8_t);
template<>
constexpr size_t PIXEL_SIZE<PixelTypes::ALPHA>			=1 * sizeof(u_int8_t);
template<>
constexpr size_t PIXEL_SIZE<PixelTypes::RGB>			=3 * sizeof(u_int8_t);
template<>
constexpr size_t PIXEL_SIZE<PixelTypes::BGR>			=3 * sizeof(u_int8_t);
template<>
constexpr size_t PIXEL_SIZE<PixelTypes::RGBA>			=4 * sizeof(u_int8_t);
template<>
constexpr size_t PIXEL_SIZE<PixelTypes::BGRA>			=4 * sizeof(u_int8_t);

}

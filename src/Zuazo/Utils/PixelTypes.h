#pragma once

#include <glad/glad.h>
#include <sys/types.h>

namespace Zuazo::Utils{

/**
 * @brief Pixel types supported by Zuazo
 */
enum class PixelTypes{
	RED,
	GREEN,
	BLUE,
	ALPHA,
	RGB,
	RGBA,
};

/**
 * @brief The size of the templated pixel in bytes,
 * assuming 1 byte per component
 */
template<PixelTypes type>
constexpr size_t PIXEL_SIZE=1;
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
constexpr size_t PIXEL_SIZE<PixelTypes::RGBA>			=4 * sizeof(u_int8_t);

}

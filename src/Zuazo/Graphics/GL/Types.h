#pragma once

#include <GL/glew.h>
#include <sys/types.h>

namespace Zuazo::Graphics::GL{

	template <typename T>
	constexpr GLenum GL=0;
	constexpr GLenum GL<float> 			=GL_FLOAT;
	constexpr GLenum GL<double>			=GL_DOUBLE;
	constexpr GLenum GL<int16_t> 		=GL_SHORT;
	constexpr GLenum GL<int32_t> 		=GL_INT;
	constexpr GLenum GL<u_int8_t> 		=GL_UNSIGNED_BYTE;
	constexpr GLenum GL<u_int16_t> 		=GL_UNSIGNED_SHORT;
	constexpr GLenum GL<u_int32_t> 		=GL_UNSIGNED_INT;
}

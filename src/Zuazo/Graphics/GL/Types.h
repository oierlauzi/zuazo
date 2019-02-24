#pragma once

#include <bits/stdint-intn.h>
#include <glad/glad.h>
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
constexpr GLenum GLPixel=(GLenum)T;

}

/*
 * This header file will be included from GLSL shaders, so C++ content
 * is discriminaded by macro definition __cplusplus
 */
#ifdef __cplusplus
	#pragma once
	#define ZUAZO_IF_CPP(x, y) x

	#include "../Math/Vector.h"
	#include "../Math/Matrix.h"

	namespace Zuazo::Graphics {
#else
	#define ZUAZO_IF_CPP(x, y) y
#endif

ZUAZO_IF_CPP(constexpr uint32_t, const uint) SAMPLER_COUNT = 4;
ZUAZO_IF_CPP(constexpr uint32_t, const uint) SAMPLER_BINDING = 0;
ZUAZO_IF_CPP(constexpr uint32_t, const uint) COLOR_TRANSFER_BINDING = 1;

ZUAZO_IF_CPP(constexpr int32_t, const int) PLANE_FORMAT_RGBA = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) PLANE_FORMAT_G_BR = 1;
ZUAZO_IF_CPP(constexpr int32_t, const int) PLANE_FORMAT_G_B_R = 2;

ZUAZO_IF_CPP(constexpr int32_t, const int) COLOR_RANGE_FULL_RGB = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) COLOR_RANGE_FULL_YCBCR = 1;
ZUAZO_IF_CPP(constexpr int32_t, const int) COLOR_RANGE_ITU_NARROW_RGB = 2;
ZUAZO_IF_CPP(constexpr int32_t, const int) COLOR_RANGE_ITU_NARROW_YCBCR = 3;

ZUAZO_IF_CPP(constexpr int32_t, const int) COLOR_TRANSFER_FUNCTION_LINEAR = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) COLOR_TRANSFER_FUNCTION_IEC61966_2_1 = 1;



struct ColorTransfer {
	ZUAZO_IF_CPP(glm::mat4, mat4) 		colorPrimaries;
	ZUAZO_IF_CPP(glm::mat4, mat4) 		colorModel;
	ZUAZO_IF_CPP(int32_t, int) 			colorTransferFunction;
	ZUAZO_IF_CPP(int32_t, int) 			colorRange;
	ZUAZO_IF_CPP(int32_t, int) 			planeFormat;
};

#ifdef __cplusplus
	} // namespace Zuazo::Graphics
#endif
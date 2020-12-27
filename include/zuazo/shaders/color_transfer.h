/*
 * This header file will be included from GLSL shaders, so C++ content
 * is discriminaded by macro definition __cplusplus
 */
#ifdef __cplusplus
	#pragma once
	#define ZUAZO_IF_CPP(x, y) x

	#include <zuazo/Math/Vector.h>
	#include <zuazo/Math/Matrix.h>

	#include <cstdint>

#else
	#define ZUAZO_IF_CPP(x, y) y
#endif

ZUAZO_IF_CPP(constexpr uint32_t, const uint) ct_SAMPLER_COUNT = 4;
ZUAZO_IF_CPP(constexpr uint32_t, const uint) ct_SAMPLER_BINDING = 0;
ZUAZO_IF_CPP(constexpr uint32_t, const uint) ct_DATA_BINDING = 1;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_SAMPLE_MODE_PASSTHOUGH = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_SAMPLE_MODE_BILINEAR2BICUBIC = 1;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_PLANE_FORMAT_RGBA = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_PLANE_FORMAT_G_BR = 1;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_PLANE_FORMAT_G_B_R = 2;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_PLANE_FORMAT_G_B_R_A = 3;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_RANGE_FULL = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_RANGE_ITU_NARROW_RGB = 1;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_RANGE_ITU_NARROW_YCBCR = 2;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_MODEL_RGB = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_MODEL_YCBCR = 1;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_LINEAR = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_BT601_709_2020 = 1;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_GAMMA22 = 2;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_GAMMA26 = 3;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_GAMMA28 = 4;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1 = 5;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_4 = 6;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_SMPTE240M = 7;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_SMPTE2084 = 8;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_ARIB_STD_B67 = 9;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_PRIMARIES_UNKNOWN = -1;



/*Using mat4 as glm does not have the 4N alignment*/

struct ct_read_data {
	ZUAZO_IF_CPP(glm::mat4, mat4) 		mtxRGB2XYZ; 
	ZUAZO_IF_CPP(glm::mat4, mat4) 		mtxYCbCr2RGB;
	ZUAZO_IF_CPP(int32_t, int) 			colorPrimaries;
	ZUAZO_IF_CPP(int32_t, int) 			colorTransferFunction;
	ZUAZO_IF_CPP(int32_t, int) 			colorModel;
	ZUAZO_IF_CPP(int32_t, int) 			colorRange;
	ZUAZO_IF_CPP(int32_t, int) 			planeFormat;
};

struct ct_write_data {
	ZUAZO_IF_CPP(glm::mat4, mat4) 		mtxXYZ2RGB;
	ZUAZO_IF_CPP(glm::mat4, mat4) 		mtxRGB2YCbCr;
	ZUAZO_IF_CPP(int32_t, int) 			colorPrimaries;
	ZUAZO_IF_CPP(int32_t, int) 			colorTransferFunction;
	ZUAZO_IF_CPP(int32_t, int) 			colorModel;
	ZUAZO_IF_CPP(int32_t, int) 			colorRange;
	ZUAZO_IF_CPP(int32_t, int) 			planeFormat;
};
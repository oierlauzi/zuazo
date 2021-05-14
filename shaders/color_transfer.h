/*
 * This header file will be included from GLSL shaders, so C++ content
 * is discriminaded by the macro definition __cplusplus
 */
#ifdef __cplusplus
	#pragma once
	#define ZUAZO_IF_CPP(x, y) x

	#include <zuazo/Math/Vector.h>
	#include <zuazo/Math/Matrix.h>

	#include <cstdint>

	using mat4 = Zuazo::Math::Mat4x4f;

#else
	#define ZUAZO_IF_CPP(x, y) y
#endif

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_PLANE_COUNT_ID = 0;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_PLANE_FORMAT_ID = 1;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_PLANE_FORMAT_RGBA = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_PLANE_FORMAT_G_BR = 1;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_PLANE_FORMAT_G_BR_A = 2;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_PLANE_FORMAT_G_B_R = 3;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_PLANE_FORMAT_G_B_R_A = 4;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_RANGE_ID = 2;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_RANGE_FULL_RGB = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_RANGE_FULL_YCBCR = 1;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_RANGE_ITU_NARROW_RGB = 2;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_RANGE_ITU_NARROW_YCBCR = 3;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_RANGE_ITU_NARROW_FULL_ALPHA_RGB = 4;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_RANGE_ITU_NARROW_FULL_ALPHA_YCBCR = 5;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_MODEL_ID = 3;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_MODEL_RGB = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_MODEL_YCBCR = 1;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_ID = 4;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_LINEAR = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_BT1886 = 1;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_GAMMA22 = 2;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_GAMMA26 = 3;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_GAMMA28 = 4;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1 = 5;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_4 = 6;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_SMPTE240M = 7;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_SMPTE2084 = 8;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_TRANSFER_FUNCTION_ARIB_STD_B67 = 9;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_COLOR_MODEL_MATRIX_BASE_ID = 5;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_MAT3x3_M00_OFFSET = 0;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_MAT3x3_M01_OFFSET = 1;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_MAT3x3_M02_OFFSET = 2;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_MAT3x3_M10_OFFSET = 3;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_MAT3x3_M11_OFFSET = 4;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_MAT3x3_M12_OFFSET = 5;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_MAT3x3_M20_OFFSET = 6;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_MAT3x3_M21_OFFSET = 7;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_MAT3x3_M22_OFFSET = 8;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_CHROMA_SAMPLE_OFFSET_X_ID = 14;
ZUAZO_IF_CPP(constexpr int32_t, const int) ct_CHROMA_SAMPLE_OFFSET_Y_ID = 15;

ZUAZO_IF_CPP(constexpr int32_t, const int) ct_SAMPLER_BINDING = 0;

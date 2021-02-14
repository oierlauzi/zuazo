/*
 * This header file will be included from GLSL shaders, so C++ content
 * is discriminaded by macro definition __cplusplus
 */
#ifdef __cplusplus
	#pragma once
	#define ZUAZO_IF_CPP(x, y) x

	#include <cstdint>

#else
	#define ZUAZO_IF_CPP(x, y) y
#endif

ZUAZO_IF_CPP(constexpr uint32_t, const uint) frame_SAMPLER_COUNT = 4;
ZUAZO_IF_CPP(constexpr uint32_t, const uint) frame_OUTPUT_COUNT = 4;

ZUAZO_IF_CPP(constexpr uint32_t, const uint) frame_SAMPLER_BINDING = 0;
ZUAZO_IF_CPP(constexpr uint32_t, const uint) frame_SAMPLE_MODE_BINDING = 1;
ZUAZO_IF_CPP(constexpr uint32_t, const uint) frame_COLOR_TRANSFER_BINDING = 2;
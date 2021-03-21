#include "frame.h"

#define frame_samplers(x) frame ## x ## _samplers
#define frame_sample_mode(x) frame ## x ## _sample_mode
#define frame_color_transfer(x) frame ## x ## _color_transfer

#define frame_descriptor_set(x) 																				\
	layout(set = x, binding = frame_SAMPLER_BINDING) uniform sampler2D frame_samplers(x)[frame_SAMPLER_COUNT];	\
	layout(set = x, binding = frame_SAMPLE_MODE_BINDING) uniform frame ## x ## _SampleModeBlock {				\
		int frame_sample_mode(x);																				\
	};																											\
	layout(set = x, binding = frame_COLOR_TRANSFER_BINDING) uniform frame ## x ## _InputColorTransferBlock {	\
		ct_read_data frame_color_transfer(x);																	\
	};

#define frame_texture(x, uv) \
	ct_texture(frame_sample_mode(x), frame_color_transfer(x), frame_samplers(x), uv)

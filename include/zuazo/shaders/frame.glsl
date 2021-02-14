#include "frame.h"

#define frame_samplers(x) frame_##x_samplers
#define frame_sample_mode(x) frame_##x_sample_mode
#define frame_color_transfer(x) frame_##x_color_transfer

#define frame_descriptor_set(x) 																				\
	layout(set = x, binding = frame_SAMPLER_BINDING) uniform sampler2D frame_samplers(x)[frame_SAMPLER_COUNT];	\
	layout(set = x, binding = frame_SAMPLE_MODE_BINDING) uniform frame_##x_SampleModeBlock {					\
		int frame_sample_mode(x);																				\
	};																											\
	layout(set = x, binding = frame_COLOR_TRANSFER_BINDING) uniform frame_##x_InputColorTransferBlock {			\
		ct_read_data frame_color_transfer(x);																	\
	};

#define frame_texture(x, uv) \
	ct_texture(frame_sample_mode(x), frame_color_transfer(x), frame_samplers(x), uv)

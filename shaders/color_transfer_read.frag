#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "color_transfer.h"
#include "color_transfer_constants.h"

//Specialization Constants
layout (constant_id = ct_PLANE_COUNT_ID) const int PLANE_COUNT = 4;
layout (constant_id = ct_PLANE_FORMAT_ID) const int PLANE_FORMAT = ct_PLANE_FORMAT_RGBA;
layout (constant_id = ct_COLOR_RANGE_ID) const int RANGE = ct_COLOR_RANGE_FULL_RGB;
layout (constant_id = ct_COLOR_MODEL_ID) const int MODEL = ct_COLOR_MODEL_RGB;
layout (constant_id = ct_COLOR_TRANSFER_FUNCTION_ID) const int TRANSFER_FUNCTION = ct_COLOR_TRANSFER_FUNCTION_LINEAR;

//Model conversion matrix (also specialization constant)
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + 0) const float modelMatrix00 = 1.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + 1) const float modelMatrix01 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + 2) const float modelMatrix02 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + 3) const float modelMatrix10 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + 4) const float modelMatrix11 = 1.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + 5) const float modelMatrix12 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + 6) const float modelMatrix20 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + 7) const float modelMatrix21 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + 8) const float modelMatrix22 = 1.0f;

//Vertex I/O
layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 out_color;

//Uniforms
layout(binding = ct_SAMPLER_BINDING) uniform sampler2D samplers[PLANE_COUNT];



/*
 * Performs a potentially multiplanar texture read
 */
vec4 load(in int planeFormat, in sampler2D images[PLANE_COUNT], in vec2 texCoords) {
	vec4 result;
	//TODO proper chroma reconstruction
	//TODO consider chroma size when interp.

	switch(planeFormat){
	case ct_PLANE_FORMAT_G_BR:
		result.g = texture(images[0], texCoords).r;
		result.br = texture(images[1], texCoords).rg;
		result.a = 1.0f;
		break;
	case ct_PLANE_FORMAT_G_BR_A:
		result.g = texture(images[0], texCoords).r;
		result.br = texture(images[1], texCoords).rg;
		result.a = texture(images[2], texCoords).r;
		break;
	case ct_PLANE_FORMAT_G_B_R:
		result.g = texture(images[0], texCoords).r;
		result.b = texture(images[1], texCoords).r;
		result.r = texture(images[2], texCoords).r;
		result.a = 1.0f;
		break;
	case ct_PLANE_FORMAT_G_B_R_A:
		result.g = texture(images[0], texCoords).r;
		result.b = texture(images[1], texCoords).r;
		result.r = texture(images[2], texCoords).r;
		result.a = texture(images[3], texCoords).r;
		break;
	
	default: //ct_PLANE_FORMAT_RGBA
		result = texture(images[0], texCoords);
		break;
	}

	return result;
}



/*
 * Expand performs range adjustments for cosidering any footroom / headroom at the coded values
 */
vec4 expand(in int range, in vec4 color){
	vec4 result;

	switch(range){
	case ct_COLOR_RANGE_FULL_YCBCR:
		result.ga = color.ga;
		result.rb = color.rb - vec2(0.5f);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_RGB:
		result = (1.0f/ITU_NARROW_SPAN_Y)*(color - vec4(ITU_NARROW_MIN_Y));
		break;
	case ct_COLOR_RANGE_ITU_NARROW_YCBCR:
		result.ga = (1.0f/ITU_NARROW_SPAN_Y)*(color.ga - vec2(ITU_NARROW_MIN_Y));
		result.rb = (1.0f/ITU_NARROW_SPAN_C)*(color.rb - vec2(ITU_NARROW_MIN_C)) - vec2(0.5f);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_FULL_ALPHA_RGB:
		result.rgb = (1.0f/ITU_NARROW_SPAN_Y)*(color.rgb - vec3(ITU_NARROW_MIN_Y));
		result.a = color.a;
		break;
	case ct_COLOR_RANGE_ITU_NARROW_FULL_ALPHA_YCBCR:
		result.g = (1.0f/ITU_NARROW_SPAN_Y)*(color.g - ITU_NARROW_MIN_Y);
		result.rb = (1.0f/ITU_NARROW_SPAN_C)*(color.rb - vec2(ITU_NARROW_MIN_C)) - vec2(0.5f);
		result.a = color.a;
		break;
	default: //ct_COLOR_RANGE_FULL_RGB
		result = color;
		break;
	}

	return result;
}



/*
 * These functions will be used to linearize the non-linear color components
 */
//Note: gain is expresed OETF-wise
vec3 gamma(in float gain, in float gamma, in vec3 color){
	return pow(color, vec3(gamma));
}

//Note: gains are expresed OETF-wise
vec3 hybrid_linear_gamma(in float gain, in float alpha, in float beta, in float gamma, in vec3 color) {
	//L = V/g							for 0 <= L <= beta*g
	//L = [V/alpha + (1-1/alpha)]^gamma	for beta*g < L <= 1

	//Sanitize the value. 0 <= L <= 1
	color = clamp(color, vec3(0.0f), vec3(1.0f));
	
	//Evaluate both parts of the expression (branchless)
	const vec3 piece0 = (1.0f/gain) * color;
	const vec3 piece1 = pow((1.0f/alpha) * color + (1.0f - 1.0f/alpha), vec3(gamma));

	//Select the maximum of both
	return mix(piece0, piece1, greaterThan(color, vec3(beta*gain)));
}

//Note: gains are expresed OETF-wise
vec3 hybrid_linear_gamma_EG(in float gain, in float alpha, in float beta, in float gamma, in vec3 color) {
	return sign(color)*hybrid_linear_gamma(gain, alpha, beta, gamma, abs(color));
}

vec3 BT1886(in vec3 color){
	return hybrid_linear_gamma(BT1886_GAIN, BT1886_ALPHA, BT1886_BETA, BT1886_GAMMA, color);
}

vec3 gamma22(in vec3 color){
	return gamma(1.0f, 2.2f, color);
}

vec3 gamma26(in vec3 color){
	return gamma(1.0f, 2.6f, color);
}

vec3 gamma28(in vec3 color){
	return gamma(1.0f, 2.8f, color);
}

vec3 IEC61966_2_1(in vec3 color){
	return hybrid_linear_gamma(IEC61966_2_1_GAIN, IEC61966_2_1_ALPHA, IEC61966_2_1_BETA, IEC61966_2_1_GAMMA, color);
}

vec3 IEC61966_2_4(in vec3 color){
	return hybrid_linear_gamma_EG(BT1886_GAIN, BT1886_ALPHA, BT1886_BETA, BT1886_GAMMA, color);
}

vec3 SMPTE240M(in vec3 color){
	return hybrid_linear_gamma(SMPTE240M_GAIN, SMPTE240M_ALPHA, SMPTE240M_BETA, SMPTE240M_GAMMA, color);
}

vec3 SMPTE2084(in vec3 color){
	//Compute the intermediary values
	const vec3 temp = pow(max(color, vec3(0.0f)), vec3(1.0f/SMPTE2084_M2));
	const vec3 num = temp - SMPTE2084_C1;
	const vec3 den = SMPTE2084_C2 - SMPTE2084_C3*temp;

	//Obtain the final result
	return pow(max(num/den, vec3(0.0f)), vec3(1.0f/SMPTE2084_M1));
}

vec3 ARIB_STD_B67(in vec3 color) {
	//L = (V^2) / 3								for 0 <= L <= 1/2
	//L = (exp((V-c)/a) + b) / 12				for 1/2 < L <= 1

	//Sanitize the value. 0 <= L <= 1
	color = clamp(color, vec3(0.0f), vec3(1.0f));
	
	//Evaluate both parts of the expression (branchless)
	const vec3 piece0 = (1.0f/3.0f)*color*color;
	const vec3 piece1 = (1.0f/12.0f)*(exp((color - ARIB_STD_B67_C) / ARIB_STD_B67_A) + ARIB_STD_B67_B);

	//Select the maximum of both
	return mix(piece0, piece1, greaterThan(color, vec3(0.5f)));
}

vec3 linearize(in int encoding, in vec3 color){
	vec3 result;

	switch(encoding){
	case ct_COLOR_TRANSFER_FUNCTION_BT1886:			result = BT1886(color);			break;
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA22:			result = gamma22(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA26:			result = gamma26(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA28:			result = gamma28(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1:		result = IEC61966_2_1(color);	break;
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_4:		result = IEC61966_2_4(color);	break;
	case ct_COLOR_TRANSFER_FUNCTION_SMPTE240M:		result = SMPTE240M(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_SMPTE2084:		result = SMPTE2084(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_ARIB_STD_B67:		result = ARIB_STD_B67(color);	break;
	default: /*ct_COLOR_TRANSFER_FUNCTION_LINEAR*/	result = color;					break;
	}

	return result;
}





void main() {
	//Sample the texture(s)
	out_color = load(PLANE_FORMAT, samplers, in_uv);

	//Expand the range
	out_color = expand(RANGE, out_color);

	//Convert it into RGB color model if necessary
	if(MODEL != ct_COLOR_MODEL_RGB) {
		const mat3 modelConversion = mat3(
			modelMatrix00,
			modelMatrix01,
			modelMatrix02,
			modelMatrix10,
			modelMatrix11,
			modelMatrix12,
			modelMatrix20,
			modelMatrix21,
			modelMatrix22
		);

		out_color.rgb = modelConversion * out_color.rgb; 
	}

	//Undo all gamma-like compressions
	out_color.rgb = linearize(TRANSFER_FUNCTION, out_color.rgb);
}
 

#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "color_transfer.h"
#include "color_transfer_constants.h"

//Constants
layout (constant_id = ct_PLANE_COUNT_ID) const int PLANE_COUNT = 4;
layout (constant_id = ct_PLANE_FORMAT_ID) const int PLANE_FORMAT = ct_PLANE_FORMAT_RGBA;
layout (constant_id = ct_COLOR_RANGE_ID) const int RANGE = ct_COLOR_RANGE_FULL_RGB;
layout (constant_id = ct_COLOR_MODEL_ID) const int MODEL = ct_COLOR_MODEL_RGB;
layout (constant_id = ct_COLOR_TRANSFER_FUNCTION_ID) const int TRANSFER_FUNCTION = ct_COLOR_TRANSFER_FUNCTION_LINEAR;

//Model conversion matrix (also specialization constant)
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M00_OFFSET) const float modelMatrix00 = 1.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M01_OFFSET) const float modelMatrix01 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M02_OFFSET) const float modelMatrix02 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M10_OFFSET) const float modelMatrix10 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M11_OFFSET) const float modelMatrix11 = 1.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M12_OFFSET) const float modelMatrix12 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M20_OFFSET) const float modelMatrix20 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M21_OFFSET) const float modelMatrix21 = 0.0f;
layout (constant_id = ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M22_OFFSET) const float modelMatrix22 = 1.0f;

//Vertex I/O
layout(location = 0) out vec4 out_color[PLANE_COUNT];

//Uniforms
layout (binding = ct_SAMPLER_BINDING, input_attachment_index = 0) uniform subpassInput in_color;



/*
 * Performs a potentially multiplanar texture write
 */
void store(in int planeFormat, in vec4 color, out vec4 planes[PLANE_COUNT]) {
	//Attach the alpha channel so that it gets correctly blended
	switch(planeFormat){
	case ct_PLANE_FORMAT_G_BR:
		planes[0].ra = color.ga;
		planes[1].rga = color.bra;
		break;
	case ct_PLANE_FORMAT_G_BR_A:
		planes[0].ra = color.ga;
		planes[1].rga = color.bra;
		planes[2].ra = color.aa;
		break;
	case ct_PLANE_FORMAT_G_B_R:
		planes[0].ra = color.ga;
		planes[1].ra = color.ba;
		planes[2].ra = color.ra;
		break;
	case ct_PLANE_FORMAT_G_B_R_A:
		planes[0].ra = color.ga;
		planes[1].ra = color.ba;
		planes[2].ra = color.ra;
		planes[3].ra = color.aa;
		break;
	
	default: //ct_PLANE_FORMAT_RGBA
		planes[0] = color;
		break;
	}
}



/*
 * Contract performs range adjustments for cosidering any footroom / headroom at the coded values
 */
vec4 contract(in int range, in vec4 color){
	vec4 result;

	switch(range){
	case ct_COLOR_RANGE_FULL_YCBCR:
		result.ga = color.ga;
		result.rb = color.rb + vec2(0.5f);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_RGB:
		result = color * ITU_NARROW_SPAN_Y + vec4(ITU_NARROW_MIN_Y);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_YCBCR:
		result.ga = color.ga * ITU_NARROW_SPAN_Y + vec2(ITU_NARROW_MIN_Y);
		result.rb = (color.rb + vec2(0.5f)) * ITU_NARROW_SPAN_C + vec2(ITU_NARROW_MIN_C);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_FULL_ALPHA_RGB:
		result.rgb = color.rgb * ITU_NARROW_SPAN_Y + vec3(ITU_NARROW_MIN_Y);
		result.a = color.a;
		break;
	case ct_COLOR_RANGE_ITU_NARROW_FULL_ALPHA_YCBCR:
		result.g = color.g * ITU_NARROW_SPAN_Y + ITU_NARROW_MIN_Y;
		result.rb = (color.rb + vec2(0.5f)) * ITU_NARROW_SPAN_C + vec2(ITU_NARROW_MIN_C);
		result.a = color.a;
		break;
	default: //ct_COLOR_RANGE_FULL_RGB
		result = color;
		break;
	}

	return result;
}



/*
 * These functions will be used to unlinarize the linear color components
 */
vec3 gamma(in float gain, in float gamma, in vec3 color){
	return pow(color, vec3(1.0f/gamma));
}

vec3 hybrid_linear_gamma(in float gain, in float alpha, in float beta, in float gamma, in vec3 color) {
	//Equation from:
	//https://en.wikipedia.org/wiki/Rec._2020
	//V = g*L							for 0 <= L <= beta
	//V = alpha*L^(1/gamma) + 1 - alpha	for beta < L <= 1

	//Sanitize the value. 0 <= L <= 1
	color = clamp(color, vec3(0.0f), vec3(1.0f));
	
	//Evaluate both parts of the expression (branchless)
	const vec3 piece0 = gain * color;
	const vec3 piece1 = alpha * pow(color, vec3(1.0f/gamma)) + (1 - alpha);

	//Select the minimum of both
	return mix(piece0, piece1, greaterThan(color, vec3(beta)));
}

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
	//Equations from:
	//https://en.wikipedia.org/wiki/High-dynamic-range_video

	//Compute the intermediary values
	const vec3 temp = pow(max(color, vec3(0.0f)), vec3(SMPTE2084_M1));
	const vec3 num = SMPTE2084_C1 	+ SMPTE2084_C2*temp;
	const vec3 den = vec3(1.0f)			+ SMPTE2084_C3*temp;

	//Obtain the final result
	return pow(num/den, vec3(SMPTE2084_M2));
}

vec3 ARIB_STD_B67(in vec3 color) {
	//Equation from:
	//https://en.wikipedia.org/wiki/Hybrid_Log-Gamma
	//V = sqrt(3*L)								for 0 <= L <= 1/12
	//V = a*ln(12*L - b) + c					for 1/12 < L <= 1

	//Sanitize the value. 0 <= L <= 1
	color = clamp(color, vec3(0.0f), vec3(1.0f));
	
	//Evaluate both parts of the expression (branchless)
	const vec3 piece0 = sqrt(3*color);
	const vec3 piece1 = ARIB_STD_B67_A*log(12*color - ARIB_STD_B67_B) + ARIB_STD_B67_C;

	//Select the minimum of both
	return mix(piece0, piece1, greaterThan(color, vec3(1.0/12.0f)));
}

vec3 unlinearize(in int encoding, in vec3 color){
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
	vec4 color = subpassLoad(in_color);

	//Apply a gamma-like compression
	color.rgb = unlinearize(TRANSFER_FUNCTION, color.rgb); 

	//Convert it into a YCbCr color model if necessary
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

		color.rgb = modelConversion * color.rgb; 
	}

	//Convert it into the corresponding range
	color = contract(RANGE, color);

	store(PLANE_FORMAT, color, out_color);
}

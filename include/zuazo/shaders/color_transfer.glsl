#include "color_transfer.h"

vec4 ct_sample(in int planeFormat, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords){
	vec4 result;

	switch(planeFormat){
	case ct_PLANE_FORMAT_G_BR:
		result.g = texture(images[0], texCoords).r;
		result.br = texture(images[1], texCoords).rg;
		result.a = 1.0f;
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



vec4 ct_expand(in int range, in vec4 color){
	vec4 result;

	const float MIN_Y = 16.0f / 255.0f;
	const float MAX_Y = 235.0f / 255.0f;
	const float MIN_C = 16.0f / 255.0f;
	const float MAX_C = 240.0f / 255.0f;

	switch(range){
	case ct_COLOR_RANGE_FULL_YCBCR:
		result.ga = color.ga;
		result.rb = color.rb - vec2(0.5f);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_RGB:
		result = (color - vec4(MIN_Y)) / (MAX_Y - MIN_Y);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_YCBCR:
		result.ga = (color.ga - vec2(MIN_Y)) / (MAX_Y - MIN_Y);
		result.rb = (color.rb - vec2(MIN_C)) / (MAX_C - MIN_C) - vec2(0.5f);
		break;
	default: //ct_COLOR_RANGE_FULL_RGB
		result = color;
		break;
	}

	return result;
}

vec4 ct_contract(in int range, in vec4 color){
	vec4 result;

	const float MIN_Y = 16.0f / 255.0f;
	const float MAX_Y = 235.0f / 255.0f;
	const float MIN_C = 16.0f / 255.0f;
	const float MAX_C = 240.0f / 255.0f;

	switch(range){
	case ct_COLOR_RANGE_FULL_YCBCR:
		result.ga = color.ga;
		result.rb = color.rb + vec2(0.5f);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_RGB:
		result = color * (MAX_Y - MIN_Y) + vec4(MIN_Y);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_YCBCR:
		result.ga = color.ga * (MAX_Y - MIN_Y) + vec2(MIN_Y);
		result.rb = (color.rb  + vec2(0.5f)) * (MAX_C - MIN_C) + vec2(MIN_C);
		break;
	default: //ct_COLOR_RANGE_FULL_RGB
		result = color;
		break;
	}

	return result;
}

/*
 * OETF: Opto-electrical transfer function (unlinearize)
 * EOTF: Electro-optical transfer function (linearize)
 * Linear value is usualy expressed as L in equations, whilst
 * the non-linear one as E or V
 */

//Note: gain is expresed OETF-wise
float ct_EOTF_linear(in float gain, in float value){
	return value / gain;
}

float ct_OETF_linear(in float gain, in float value){
	return value * gain;
}


//Note: gain is expresed OETF-wise
float ct_EOTF_gamma(in float gain, in float gamma, in float value){
	return pow(ct_EOTF_linear(gain, value), gamma);
}

float ct_OETF_gamma(in float gain, in float gamma, in float value){
	return ct_OETF_linear(gain, pow(value, 1.0f/gamma));
}


//Note: gain is expresed OETF-wise
float ct_EOTF_log(in float gain, in float b, in float value){
	return exp(ct_EOTF_linear(gain, value)) + b;
}

float ct_OETF_log(in float gain, in float b, in float value){
	return ct_OETF_linear(gain, log(value - b));
}


float ct_hybrid_linear_gamma_offset(in float thresh, in float gain1, in float gain2, in float gamma) {
	return ct_OETF_linear(gain1, thresh) - ct_OETF_gamma(gain2, gamma, thresh); //Usualy (1 - gain2)
}

//Note: gains are expresed OETF-wise
float ct_EOTF_hybrid_linear_gamma(in float thresh, in float gain1, in float gain2, in float gamma, in float value) {
	//Equations from:
	//https://en.wikipedia.org/wiki/Rec._2020
	if(value < 0.0f) {
		return 0.0f;
	} else if(value <= ct_OETF_linear(gain1, thresh)){
		return ct_EOTF_linear(gain1, value);
	} else {
		return ct_EOTF_gamma(gain2, gamma, value - ct_hybrid_linear_gamma_offset(thresh, gain1, gain2, gamma));
	}
}

float ct_OETF_hybrid_linear_gamma(in float thresh, in float gain1, in float gain2, in float gamma, in float value){
	if(value < 0.0f) {
		return 0.0f;
	} else if(value <= thresh){
		return ct_OETF_linear(gain1, value);
	} else {
		return ct_OETF_gamma(gain2, gamma, value) + ct_hybrid_linear_gamma_offset(thresh, gain1, gain2, gamma);
	}
}


//Note: gains are expresed OETF-wise
float ct_EOTF_hybrid_linear_gamma_EG(in float thresh, in float gain1, in float gain2, in float gamma, in float value) {
	const float b = ct_OETF_linear(gain1, thresh);

	if(value < -b) {
		return ct_EOTF_gamma(-gain2, gamma, -value - ct_hybrid_linear_gamma_offset(thresh, gain1, gain2, gamma));
	} else if(value <= b){
		return ct_EOTF_linear(gain1, value);
	} else {
		return ct_EOTF_gamma(gain2, gamma, value - ct_hybrid_linear_gamma_offset(thresh, gain1, gain2, gamma));
	}
}

float ct_OETF_hybrid_linear_gamma_EG(in float thresh, in float gain1, in float gain2, in float gamma, in float value){
	if(value < -thresh) {
		return ct_OETF_gamma(-gain2, gamma, -value) + ct_hybrid_linear_gamma_offset(thresh, gain1, gain2, gamma);
	} else if(value <= thresh){
		return ct_OETF_linear(gain1, value);
	} else {
		return ct_OETF_gamma(gain2, gamma, value) + ct_hybrid_linear_gamma_offset(thresh, gain1, gain2, gamma);
	}
}


//Note: gain is expresed OETF-wise
float ct_EOTF_PQ(in float c1, in float c2, in float m1, in float m2, in float value){
	const float c3 = c1 + c2 - 1;
	const float MIN = pow(c1, m2);

	const float value2 = pow(max(value, MIN), 1.0f/m2);
	const float num = max(value2 - c1, 0.0f);
	const float den = c2 - c3*value2;
	return pow(num/den, 1.0f/m1);
}

float ct_OETF_PQ(in float c1, in float c2, in float m1, in float m2, in float value){
	//Equations from:
	//https://en.wikipedia.org/wiki/High-dynamic-range_video

	const float c3 = c1 + c2 - 1;

	const float value2 = pow(max(value, 0.0f), m1);
	const float num = c1 	+ c2*value2;
	const float den = 1.0f 	+ c3*value2;
	return pow(num/den, m2);
}


//Note: gains are expresed OETF-wise
float ct_EOTF_hybrid_log_gamma(in float thresh, in float gain1, in float gamma, in float gain2, in float b, in float value) {
	if(value <= 0.0f) {
		return 0.0f;
	} else if(value < ct_OETF_gamma(gain1, gamma, value)){
		return ct_EOTF_gamma(gain1, gamma, value);
	} else {
		const float offset = ct_OETF_gamma(gain1, gamma, thresh) - ct_OETF_log(gain2, b, thresh);
		return ct_EOTF_log(gain2, b, value - offset);
	}
}

float ct_OETF_hybrid_log_gamma(in float thresh, in float gain1, in float gamma, in float gain2, in float b, in float value){
	//Equations from:
	//https://en.wikipedia.org/wiki/Hybrid_Log-Gamma

	if(value <= 0.0f) {
		return 0.0f;
	} else if(value < thresh){
		return ct_OETF_gamma(gain1, gamma, value);
	} else {
		const float offset = ct_OETF_gamma(gain1, gamma, thresh) - ct_OETF_log(gain2, b, thresh);
		return ct_OETF_log(gain2, b, value) + offset;
	}
}


float ct_EOTF_bt601_709_2020(in float value){
	return ct_EOTF_hybrid_linear_gamma(0.0181f, 4.5f, 1.0993f, 1.0f/0.45f, value);
}

float ct_OETF_bt601_709_2020(in float value){
	return ct_OETF_hybrid_linear_gamma(0.0181f, 4.5f, 1.0993f, 1.0f/0.45f, value);
}

vec4 ct_EOTF_bt601_709_2020(in vec4 color){
	return vec4(
		ct_EOTF_bt601_709_2020(color.r),
		ct_EOTF_bt601_709_2020(color.g),
		ct_EOTF_bt601_709_2020(color.b),
		color.a
	);
}

vec4 ct_OETF_bt601_709_2020(in vec4 color){
	return vec4(
		ct_OETF_bt601_709_2020(color.r),
		ct_OETF_bt601_709_2020(color.g),
		ct_OETF_bt601_709_2020(color.b),
		color.a
	);
}


float ct_EOTF_gamma22(in float value){
	return ct_EOTF_gamma(1.0f, 2.2f, value);
}

float ct_OETF_gamma22(in float value){
	return ct_OETF_gamma(1.0f, 2.2f, value);
}

vec4 ct_EOTF_gamma22(in vec4 color){
	return vec4(
		ct_EOTF_gamma22(color.r),
		ct_EOTF_gamma22(color.g),
		ct_EOTF_gamma22(color.b),
		color.a
	);
}

vec4 ct_OETF_gamma22(in vec4 color){
	return vec4(
		ct_OETF_gamma22(color.r),
		ct_OETF_gamma22(color.g),
		ct_OETF_gamma22(color.b),
		color.a
	);
}


float ct_EOTF_gamma26(in float value){
	return ct_EOTF_gamma(1.0f, 2.6f, value);
}

float ct_OETF_gamma26(in float value){
	return ct_OETF_gamma(1.0f, 2.6f, value);
}

vec4 ct_EOTF_gamma26(in vec4 color){
	return vec4(
		ct_EOTF_gamma26(color.r),
		ct_EOTF_gamma26(color.g),
		ct_EOTF_gamma26(color.b),
		color.a
	);
}

vec4 ct_OETF_gamma26(in vec4 color){
	return vec4(
		ct_OETF_gamma26(color.r),
		ct_OETF_gamma26(color.g),
		ct_OETF_gamma26(color.b),
		color.a
	);
}


float ct_EOTF_gamma28(in float value){
	return ct_EOTF_gamma(1.0f, 2.8f, value);
}

float ct_OETF_gamma28(in float value){
	return ct_OETF_gamma(1.0f, 2.8f, value);
}

vec4 ct_EOTF_gamma28(in vec4 color){
	return vec4(
		ct_EOTF_gamma28(color.r),
		ct_EOTF_gamma28(color.g),
		ct_EOTF_gamma28(color.b),
		color.a
	);
}

vec4 ct_OETF_gamma28(in vec4 color){
	return vec4(
		ct_OETF_gamma28(color.r),
		ct_OETF_gamma28(color.g),
		ct_OETF_gamma28(color.b),
		color.a
	);
}


float ct_EOTF_IEC61966_2_1(in float value){
	return ct_EOTF_hybrid_linear_gamma(0.0031308f, 12.92f, 1.055f, 12.0f/5.0f, value);
}

float ct_OETF_IEC61966_2_1(in float value){
	return ct_OETF_hybrid_linear_gamma(0.0031308f, 12.92f, 1.055f, 12.0f/5.0f, value);
}

vec4 ct_EOTF_IEC61966_2_1(in vec4 color){
	return vec4(
		ct_EOTF_IEC61966_2_1(color.r),
		ct_EOTF_IEC61966_2_1(color.g),
		ct_EOTF_IEC61966_2_1(color.b),
		color.a
	);
}

vec4 ct_OETF_IEC61966_2_1(in vec4 color){
	return vec4(
		ct_OETF_IEC61966_2_1(color.r),
		ct_OETF_IEC61966_2_1(color.g),
		ct_OETF_IEC61966_2_1(color.b),
		color.a
	);
}


float ct_EOTF_IEC61966_2_4(in float value){
	return ct_EOTF_hybrid_linear_gamma_EG(0.0181f, 4.5f, 1.0993f, 1.0f/0.45f, value);
}

float ct_OETF_IEC61966_2_4(in float value){
	return ct_OETF_hybrid_linear_gamma_EG(0.0181f, 4.5f, 1.0993f, 1.0f/0.45f, value);
}

vec4 ct_EOTF_IEC61966_2_4(in vec4 color){
	return vec4(
		ct_EOTF_IEC61966_2_4(color.r),
		ct_EOTF_IEC61966_2_4(color.g),
		ct_EOTF_IEC61966_2_4(color.b),
		color.a
	);
}

vec4 ct_OETF_IEC61966_2_4(in vec4 color){
	return vec4(
		ct_OETF_IEC61966_2_4(color.r),
		ct_OETF_IEC61966_2_4(color.g),
		ct_OETF_IEC61966_2_4(color.b),
		color.a
	);
}


float ct_EOTF_SMPTE240M(in float value){
	return ct_EOTF_hybrid_linear_gamma(0.0031308f, 12.92f, 1.055f, 1.0f/0.45f, value);
}

float ct_OETF_SMPTE240M(in float value){
	return ct_OETF_hybrid_linear_gamma(0.0031308f, 12.92f, 1.055f, 1.0f/0.45f, value);
}

vec4 ct_EOTF_SMPTE240M(in vec4 color){
	return vec4(
		ct_EOTF_SMPTE240M(color.r),
		ct_EOTF_SMPTE240M(color.g),
		ct_EOTF_SMPTE240M(color.b),
		color.a
	);
}

vec4 ct_OETF_SMPTE240M(in vec4 color){
	return vec4(
		ct_OETF_SMPTE240M(color.r),
		ct_OETF_SMPTE240M(color.g),
		ct_OETF_SMPTE240M(color.b),
		color.a
	);
}


float ct_EOTF_SMPTE2084(in float value){
	return ct_EOTF_PQ(3424.0f/4096.0f, 32.0f*2413.0f/4096.0f, 0.25f*2610.0f/4096.0f, 128.0f*2523.0f/4096.0f, value);
}

float ct_OETF_SMPTE2084(in float value){
	return ct_OETF_PQ(3424.0f/4096.0f, 32.0f*2413.0f/4096.0f, 0.25f*2610.0f/4096.0f, 128.0f*2523.0f/4096.0f, value);
}

vec4 ct_EOTF_SMPTE2084(in vec4 color){
	return vec4(
		ct_EOTF_SMPTE2084(color.r),
		ct_EOTF_SMPTE2084(color.g),
		ct_EOTF_SMPTE2084(color.b),
		color.a
	);
}

vec4 ct_OETF_SMPTE2084(in vec4 color){
	return vec4(
		ct_OETF_SMPTE2084(color.r),
		ct_OETF_SMPTE2084(color.g),
		ct_OETF_SMPTE2084(color.b),
		color.a
	);
}


float ct_EOTF_ARIB_STD_B67(in float value){
	//ARIB STD-B67 is expressed for values between 0-12, thats why the result gets divided by 12
	return ct_EOTF_hybrid_log_gamma(1.0f, 0.5f, 1.0f/0.5f, 0.17883277f, 0.28466892f, value) / 12.0f;
}

float ct_OETF_ARIB_STD_B67(in float value){
	//ARIB STD-B67 is expressed for values between 0-12, thats why the value gets multiplied by 12
	return ct_OETF_hybrid_log_gamma(1.0f, 0.5f, 1.0f/0.5f, 0.17883277f, 0.28466892f, value * 12.0f);
}

vec4 ct_EOTF_ARIB_STD_B67(in vec4 color){
	return vec4(
		ct_EOTF_ARIB_STD_B67(color.r),
		ct_EOTF_ARIB_STD_B67(color.g),
		ct_EOTF_ARIB_STD_B67(color.b),
		color.a
	);
}

vec4 ct_OETF_ARIB_STD_B67(in vec4 color){
	return vec4(
		ct_OETF_ARIB_STD_B67(color.r),
		ct_OETF_ARIB_STD_B67(color.g),
		ct_OETF_ARIB_STD_B67(color.b),
		color.a
	);
}


vec4 ct_EOTF(in int encoding, in vec4 color){
	switch(encoding){
	case ct_COLOR_TRANSFER_FUNCTION_BT601_709_2020:	return ct_EOTF_bt601_709_2020(color);
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA22:		return ct_EOTF_gamma22(color);
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA26:		return ct_EOTF_gamma26(color);
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA28:		return ct_EOTF_gamma28(color);
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1:	return ct_EOTF_IEC61966_2_1(color);
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_4:	return ct_EOTF_IEC61966_2_4(color);
	case ct_COLOR_TRANSFER_FUNCTION_SMPTE240M:		return ct_EOTF_SMPTE240M(color);
	case ct_COLOR_TRANSFER_FUNCTION_SMPTE2084:		return ct_EOTF_SMPTE2084(color);
	case ct_COLOR_TRANSFER_FUNCTION_ARIB_STD_B67:	return ct_EOTF_ARIB_STD_B67(color);
	default: /*ct_COLOR_TRANSFER_FUNCTION_LINEAR*/	return color;
	}
}

vec4 ct_OETF(in int encoding, in vec4 color){
	switch(encoding){
	case ct_COLOR_TRANSFER_FUNCTION_BT601_709_2020:	return ct_OETF_bt601_709_2020(color);
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA22:		return ct_OETF_gamma22(color);
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA26:		return ct_OETF_gamma26(color);
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA28:		return ct_OETF_gamma28(color);
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1:	return ct_OETF_IEC61966_2_1(color);
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_4:	return ct_OETF_IEC61966_2_4(color);
	case ct_COLOR_TRANSFER_FUNCTION_SMPTE240M:		return ct_OETF_SMPTE240M(color);
	case ct_COLOR_TRANSFER_FUNCTION_SMPTE2084:		return ct_OETF_SMPTE2084(color);
	case ct_COLOR_TRANSFER_FUNCTION_ARIB_STD_B67:	return ct_OETF_ARIB_STD_B67(color);
	default: /*ct_COLOR_TRANSFER_FUNCTION_LINEAR*/	return color;
	}
}



vec4 ct_readColor(in ct_read_data inputProp, in ct_write_data outputProp, in vec4 color){
	vec4 result = color;

	result = ct_expand(inputProp.colorRange, result); //Normalize the values into [0.0, 1.0] (or [-0.5, 0.5] for chroma)
	result = inputProp.mtxYCbCr2RGB * result; //Convert it into RGB color model
	result = ct_EOTF(inputProp.colorTransferFunction, result); //Undo all gamma-like compressions
	result = (outputProp.mtxXYZ2RGB * inputProp.mtxRGB2XYZ) * result; //Convert it into the destination color primaries

	return result;
}

vec4 ct_writeColor(in ct_write_data outputProp, in vec4 color){
	vec4 result = color;

	result = ct_OETF(outputProp.colorTransferFunction, result); //Apply a gamma-like compression
	result = outputProp.mtxRGB2YCbCr * result; //Convert it into the destination color model
	result = ct_contract(outputProp.colorRange, result); //Convert it into the corresponding range

	return result;
}

vec4 ct_transferColor(in ct_read_data inputProp, in ct_write_data outputProp, in vec4 color){
	vec4 result = ct_readColor(inputProp, outputProp, color);
	result = ct_writeColor(outputProp, result);
	return result;
}

vec4 ct_getColor(in ct_read_data inputProp, in ct_write_data outputProp, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords){
	vec4 result = ct_sample(inputProp.planeFormat, images, texCoords);
	result = ct_readColor(inputProp, outputProp, result);
	return result;
}


vec4 ct_readColorYCbCr(in ct_read_data inputProp, in ct_write_data outputProp, in vec4 color){
	vec4 result = color;

	result = ct_expand(inputProp.colorRange, result); //Normalize the values into [0.0, 1.0] (or [-0.5, 0.5] for chroma)
	result = inputProp.mtxYCbCr2RGB * result; //Convert it into RGB color model
	result = ct_EOTF(inputProp.colorTransferFunction, result); //Undo all gamma-like compressions
	result = (outputProp.mtxXYZ2RGB * inputProp.mtxRGB2XYZ) * result; //Convert it into the destination color primaries
	result = ct_OETF(outputProp.colorTransferFunction, result); //Apply a gamma-like compression
	result = outputProp.mtxRGB2YCbCr * result; //Convert it into the destination color model

	return result;
}

vec4 ct_writeColorYCbCr(in ct_write_data outputProp, in vec4 color){
	vec4 result = color;

	result = ct_contract(outputProp.colorRange, result); //Convert it into the corresponding range

	return result;
}

vec4 ct_getColorYCbCr(in ct_read_data inputProp, in ct_write_data outputProp, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords){
	vec4 result = ct_sample(inputProp.planeFormat, images, texCoords);
	result = ct_readColorYCbCr(inputProp, outputProp, result);
	return result;
}
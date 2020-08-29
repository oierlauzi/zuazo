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
	default: //ct_PLANE_FORMAT_RGBA
		result = texture(images[0], texCoords);
		break;
	}

	return result;
}

vec4 ct_make_cbcr_negative(in int isYCbCr, in vec4 color){
	vec4 result = color;

	if(isYCbCr == ct_YCBCR_TRUE) {
		color.rb -= vec2(0.5f); //Values between -0.5 and 0.5
	}

	return result;
}

vec4 ct_make_cbcr_positive(in int isYCbCr, in vec4 color){
	vec4 result = color;

	if(isYCbCr == ct_YCBCR_TRUE) {
		color.rb += vec2(0.5f); //Values between -0.5 and 0.5
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
	case ct_COLOR_RANGE_ITU_NARROW_RGB:
		result = (color - vec4(MIN_Y)) / (MAX_Y - MIN_Y);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_YCBCR:
		result.ga = (color.ga - vec2(MIN_Y)) / (MAX_Y - MIN_Y);
		result.rb = (color.rb - vec2(MIN_C)) / (MAX_C - MIN_C);
		break;
	default: //ct_COLOR_RANGE_FULL
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
	case ct_COLOR_RANGE_ITU_NARROW_RGB:
		result = color * (MAX_Y - MIN_Y) + vec4(MIN_Y);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_YCBCR:
		result.ga = color.ga * (MAX_Y - MIN_Y) + vec2(MIN_Y);
		result.rb = color.rb * (MAX_C - MIN_C) + vec2(MIN_C);
		break;
	default: //ct_COLOR_RANGE_FULL
		result = color;
		break;
	}

	return result;
}

float ct_unlinearize_IEC61966_2_1(in float value){
	if(value < 0.0031308f){
		return 12.92f * value;
	} else {
		return 1.055f * pow(value, 5.0f / 12.0f) - 0.055f;
	}
}

float ct_linearize_IEC61966_2_1(in float value){
	if(value < 0.04045f){
		return value / 12.92f;
	} else {
		return pow((value + 0.055f) / 1.055f, 12.0f / 5.0f);
	}
}

vec4 ct_unlinearize(in int encoding, in vec4 color){
	vec4 result;

	switch(encoding){
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1:
		result.r = ct_unlinearize_IEC61966_2_1(color.r);
		result.g = ct_unlinearize_IEC61966_2_1(color.g);
		result.b = ct_unlinearize_IEC61966_2_1(color.b);
		result.a = color.a;
		break;
	default: //ct_COLOR_TRANSFER_FUNCTION_LINEAR
		result = color;
		break;
	}

	return result;
}

vec4 ct_linearize(in int encoding, in vec4 color){
	vec4 result;

	switch(encoding){
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1:
		result.r = ct_linearize_IEC61966_2_1(color.r);
		result.g = ct_linearize_IEC61966_2_1(color.g);
		result.b = ct_linearize_IEC61966_2_1(color.b);
		result.a = color.a;
		break;
	default: //ct_COLOR_TRANSFER_FUNCTION_LINEAR
		result = color;
		break;
	}

	return result;
}

vec4 ct_readColor(in ct_data inputProp, in ct_data outputProp, in vec4 color){
	vec4 result = color;

	result = ct_expand(inputProp.colorRange, result); //Normalize the values into [0.0, 1.0]
	result = ct_make_cbcr_negative(inputProp.isYCbCr, result);
	result = inverse(inputProp.colorModel) * result; //Convert it into RGB color model
	result = ct_linearize(inputProp.colorTransferFunction, result); //Undo all gamma-like compressions
	result = outputProp.colorPrimaries * inverse(inputProp.colorPrimaries) * result; //Convert the destination's color primaries

	return result;
}

vec4 ct_writeColor(in ct_data outputProp, in vec4 color){
	vec4 result = color;

	result = ct_unlinearize(outputProp.colorTransferFunction, result); //Apply a gamma-like compression
	result = outputProp.colorModel * result; //Convert it into the destination color model
	result = ct_make_cbcr_positive(outputProp.isYCbCr, result);
	result = ct_contract(outputProp.colorRange, result); //Convert it into the corresponding range

	return result;
}

vec4 ct_transferColor(in ct_data inputProp, in ct_data outputProp, in vec4 color){
	vec4 result = ct_readColor(inputProp, outputProp, color);
	result = ct_writeColor(outputProp, result);
	return result;
}

vec4 ct_getColor(in ct_data inputProp, in ct_data outputProp, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords){
	vec4 result = ct_sample(inputProp.planeFormat, images, texCoords);
	result = ct_readColor(inputProp, outputProp, result);
	return result;
}
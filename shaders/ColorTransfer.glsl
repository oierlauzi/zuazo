#include "../include/Graphics/ColorTransfer.h"

vec4 ct_sample(in int planeFormat, in sampler2D samplers[SAMPLER_COUNT], in vec2 texCoords){
	vec4 result;

	switch(planeFormat){
	case PLANAR_FORMAT_G_BR:
		result.r = texture(samplers[0], texCoords).r;
		result.br = texture(samplers[1], texCoords).rg;
		result.a = 1.0f;
		break;
	case PLANAR_FORMAT_G_B_R:
		result.g = texture(samplers[0], texCoords).r;
		result.b = texture(samplers[1], texCoords).r;
		result.r = texture(samplers[2], texCoords).r;
		result.a = 1.0f;
		break;
	default: //PLANAR_FORMAT_RGBA
		result = texture(samplers[0], texCoords);
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
	case COLOR_RANGE_FULL_YCBCR:
		result.ra = color.ra;
		result.gb = color.gb + vec2(0.5f);
		break;
	case COLOR_RANGE_ITU_NARROW_RGB:
		result = color * (MAX_Y - MIN_Y) + vec4(MIN_Y);
		break;
	case COLOR_RANGE_ITU_NARROW_YCBCR:
		result.ra = color.ra * (MAX_Y - MIN_Y) + vec2(MIN_Y);
		result.gb = (color.gb + vec2(0.5f)) * (MAX_C - MIN_C) + vec2(MIN_Y);
		break;
	default: //COLOR_RANGE_FULL_RGB
		result = color;
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
	case COLOR_RANGE_FULL_YCBCR:
		result.ra = color.ra;
		result.gb = color.gb - vec2(0.5f);
		break;
	case COLOR_RANGE_ITU_NARROW_RGB:
		result = (color - vec4(MIN_Y)) / (MAX_Y - MIN_Y);
		break;
	case COLOR_RANGE_ITU_NARROW_YCBCR:
		result.ra = (color.ra - vec2(MIN_Y)) / (MAX_Y - MIN_Y);
		result.gb = (color.gb - vec2(MIN_C)) / (MAX_C - MIN_C) - vec2(0.5f);
		break;
	default: //COLOR_RANGE_FULL_RGB
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
	case COLOR_TRANSFER_FUNCTION_IEC61966_2_1:
		result.r = ct_unlinearize_IEC61966_2_1(color.r);
		result.g = ct_unlinearize_IEC61966_2_1(color.g);
		result.b = ct_unlinearize_IEC61966_2_1(color.b);
		result.a = color.a;
		break;
	default: //COLOR_TRANSFER_FUNCTION_LINEAR
		result = color;
		break;
	}

	return result;
}

vec4 ct_linearize(in int encoding, in vec4 color){
	vec4 result;

	switch(encoding){
	case COLOR_TRANSFER_FUNCTION_IEC61966_2_1:
		result.r = ct_linearize_IEC61966_2_1(color.r);
		result.g = ct_linearize_IEC61966_2_1(color.g);
		result.b = ct_linearize_IEC61966_2_1(color.b);
		result.a = color.a;
		break;
	default: //COLOR_TRANSFER_FUNCTION_LINEAR
		result = color;
		break;
	}

	return result;
}

vec4 ct_transferColor(in ColorTransfer inputProp, in ColorTransfer outputProp, in vec4 inColor){
	vec4 result = inColor;

	result = ct_expand(inputProp.colorRange, result);
	result = inputProp.colorSpace * result;
	result = ct_linearize(inputProp.colorTransferFunction, result);
	result = ct_unlinearize(outputProp.colorTransferFunction, result);
	result = outputProp.colorSpace * result;
	result = ct_contract(outputProp.colorRange, result);

	return result;
}

vec4 ct_getColor(in ColorTransfer inputProp, in ColorTransfer outputProp, in sampler2D samplers[SAMPLER_COUNT], in vec2 texCoords){
	return ct_transferColor(inputProp, outputProp, ct_sample(inputProp.planarFormat, samplers, texCoords));
}
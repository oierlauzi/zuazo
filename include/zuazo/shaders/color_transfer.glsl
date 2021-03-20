#include "color_transfer.h"

/*
 * Load / Store performs image read/writes in a planar maner
 */

vec4 ct_load(in int planeFormat, in vec4 plane0, in vec4 plane1, in vec4 plane2, in vec4 plane3) {
	vec4 result;

	switch(planeFormat){
	case ct_PLANE_FORMAT_G_BR:
		result.g = plane0.r;
		result.br = plane1.rg;
		result.a = 1.0f;
		break;
	case ct_PLANE_FORMAT_G_B_R:
		result.g = plane0.r;
		result.b = plane1.r;
		result.r = plane2.r;
		result.a = 1.0f;
		break;
	case ct_PLANE_FORMAT_G_B_R_A:
		result.g = plane0.r;
		result.b = plane1.r;
		result.r = plane2.r;
		result.a = plane3.r;
		break;
	
	default: //ct_PLANE_FORMAT_RGBA
		result = plane0;
		break;
	}

	return result;
}

vec4 ct_load(in int planeFormat, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords) {
	vec4 result;
	//TODO proper chroma reconstruction
	//TODO consider chroma size when interp.

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

void ct_store(in int planeFormat, in vec4 color, out vec4 plane0, out vec4 plane1, out vec4 plane2, out vec4 plane3) {
	//Attach the alpha channel so that it gets correctly blended
	switch(planeFormat){
	case ct_PLANE_FORMAT_G_BR:
		plane0.ra = color.ga;
		plane1.rga = color.bra;
		break;
	case ct_PLANE_FORMAT_G_B_R:
		plane0.ra = color.ga;
		plane1.ra = color.ba;
		plane2.ra = color.ra;
		break;
	case ct_PLANE_FORMAT_G_B_R_A:
		plane0.ra = color.ga;
		plane1.ra = color.ba;
		plane2.ra = color.ra;
		plane3.ra = color.aa;
		break;
	
	default: //ct_PLANE_FORMAT_RGBA
		plane0 = color;
		break;
	}
}



/*
 * Expand / Contract performs range adjustments for cosidering any footroom / headroom at the coded values
 */

//Constants specified for a hypothetic 16bit system. Unaccurate for higher precisions
const float ct_ITU_NARROW_MIN_Y = 16.0f*256.0f / 65535.0f;
const float ct_ITU_NARROW_MAX_Y = 235.0f*256.0f / 65535.0f;
const float ct_ITU_NARROW_MIN_C = 16.0f*256.0f / 65535.0f;
const float ct_ITU_NARROW_MAX_C = 240.0f*256.0f / 65535.0f;

vec4 ct_expand(in int range, in vec4 color){
	vec4 result;

	switch(range){
	case ct_COLOR_RANGE_FULL_YCBCR:
		result.ga = color.ga;
		result.rb = color.rb - vec2(0.5f);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_RGB:
		result = (color - vec4(ct_ITU_NARROW_MIN_Y)) / (ct_ITU_NARROW_MAX_Y - ct_ITU_NARROW_MIN_Y);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_YCBCR:
		result.ga = (color.ga - vec2(ct_ITU_NARROW_MIN_Y)) / (ct_ITU_NARROW_MAX_Y - ct_ITU_NARROW_MIN_Y);
		result.rb = (color.rb - vec2(ct_ITU_NARROW_MIN_C)) / (ct_ITU_NARROW_MAX_C - ct_ITU_NARROW_MIN_C) - vec2(0.5f);
		break;
	default: //ct_COLOR_RANGE_FULL_RGB
		result = color;
		break;
	}

	result.a = clamp(result.a, 0.0f, 1.0f); //Alpha might have been set to the default value, which lead to out of range results
	return result;
}

vec4 ct_contract(in int range, in vec4 color){
	vec4 result;

	switch(range){
	case ct_COLOR_RANGE_FULL_YCBCR:
		result.ga = color.ga;
		result.rb = color.rb + vec2(0.5f);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_RGB:
		result = color * (ct_ITU_NARROW_MAX_Y - ct_ITU_NARROW_MIN_Y) + vec4(ct_ITU_NARROW_MIN_Y);
		break;
	case ct_COLOR_RANGE_ITU_NARROW_YCBCR:
		result.ga = color.ga * (ct_ITU_NARROW_MAX_Y - ct_ITU_NARROW_MIN_Y) + vec2(ct_ITU_NARROW_MIN_Y);
		result.rb = (color.rb + vec2(0.5f)) * (ct_ITU_NARROW_MAX_C - ct_ITU_NARROW_MIN_C) + vec2(ct_ITU_NARROW_MIN_C);
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


float ct_hybrid_linear_gamma_offset(in float gain2) {
	//return ct_OETF_linear(gain1, thresh) - ct_OETF_gamma(gain2, gamma, thresh); //Usualy (1 - gain2)
	return 1.0f - gain2;
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
		return ct_EOTF_gamma(gain2, gamma, value - ct_hybrid_linear_gamma_offset(gain2));
	}
}

float ct_OETF_hybrid_linear_gamma(in float thresh, in float gain1, in float gain2, in float gamma, in float value){
	if(value < 0.0f) {
		return 0.0f;
	} else if(value <= thresh){
		return ct_OETF_linear(gain1, value);
	} else {
		return ct_OETF_gamma(gain2, gamma, value) + ct_hybrid_linear_gamma_offset(gain2);
	}
}


//Note: gains are expresed OETF-wise
float ct_EOTF_hybrid_linear_gamma_EG(in float thresh, in float gain1, in float gain2, in float gamma, in float value) {
	const float b = ct_OETF_linear(gain1, thresh);

	if(value < -b) {
		return ct_EOTF_gamma(-gain2, gamma, -value + ct_hybrid_linear_gamma_offset(gain2));
	} else if(value <= b){
		return ct_EOTF_linear(gain1, value);
	} else {
		return ct_EOTF_gamma(gain2, gamma, value - ct_hybrid_linear_gamma_offset(gain2));
	}
}

float ct_OETF_hybrid_linear_gamma_EG(in float thresh, in float gain1, in float gain2, in float gamma, in float value){
	if(value < -thresh) {
		return ct_OETF_gamma(-gain2, gamma, -value) - ct_hybrid_linear_gamma_offset(gain2);
	} else if(value <= thresh){
		return ct_OETF_linear(gain1, value);
	} else {
		return ct_OETF_gamma(gain2, gamma, value) + ct_hybrid_linear_gamma_offset(gain2);
	}
}


//Note: gain is expresed OETF-wise
float ct_EOTF_PQ(in float c1, in float c2, in float m1, in float m2, in float value){
	const float c3 = c1 + c2 - 1;

	const float value2 = pow(max(value, 0.0f), 1.0f/m2); //max is placed in order to avoid negative roots
	const float num = value2 - c1;
	const float den = c2 - c3*value2;
	return pow(max(num/den, 0.0f), 1.0f/m1);
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

vec3 ct_EOTF_bt601_709_2020(in vec3 color){
	return vec3(
		ct_EOTF_bt601_709_2020(color.r),
		ct_EOTF_bt601_709_2020(color.g),
		ct_EOTF_bt601_709_2020(color.b)
	);
}

vec3 ct_OETF_bt601_709_2020(in vec3 color){
	return vec3(
		ct_OETF_bt601_709_2020(color.r),
		ct_OETF_bt601_709_2020(color.g),
		ct_OETF_bt601_709_2020(color.b)
	);
}


float ct_EOTF_gamma22(in float value){
	return ct_EOTF_gamma(1.0f, 2.2f, value);
}

float ct_OETF_gamma22(in float value){
	return ct_OETF_gamma(1.0f, 2.2f, value);
}

vec3 ct_EOTF_gamma22(in vec3 color){
	return vec3(
		ct_EOTF_gamma22(color.r),
		ct_EOTF_gamma22(color.g),
		ct_EOTF_gamma22(color.b)
	);
}

vec3 ct_OETF_gamma22(in vec3 color){
	return vec3(
		ct_OETF_gamma22(color.r),
		ct_OETF_gamma22(color.g),
		ct_OETF_gamma22(color.b)
	);
}


float ct_EOTF_gamma26(in float value){
	return ct_EOTF_gamma(1.0f, 2.6f, value);
}

float ct_OETF_gamma26(in float value){
	return ct_OETF_gamma(1.0f, 2.6f, value);
}

vec3 ct_EOTF_gamma26(in vec3 color){
	return vec3(
		ct_EOTF_gamma26(color.r),
		ct_EOTF_gamma26(color.g),
		ct_EOTF_gamma26(color.b)
	);
}

vec3 ct_OETF_gamma26(in vec3 color){
	return vec3(
		ct_OETF_gamma26(color.r),
		ct_OETF_gamma26(color.g),
		ct_OETF_gamma26(color.b)
	);
}


float ct_EOTF_gamma28(in float value){
	return ct_EOTF_gamma(1.0f, 2.8f, value);
}

float ct_OETF_gamma28(in float value){
	return ct_OETF_gamma(1.0f, 2.8f, value);
}

vec3 ct_EOTF_gamma28(in vec3 color){
	return vec3(
		ct_EOTF_gamma28(color.r),
		ct_EOTF_gamma28(color.g),
		ct_EOTF_gamma28(color.b)
	);
}

vec3 ct_OETF_gamma28(in vec3 color){
	return vec3(
		ct_OETF_gamma28(color.r),
		ct_OETF_gamma28(color.g),
		ct_OETF_gamma28(color.b)
	);
}


float ct_EOTF_IEC61966_2_1(in float value){
	return ct_EOTF_hybrid_linear_gamma(0.0031308f, 12.92f, 1.055f, 12.0f/5.0f, value);
}

float ct_OETF_IEC61966_2_1(in float value){
	return ct_OETF_hybrid_linear_gamma(0.0031308f, 12.92f, 1.055f, 12.0f/5.0f, value);
}

vec3 ct_EOTF_IEC61966_2_1(in vec3 color){
	return vec3(
		ct_EOTF_IEC61966_2_1(color.r),
		ct_EOTF_IEC61966_2_1(color.g),
		ct_EOTF_IEC61966_2_1(color.b)
	);
}

vec3 ct_OETF_IEC61966_2_1(in vec3 color){
	return vec3(
		ct_OETF_IEC61966_2_1(color.r),
		ct_OETF_IEC61966_2_1(color.g),
		ct_OETF_IEC61966_2_1(color.b)
	);
}


float ct_EOTF_IEC61966_2_4(in float value){
	return ct_EOTF_hybrid_linear_gamma_EG(0.0181f, 4.5f, 1.0993f, 1.0f/0.45f, value);
}

float ct_OETF_IEC61966_2_4(in float value){
	return ct_OETF_hybrid_linear_gamma_EG(0.0181f, 4.5f, 1.0993f, 1.0f/0.45f, value);
}

vec3 ct_EOTF_IEC61966_2_4(in vec3 color){
	return vec3(
		ct_EOTF_IEC61966_2_4(color.r),
		ct_EOTF_IEC61966_2_4(color.g),
		ct_EOTF_IEC61966_2_4(color.b)
	);
}

vec3 ct_OETF_IEC61966_2_4(in vec3 color){
	return vec3(
		ct_OETF_IEC61966_2_4(color.r),
		ct_OETF_IEC61966_2_4(color.g),
		ct_OETF_IEC61966_2_4(color.b)
	);
}


float ct_EOTF_SMPTE240M(in float value){
	return ct_EOTF_hybrid_linear_gamma(0.0031308f, 12.92f, 1.055f, 1.0f/0.45f, value);
}

float ct_OETF_SMPTE240M(in float value){
	return ct_OETF_hybrid_linear_gamma(0.0031308f, 12.92f, 1.055f, 1.0f/0.45f, value);
}

vec3 ct_EOTF_SMPTE240M(in vec3 color){
	return vec3(
		ct_EOTF_SMPTE240M(color.r),
		ct_EOTF_SMPTE240M(color.g),
		ct_EOTF_SMPTE240M(color.b)
	);
}

vec3 ct_OETF_SMPTE240M(in vec3 color){
	return vec3(
		ct_OETF_SMPTE240M(color.r),
		ct_OETF_SMPTE240M(color.g),
		ct_OETF_SMPTE240M(color.b)
	);
}


float ct_EOTF_SMPTE2084(in float value){
	return ct_EOTF_PQ(107.0f/128.0f, 2413.0f/128.0f, 1305.0f/8192.0f, 2523.0f/32.0f, value);
}

float ct_OETF_SMPTE2084(in float value){
	return ct_OETF_PQ(107.0f/128.0f, 2413.0f/128.0f, 1305.0f/8192.0f, 2523.0f/32.0f, value);
}

vec3 ct_EOTF_SMPTE2084(in vec3 color){
	return vec3(
		ct_EOTF_SMPTE2084(color.r),
		ct_EOTF_SMPTE2084(color.g),
		ct_EOTF_SMPTE2084(color.b)
	);
}

vec3 ct_OETF_SMPTE2084(in vec3 color){
	return vec3(
		ct_OETF_SMPTE2084(color.r),
		ct_OETF_SMPTE2084(color.g),
		ct_OETF_SMPTE2084(color.b)
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

vec3 ct_EOTF_ARIB_STD_B67(in vec3 color){
	return vec3(
		ct_EOTF_ARIB_STD_B67(color.r),
		ct_EOTF_ARIB_STD_B67(color.g),
		ct_EOTF_ARIB_STD_B67(color.b)
	);
}

vec3 ct_OETF_ARIB_STD_B67(in vec3 color){
	return vec3(
		ct_OETF_ARIB_STD_B67(color.r),
		ct_OETF_ARIB_STD_B67(color.g),
		ct_OETF_ARIB_STD_B67(color.b)
	);
}


vec3 ct_EOTF(in int encoding, in vec3 color){
	vec3 result;

	switch(encoding){
	case ct_COLOR_TRANSFER_FUNCTION_BT601_709_2020:	result = ct_EOTF_bt601_709_2020(color);	break;
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA22:		result = ct_EOTF_gamma22(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA26:		result = ct_EOTF_gamma26(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA28:		result = ct_EOTF_gamma28(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1:	result = ct_EOTF_IEC61966_2_1(color);	break;
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_4:	result = ct_EOTF_IEC61966_2_4(color);	break;
	case ct_COLOR_TRANSFER_FUNCTION_SMPTE240M:		result = ct_EOTF_SMPTE240M(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_SMPTE2084:		result = ct_EOTF_SMPTE2084(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_ARIB_STD_B67:	result = ct_EOTF_ARIB_STD_B67(color);	break;
	default: /*ct_COLOR_TRANSFER_FUNCTION_LINEAR*/	result = color;							break;
	}

	return result;
}

vec3 ct_OETF(in int encoding, in vec3 color){
	vec3 result;

	switch(encoding){
	case ct_COLOR_TRANSFER_FUNCTION_BT601_709_2020:	result = ct_OETF_bt601_709_2020(color);	break;
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA22:		result = ct_OETF_gamma22(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA26:		result = ct_OETF_gamma26(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_GAMMA28:		result = ct_OETF_gamma28(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1:	result = ct_OETF_IEC61966_2_1(color);	break;
	case ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_4:	result = ct_OETF_IEC61966_2_4(color);	break;
	case ct_COLOR_TRANSFER_FUNCTION_SMPTE240M:		result = ct_OETF_SMPTE240M(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_SMPTE2084:		result = ct_OETF_SMPTE2084(color);		break;
	case ct_COLOR_TRANSFER_FUNCTION_ARIB_STD_B67:	result = ct_OETF_ARIB_STD_B67(color);	break;
	default: /*ct_COLOR_TRANSFER_FUNCTION_LINEAR*/	result = color;							break;
	}

	return result;
}



/*
 * Read / Write wrap all former operations
 */

vec4 ct_read(in ct_read_data inputProp, in vec4 color) {
	//Normalize the values into [0.0, 1.0]
	color = ct_expand(inputProp.colorRange, color);

	//Convert it into RGB color model if necessary
	if(inputProp.colorModel != ct_COLOR_MODEL_RGB) {
		color.rgb = (inputProp.mtxYCbCr2RGB * vec4(color.rgb, 0.0f)).rgb; 
	}

	//Undo all gamma-like compressions
	color.rgb = ct_EOTF(inputProp.colorTransferFunction, color.rgb);

	return color;
}

vec4 ct_write(in ct_write_data outputProp, in vec4 color) {
	//Apply a gamma-like compression
	color.rgb = ct_OETF(outputProp.colorTransferFunction, color.rgb); 

	//Convert it into a YCbCr color model if necessary
	if(outputProp.colorModel != ct_COLOR_MODEL_RGB) {
		color.rgb 	= (outputProp.mtxRGB2YCbCr * vec4(color.rgb, 0.0f)).rgb; 
	}

	//Convert it into the corresponding range
	color = ct_contract(outputProp.colorRange, color);

	return color;
}


/*
 * Sampling interpolation operations
 */

vec4 ct_passthrough(in ct_read_data inputProp, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords) {
	const vec4 color = ct_load(inputProp.planeFormat,  images, texCoords);
	return ct_read(inputProp, color);
}

vec4 ct_bilinear(in ct_read_data inputProp, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords) {
	vec4 result;

	//Convert the texture coordinates into image coordinates
	const ivec2 texSize = textureSize(images[0], 0);
	texCoords = texCoords * texSize - 0.5f;

	//Obtain the fractional part of the image coordinates
	const vec2 fxy  = fract(texCoords);

	//Calculate the offset
	const vec4 offset = (texCoords.xxyy + vec2(-0.5f, +0.5f).xyxy) / texSize.xxyy;

	//Sample multiple values.
	const vec4 sample0 = ct_passthrough(inputProp, images, offset.xz);
	const vec4 sample1 = ct_passthrough(inputProp, images, offset.yz);
	const vec4 sample2 = ct_passthrough(inputProp, images, offset.xw);
	const vec4 sample3 = ct_passthrough(inputProp, images, offset.yw);

	//Obtain the sample weights
	const float sx = fxy.x;
	const float sy = fxy.y;

	//Interpolate bilinearly between the samples
	result = mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy);

	return result;
}


vec4 ct_cubic(in float v) {
	//From https://stackoverflow.com/questions/13501081/efficient-bicubic-filtering-code-in-glsl
    const vec4 n = vec4(1.0f, 2.0f, 3.0f, 4.0f) - vec4(v);
    const vec4 s = n * n * n;
    const float x = s.x;
    const float y = s.y - 4.0f * s.x;
    const float z = s.z - 4.0f * s.y + 6.0f * s.x;
    const float w = 6.0 - x - y - z;
    return vec4(x, y, z, w) * (1.0f/6.0f);
}

vec4 ct_bicubic(in bool bilinearSampler, in ct_read_data inputProp, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords) {
	//From https://stackoverflow.com/questions/13501081/efficient-bicubic-filtering-code-in-glsl
	//Convert the texture coordinates into image coordinates
	const ivec2 texSize = textureSize(images[0], 0);
	texCoords = texCoords * texSize - 0.5f;

	//Obtain the fractional and integer part of the image coordinates
	const vec2 fxy  = fract(texCoords);
	texCoords -= fxy;

	//Sample the cubic function for the fractional parts
	const vec4 xcubic = ct_cubic(fxy.x);
	const vec4 ycubic = ct_cubic(fxy.y);
	
	//Calculate the multi-sample offsets
	const vec4 c = texCoords.xxyy + vec2(-0.5f, +1.5f).xyxy;
	const vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
	const vec4 offset = (c + vec4(xcubic.yw, ycubic.yw) / s) / texSize.xxyy;

	//Sample multiple values
	vec4 sample0;
	vec4 sample1;
	vec4 sample2;
	vec4 sample3;

	if(bilinearSampler) {
		//Sampler configured as bilinear
		sample0 = ct_passthrough(inputProp, images, offset.xz);
		sample1 = ct_passthrough(inputProp, images, offset.yz);
		sample2 = ct_passthrough(inputProp, images, offset.xw);
		sample3 = ct_passthrough(inputProp, images, offset.yw);
	} else {
		//Sampler configured as nearest
		sample0 = ct_bilinear(inputProp, images, offset.xz);
		sample1 = ct_bilinear(inputProp, images, offset.yz);
		sample2 = ct_bilinear(inputProp, images, offset.xw);
		sample3 = ct_bilinear(inputProp, images, offset.yw);
	}

	//Obtain the sample weights
	const float sx = s.x / (s.x + s.y);
	const float sy = s.z / (s.z + s.w);

	//Interpolate bilinearly between the samples
	return mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy);
}

vec4 ct_texture(in int mode, in ct_read_data inputProp, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords) {
	vec4 result;

	switch(mode){
	case ct_SAMPLE_MODE_BILINEAR:
		result = ct_bilinear(inputProp, images, texCoords);
		break;
	case ct_SAMPLE_MODE_BICUBIC:
		result = ct_bicubic(false, inputProp, images, texCoords);
		break;
	case ct_SAMPLE_MODE_BILINEAR_TO_BICUBIC:
		result = ct_bicubic(true, inputProp, images, texCoords);
		break;
	default: //ct_SAMPLE_MODE_PASSTHOUGH
		result = ct_passthrough(inputProp, images, texCoords);
		break;
	}

	return result;
}



/*
 * Color output
 */

void ct_output(in ct_write_data outputProp, in vec4 color, out vec4 plane0, out vec4 plane1, out vec4 plane2, out vec4 plane3){
	color = ct_write(outputProp, color);
	ct_store(outputProp.planeFormat, color, plane0, plane1, plane2, plane3);
}

vec4 ct_premultiply_alpha(in vec4 color) {
	color.rgb *= color.a;
	return color;
}

/*
 * Color space conversion
 */

vec4 ct_transferColor(in ct_read_data inputProp, in ct_write_data outputProp, in vec4 color){
	//Convert it into the destination color primaries if necessary
	if(inputProp.colorPrimaries != outputProp.colorPrimaries || inputProp.colorPrimaries == ct_COLOR_PRIMARIES_UNKNOWN) {
		color = outputProp.mtxXYZ2RGB * inputProp.mtxRGB2XYZ * color; //Both matrices should have a passthough for alpha
	}

	return color;
}
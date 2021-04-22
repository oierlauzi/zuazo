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
	case ct_PLANE_FORMAT_G_BR_A:
		result.g = plane0.r;
		result.br = plane1.rg;
		result.a = plane2.r;
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

void ct_store(in int planeFormat, in vec4 color, out vec4 plane0, out vec4 plane1, out vec4 plane2, out vec4 plane3) {
	//Attach the alpha channel so that it gets correctly blended
	switch(planeFormat){
	case ct_PLANE_FORMAT_G_BR:
		plane0.ra = color.ga;
		plane1.rga = color.bra;
		break;
	case ct_PLANE_FORMAT_G_BR_A:
		plane0.ra = color.ga;
		plane1.rga = color.bra;
		plane2.ra = color.aa;
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
vec3 ct_EOTF_gamma(in float gain, in float gamma, in vec3 color){
	return pow(color, vec3(gamma));
}

vec3 ct_OETF_gamma(in float gain, in float gamma, in vec3 color){
	return pow(color, vec3(1.0f/gamma));
}



//Note: gains are expresed OETF-wise
vec3 ct_EOTF_hybrid_linear_gamma(in float gain, in float alpha, in float beta, in float gamma, in vec3 color) {
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

vec3 ct_OETF_hybrid_linear_gamma(in float gain, in float alpha, in float beta, in float gamma, in vec3 color){
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


//Note: gains are expresed OETF-wise
vec3 ct_EOTF_hybrid_linear_gamma_EG(in float gain, in float alpha, in float beta, in float gamma, in vec3 color) {
	return sign(color)*ct_EOTF_hybrid_linear_gamma(gain, alpha, beta, gamma, abs(color));
}

vec3 ct_OETF_hybrid_linear_gamma_EG(in float gain, in float alpha, in float beta, in float gamma, in vec3 color){
	return sign(color)*ct_OETF_hybrid_linear_gamma(gain, alpha, beta, gamma, abs(color));
}



const float ct_BT1886_GAIN = 4.5f;
const float ct_BT1886_ALPHA = 1.09929682680944f;
const float ct_BT1886_BETA = 0.018053968510807f;
const float ct_BT1886_GAMMA = 1.0f/0.45f; //2.222222...

vec3 ct_EOTF_bt1886(in vec3 color){
	return ct_EOTF_hybrid_linear_gamma(ct_BT1886_GAIN, ct_BT1886_ALPHA, ct_BT1886_BETA, ct_BT1886_GAMMA, color);
}

vec3 ct_OETF_bt1886(in vec3 color){
	return ct_OETF_hybrid_linear_gamma(ct_BT1886_GAIN, ct_BT1886_ALPHA, ct_BT1886_BETA, ct_BT1886_GAMMA, color);
}



vec3 ct_EOTF_gamma22(in vec3 color){
	return ct_EOTF_gamma(1.0f, 2.2f, color);
}

vec3 ct_OETF_gamma22(in vec3 color){
	return ct_OETF_gamma(1.0f, 2.2f, color);
}

vec3 ct_EOTF_gamma26(in vec3 color){
	return ct_EOTF_gamma(1.0f, 2.6f, color);
}

vec3 ct_OETF_gamma26(in vec3 color){
	return ct_OETF_gamma(1.0f, 2.6f, color);
}

vec3 ct_EOTF_gamma28(in vec3 color){
	return ct_EOTF_gamma(1.0f, 2.8f, color);
}

vec3 ct_OETF_gamma28(in vec3 color){
	return ct_OETF_gamma(1.0f, 2.8f, color);
}



const float ct_IEC61966_2_1_GAIN = 12.92f;
const float ct_IEC61966_2_1_ALPHA = 1.055f;
const float ct_IEC61966_2_1_BETA = 0.0031308f;
const float ct_IEC61966_2_1_GAMMA = 2.4;

vec3 ct_EOTF_IEC61966_2_1(in vec3 color){
	return ct_EOTF_hybrid_linear_gamma(ct_IEC61966_2_1_GAIN, ct_IEC61966_2_1_ALPHA, ct_IEC61966_2_1_BETA, ct_IEC61966_2_1_GAMMA, color);
}

vec3 ct_OETF_IEC61966_2_1(in vec3 color){
	return ct_OETF_hybrid_linear_gamma(ct_IEC61966_2_1_GAIN, ct_IEC61966_2_1_ALPHA, ct_IEC61966_2_1_BETA, ct_IEC61966_2_1_GAMMA, color);
}



vec3 ct_EOTF_IEC61966_2_4(in vec3 color){
	return ct_EOTF_hybrid_linear_gamma_EG(ct_BT1886_GAIN, ct_BT1886_ALPHA, ct_BT1886_BETA, ct_BT1886_GAMMA, color);
}

vec3 ct_OETF_IEC61966_2_4(in vec3 color){
	return ct_OETF_hybrid_linear_gamma_EG(ct_BT1886_GAIN, ct_BT1886_ALPHA, ct_BT1886_BETA, ct_BT1886_GAMMA, color);
}



const float ct_SMPTE240M_GAIN = 4.0f;
const float ct_SMPTE240M_ALPHA = 1.1115f;
const float ct_SMPTE240M_BETA =  0.0228f;
const float ct_SMPTE240M_GAMMA = 1.0f/0.45; //2.222222

vec3 ct_EOTF_SMPTE240M(in vec3 color){
	return ct_EOTF_hybrid_linear_gamma(ct_SMPTE240M_GAIN, ct_SMPTE240M_ALPHA, ct_SMPTE240M_BETA, ct_SMPTE240M_GAMMA, color);
}

vec3 ct_OETF_SMPTE240M(in vec3 color){
	return ct_OETF_hybrid_linear_gamma(ct_SMPTE240M_GAIN, ct_SMPTE240M_ALPHA, ct_SMPTE240M_BETA, ct_SMPTE240M_GAMMA, color);
}



const float ct_SMPTE2084_C1 = 107.0f / 128.0f;
const float ct_SMPTE2084_C2 = 2413.0f / 128.0f;
const float ct_SMPTE2084_C3 = 2392.0f / 128.0f;  //ct_SMPTE2084_C1 + ct_SMPTE2084_C2 - 1;
const float ct_SMPTE2084_M1 = 1305.0f / 8192.0f;
const float ct_SMPTE2084_M2 = 2523.0f / 32.0f;

vec3 ct_EOTF_SMPTE2084(in vec3 color){
	//Compute the intermediary values
	const vec3 temp = pow(max(color, vec3(0.0f)), vec3(1.0f/ct_SMPTE2084_M2));
	const vec3 num = temp - ct_SMPTE2084_C1;
	const vec3 den = ct_SMPTE2084_C2 - ct_SMPTE2084_C3*temp;

	//Obtain the final result
	return pow(max(num/den, vec3(0.0f)), vec3(1.0f/ct_SMPTE2084_M1));
}

vec3 ct_OETF_SMPTE2084(in vec3 color){
	//Equations from:
	//https://en.wikipedia.org/wiki/High-dynamic-range_video

	//Compute the intermediary values
	const vec3 temp = pow(max(color, vec3(0.0f)), vec3(ct_SMPTE2084_M1));
	const vec3 num = ct_SMPTE2084_C1 	+ ct_SMPTE2084_C2*temp;
	const vec3 den = vec3(1.0f)			+ ct_SMPTE2084_C3*temp;

	//Obtain the final result
	return pow(num/den, vec3(ct_SMPTE2084_M2));
}



const float ct_ARIB_STD_B67_A = 0.17883277f;
const float ct_ARIB_STD_B67_B = 0.28466892f; //1 - 4a
const float ct_ARIB_STD_B67_C = 0.55991073f; //0.5 - a ln(4a)

vec3 ct_EOTF_ARIB_STD_B67(in vec3 color) {
	//L = (V^2) / 3								for 0 <= L <= 1/2
	//L = (exp((V-c)/a) + b) / 12				for 1/2 < L <= 1

	//Sanitize the value. 0 <= L <= 1
	color = clamp(color, vec3(0.0f), vec3(1.0f));
	
	//Evaluate both parts of the expression (branchless)
	const vec3 piece0 = (1.0f/3.0f)*color*color;
	const vec3 piece1 = (1.0f/12.0f)*(exp((color - ct_ARIB_STD_B67_C) / ct_ARIB_STD_B67_A) + ct_ARIB_STD_B67_B);

	//Select the maximum of both
	return mix(piece0, piece1, greaterThan(color, vec3(0.5f)));
}

vec3 ct_OETF_ARIB_STD_B67(in vec3 color){
	//Equation from:
	//https://en.wikipedia.org/wiki/Hybrid_Log-Gamma
	//V = sqrt(3*L)								for 0 <= L <= 1/12
	//V = a*ln(12*L - b) + c					for 1/12 < L <= 1

	//Sanitize the value. 0 <= L <= 1
	color = clamp(color, vec3(0.0f), vec3(1.0f));
	
	//Evaluate both parts of the expression (branchless)
	const vec3 piece0 = sqrt(3*color);
	const vec3 piece1 = ct_ARIB_STD_B67_A*log(12*color - ct_ARIB_STD_B67_B) + ct_ARIB_STD_B67_C;

	//Select the minimum of both
	return mix(piece0, piece1, greaterThan(color, vec3(1.0/12.0f)));
}



vec3 ct_EOTF(in int encoding, in vec3 color){
	vec3 result;

	switch(encoding){
	case ct_COLOR_TRANSFER_FUNCTION_BT1886:			result = ct_EOTF_bt1886(color);			break;
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
	case ct_COLOR_TRANSFER_FUNCTION_BT1886:			result = ct_OETF_bt1886(color);			break;
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


vec4 ct_catmullRom(in float v) {
	//From https://stackoverflow.com/questions/13501081/efficient-bicubic-filtering-code-in-glsl
    const vec4 n = vec4(1.0f, 2.0f, 3.0f, 4.0f) - vec4(v);
    const vec4 s = n * n * n;
    const float x = s.x;
    const float y = s.y - 4.0f * s.x;
    const float z = s.z - 4.0f * s.y + 6.0f * s.x;
    const float w = 6.0 - x - y - z;
    return (1.0f/6.0f) * vec4(x, y, z, w);
}

vec4 ct_bicubic(in ct_read_data inputProp, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords) {
	//Based on:
	//https://github.com/visionworkbench/visionworkbench/blob/95f0729c31121838e9d9f993b92c1912bf024d81/src/vw/GPU/Shaders/Interp/interpolation-bicubic.glsl
	//TODO optimize using matrices

	//Convert the texture coordinates into image coordinates
	const ivec2 texSize = textureSize(images[0], 0);
	texCoords = texCoords * texSize - 0.5f;

	//Obtain the fractional part of the image coordinates
	const vec2 fxy  = fract(texCoords);

	//Sample the cubic function for the fractional parts
	const vec4 xCubic = ct_catmullRom(fxy.x);
	const vec4 yCubic = ct_catmullRom(fxy.y);

	//Sample all 16 positions
	const mat4 samples[4] = {
		//Column 0
		mat4(
    		ct_passthrough(inputProp, images, (texCoords + vec2(-1.0f, -1.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(-1.0f, +0.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(-1.0f, +1.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(-1.0f, +2.0f))/texSize) 
		),
		//Column 1
		mat4(
    		ct_passthrough(inputProp, images, (texCoords + vec2(+0.0f, -1.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(+0.0f, +0.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(+0.0f, +1.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(+0.0f, +2.0f))/texSize) 
		),
		//Column 2
		mat4(
    		ct_passthrough(inputProp, images, (texCoords + vec2(+1.0f, -1.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(+1.0f, +0.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(+1.0f, +1.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(+1.0f, +2.0f))/texSize) 
		),
		//Column 3
		mat4(
    		ct_passthrough(inputProp, images, (texCoords + vec2(+2.0f, -1.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(+2.0f, +0.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(+2.0f, +1.0f))/texSize),
    		ct_passthrough(inputProp, images, (texCoords + vec2(+2.0f, +2.0f))/texSize) 
		)
	};

	//Interpolate all rows to obtain a single color per column
	const mat4 columns = mat4(
		samples[0] * yCubic, //Column 0
		samples[1] * yCubic, //Column 1
		samples[2] * yCubic, //Column 2
		samples[3] * yCubic  //Column 3
	);

	//Interpolate all columns to obtain a single value
	return columns * xCubic; //Maybe *0.25f?
}

vec4 ct_bicubicFromBilinear(in ct_read_data inputProp, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords) {
	//From https://stackoverflow.com/questions/13501081/efficient-bicubic-filtering-code-in-glsl
	//Convert the texture coordinates into image coordinates
	const ivec2 texSize = textureSize(images[0], 0);
	texCoords = texCoords * texSize - 0.5f;

	//Obtain the fractional and integer part of the image coordinates
	const vec2 fxy  = fract(texCoords);
	texCoords -= fxy;

	//Sample the cubic function for the fractional parts
	const vec4 xCubic = ct_catmullRom(fxy.x);
	const vec4 yCubic = ct_catmullRom(fxy.y);
	
	//Calculate the multi-sample offsets
	const vec4 c = texCoords.xxyy + vec2(-0.5f, +1.5f).xyxy;
	const vec4 s = vec4(xCubic.xz + xCubic.yw, yCubic.xz + yCubic.yw);
	const vec4 offset = (c + vec4(xCubic.yw, yCubic.yw) / s) / texSize.xxyy;

	//Sample 4 times using the bilinear sampler
	const vec4 sample0 = ct_passthrough(inputProp, images, offset.xz);
	const vec4 sample1 = ct_passthrough(inputProp, images, offset.yz);
	const vec4 sample2 = ct_passthrough(inputProp, images, offset.xw);
	const vec4 sample3 = ct_passthrough(inputProp, images, offset.yw);

	//Obtain the sample weights
	const float sx = s.x / (s.x + s.y);
	const float sy = s.z / (s.z + s.w);

	//Interpolate bilinearly between the samples
	return mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy);
}

vec4 ct_texture(in int mode, in ct_read_data inputProp, in sampler2D images[ct_SAMPLER_COUNT], in vec2 texCoords) {
	vec4 result;

	//Test if any of the UV coordinates is outside [0, 0], [1, 1] square
	const bool isOutside = any(lessThan(vec4(texCoords, -texCoords), vec4(vec2(0.0f), vec2(-1.0f))));
	
	//Depending on if it is outside, sample or return black
	if(isOutside) {
		result = vec4(0.0f);
	} else {
		switch(mode){
		case ct_SAMPLE_MODE_BILINEAR:
			result = ct_bilinear(inputProp, images, texCoords);
			break;
		case ct_SAMPLE_MODE_BICUBIC:
			result = ct_bicubic(inputProp, images, texCoords);
			break;
		case ct_SAMPLE_MODE_BICUBIC_FROM_BILINEAR:
			result = ct_bicubicFromBilinear(inputProp, images, texCoords);
			break;
		default: //ct_SAMPLE_MODE_PASSTHOUGH
			result = ct_passthrough(inputProp, images, texCoords);
			break;
		}
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
#include "frame.h"

#define frame_sampler(x) frame_sampler ## x

#define frame_descriptor_set(x) 															\
	layout(set = x, binding = frame_SAMPLER_BINDING) uniform sampler2D frame_sampler(x);	\

/*
 * Sampling interpolation operations
 */

vec4 frame_bilinear(in sampler2D image, in vec2 texCoords) {
	vec4 result;

	//Convert the texture coordinates into image coordinates
	const ivec2 texSize = textureSize(image, 0);
	texCoords = texCoords * texSize - 0.5f;

	//Obtain the fractional part of the image coordinates
	const vec2 fxy  = fract(texCoords);

	//Calculate the offset
	const vec4 offset = (texCoords.xxyy + vec2(-0.5f, +0.5f).xyxy) / texSize.xxyy;

	//Sample multiple values.
	const vec4 sample0 = texture(image, offset.xz);
	const vec4 sample1 = texture(image, offset.yz);
	const vec4 sample2 = texture(image, offset.xw);
	const vec4 sample3 = texture(image, offset.yw);

	//Obtain the sample weights
	const float sx = fxy.x;
	const float sy = fxy.y;

	//Interpolate bilinearly between the samples
	result = mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy);

	return result;
}


vec4 frame_catmullRom(in float v) {
	//From https://stackoverflow.com/questions/13501081/efficient-bicubic-filtering-code-in-glsl
    const vec4 n = vec4(1.0f, 2.0f, 3.0f, 4.0f) - vec4(v);
    const vec4 s = n * n * n;
    const float x = s.x;
    const float y = s.y - 4.0f * s.x;
    const float z = s.z - 4.0f * s.y + 6.0f * s.x;
    const float w = 6.0 - x - y - z;
    return (1.0f/6.0f) * vec4(x, y, z, w);
}

vec4 frame_bicubic(in sampler2D image, in vec2 texCoords) {
	//Based on:
	//https://github.com/visionworkbench/visionworkbench/blob/95f0729c31121838e9d9f993b92c1912bf024d81/src/vw/GPU/Shaders/Interp/interpolation-bicubic.glsl
	//TODO optimize using matrices

	//Convert the texture coordinates into image coordinates
	const ivec2 texSize = textureSize(image, 0);
	texCoords = texCoords * texSize - 0.5f;

	//Obtain the fractional part of the image coordinates
	const vec2 fxy  = fract(texCoords);

	//Sample the cubic function for the fractional parts
	const vec4 xCubic = frame_catmullRom(fxy.x);
	const vec4 yCubic = frame_catmullRom(fxy.y);

	//Sample all 16 positions
	const mat4 samples[4] = {
		//Column 0
		mat4(
    		texture(image, (texCoords + vec2(-1.0f, -1.0f))/texSize),
    		texture(image, (texCoords + vec2(-1.0f, +0.0f))/texSize),
    		texture(image, (texCoords + vec2(-1.0f, +1.0f))/texSize),
    		texture(image, (texCoords + vec2(-1.0f, +2.0f))/texSize) 
		),
		//Column 1
		mat4(
    		texture(image, (texCoords + vec2(+0.0f, -1.0f))/texSize),
    		texture(image, (texCoords + vec2(+0.0f, +0.0f))/texSize),
    		texture(image, (texCoords + vec2(+0.0f, +1.0f))/texSize),
    		texture(image, (texCoords + vec2(+0.0f, +2.0f))/texSize) 
		),
		//Column 2
		mat4(
    		texture(image, (texCoords + vec2(+1.0f, -1.0f))/texSize),
    		texture(image, (texCoords + vec2(+1.0f, +0.0f))/texSize),
    		texture(image, (texCoords + vec2(+1.0f, +1.0f))/texSize),
    		texture(image, (texCoords + vec2(+1.0f, +2.0f))/texSize) 
		),
		//Column 3
		mat4(
    		texture(image, (texCoords + vec2(+2.0f, -1.0f))/texSize),
    		texture(image, (texCoords + vec2(+2.0f, +0.0f))/texSize),
    		texture(image, (texCoords + vec2(+2.0f, +1.0f))/texSize),
    		texture(image, (texCoords + vec2(+2.0f, +2.0f))/texSize) 
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

vec4 frame_bicubicFromBilinear(in sampler2D image, in vec2 texCoords) {
	//From https://stackoverflow.com/questions/13501081/efficient-bicubic-filtering-code-in-glsl
	//Convert the texture coordinates into image coordinates
	const ivec2 texSize = textureSize(image, 0);
	texCoords = texCoords * texSize - 0.5f;

	//Obtain the fractional and integer part of the image coordinates
	const vec2 fxy  = fract(texCoords);
	texCoords -= fxy;

	//Sample the cubic function for the fractional parts
	const vec4 xCubic = frame_catmullRom(fxy.x);
	const vec4 yCubic = frame_catmullRom(fxy.y);
	
	//Calculate the multi-sample offsets
	const vec4 c = texCoords.xxyy + vec2(-0.5f, +1.5f).xyxy;
	const vec4 s = vec4(xCubic.xz + xCubic.yw, yCubic.xz + yCubic.yw);
	const vec4 offset = (c + vec4(xCubic.yw, yCubic.yw) / s) / texSize.xxyy;

	//Sample 4 times using the bilinear sampler
	const vec4 sample0 = texture(image, offset.xz);
	const vec4 sample1 = texture(image, offset.yz);
	const vec4 sample2 = texture(image, offset.xw);
	const vec4 sample3 = texture(image, offset.yw);

	//Obtain the sample weights
	const float sx = s.x / (s.x + s.y);
	const float sy = s.z / (s.z + s.w);

	//Interpolate bilinearly between the samples
	return mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy);
}

vec4 frame_texture(in int mode, in sampler2D image, in vec2 texCoords) {
	vec4 result;

	//Test if any of the UV coordinates is outside [0, 0], [1, 1] square
	const bool isOutside = any(lessThan(vec4(texCoords, -texCoords), vec4(vec2(0.0f), vec2(-1.0f))));
	
	//Depending on if it is outside, sample or return black
	if(isOutside) {
		result = vec4(0.0f);
	} else {
		switch(mode){
		case frame_SAMPLE_MODE_BILINEAR:
			result = frame_bilinear(image, texCoords);
			break;
		case frame_SAMPLE_MODE_BICUBIC:
			result = frame_bicubic(image, texCoords);
			break;
		case frame_SAMPLE_MODE_BICUBIC_FROM_BILINEAR:
			result = frame_bicubicFromBilinear(image, texCoords);
			break;
		default: //frame_SAMPLE_MODE_PASSTHOUGH
			result = texture(image, texCoords);
			break;
		}
	}

	return result;
}

vec4 frame_premultiply_alpha(in vec4 color) {
	color.rgb *= color.a;
	return color;
}

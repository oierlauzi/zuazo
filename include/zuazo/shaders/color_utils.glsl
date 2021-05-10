//Based on:
//http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
vec3 rgb2hsv(vec3 rgb) {
	//Some constants
	const vec4 K = vec4(0.0, -1.0/3.0, 2.0/3.0, -1.0);
	const float epsilon = 1.0e-10;

	const vec4 p = rgb.g < rgb.b ? vec4(rgb.bg, K.wz) : vec4(rgb.gb, K.xy);
	const vec4 q = rgb.r < p.x   ? vec4(p.xyw, rgb.r) : vec4(rgb.r, p.yzx);

	//Calculate the final result
	const float d = q.x - min(q.w, q.y);
	return vec3(
		60.0 * abs(q.z + (q.w - q.y) / (d + epsilon)), //Unlike the original, hue 0 to 360
		d / (q.x + epsilon), 
		q.x
	);
}

//Based on:
//http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
vec3 hsv2rgb(vec3 hsv) {
	const float angleNorm = 1.0/60.0; //Used to turn 0-360 into 0-6
	const vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);

	const vec3 p = abs(angleNorm*fract(hsv.xxx + K.xyz) - K.www);
	return hsv.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), hsv.y);
}

//Based on:
//https://en.wikipedia.org/wiki/HSL_and_HSV#Interconversion
vec3 hsv2hsl(in vec3 hsv) {
	//Some aliases
	const float h = hsv.x; //H remains unchanged
	const float v = hsv.z;

	const float l = v*(1 - 0.5*hsv.y); //L=(V - V*S/2) = V*(1-S/2)
	const float sDen = min(l, 1-l);
	const float s = (sDen == 0.0) ? 0.0 : (v - l) / sDen; //Warining! div by zero

	return vec3(h, s, l);
}

//Based on:
//https://en.wikipedia.org/wiki/HSL_and_HSV#Interconversion
vec3 hsl2hsv(in vec3 hsl) {
	//Some aliases
	const float l = hsl.z;
	const float cl = 1.0f - l;
	const float h = hsl.x; //H remains unchanged

	const float v = l + hsl.y*min(l, cl);
	const float s = (v == 0) ? 0.0 : 2.0*cl / v; 

	return vec3(h, s, v);
}

vec3 rgb2hsl(vec3 rgb) {
	return hsv2hsl(rgb2hsv(rgb));
}

vec3 hsl2rgb(vec3 hsl) {
	return hsv2rgb(hsl2hsv(hsl));
}

float getLuminance(in vec3 rgb) {
	const vec3 weights = vec3(0.299f, 0.587f, 0.114); //YUV/YIQ
	return dot(rgb, weights);
}
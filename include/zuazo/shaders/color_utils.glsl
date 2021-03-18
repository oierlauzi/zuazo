vec3 rgb2hsl(in vec3 rgb) {
	//Based on:
	//https://en.wikipedia.org/wiki/HSL_and_HSV
	//Obtain the maximum and the minimum components of rgb
	const float maxVal = max(rgb.r, max(rgb.g, rgb.b));
	const float minVal = min(rgb.r, min(rgb.g, rgb.b));

	//Calculate the lightness
	const float lightness = 0.5f*minVal + 0.5f*maxVal; //Average

	float hue = 0.0f;
	float saturation = 0.0f;
	if(maxVal != minVal) {
		//Calculate the hue
		const float hueDen = maxVal - minVal;
		if(maxVal == rgb.g) { //max==G
			hue = (rgb.b-rgb.r) / hueDen + 2.0f;
		} else if(maxVal == rgb.b) { //max==B
			hue = (rgb.r-rgb.g) / hueDen + 4.0f;
		} else if(rgb.b > rgb.g) { //max==R and G-B < 0
			hue = (rgb.g-rgb.b) / hueDen + 6.0f;
		} else { //max==R and G-B > 0
			hue = (rgb.g-rgb.b) / hueDen;
		}
		hue *= 60.0f;

		//Calculate the saturation
		const float satNum = hueDen;
		if(lightness <= 0.5f) {
			saturation = satNum / lightness;
		} else {
			saturation = satNum / (1.0f - lightness);
		}
		saturation /= 2.0f;
	}

	return vec3(hue, saturation, lightness);
}

float getLuminance(in vec3 rgb) {
	const vec3 weights = vec3(0.299f, 0.587f, 0.114); //YUV
	return dot(rgb, weights);
}
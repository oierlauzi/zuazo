vec4 border_smooth(	in vec4 inColor, 
					in vec4 outColor, 
					in vec4 borderColor, 
					in float width, 
					in float smoothness, 
					in float sDist )
{
	//Obtain the blend coefficients
	const float blend0 = clamp(0.5f - (sDist + width) / smoothness, 0.0f, 1.0f); //in gain
	const float blend1 = clamp(0.5f + sDist / smoothness, 0.0f, 1.0f); //out gain
	const float blend2 = 1.0f - blend0 - blend1; //border gain

	return blend0*inColor + blend1*outColor + blend2*borderColor;
}
#include "Rectangle.h"

using namespace Zuazo::Graphics::Shapes;

Rectangle::RectangleTexCoords Rectangle::scaleFrame(
		const Utils::Resolution& srcRes,
		const Utils::Resolution& dstRes,
		Utils::ScalingModes scalingMode)
{
	float sX, sY, s;

	//Calculate the scale
	sX=(float)dstRes.width / srcRes.width;
	sY=(float)dstRes.height / srcRes.height;


	//Evaluate drawing area's dimensions
	switch(scalingMode){
	case Utils::ScalingModes::Boxed:
		//Scale with the minimum factor
		s=std::min(sX, sY);
		sX=s;
		sY=s;
		break;
	case Utils::ScalingModes::Cropped:
		//Scale with the maximum factor
		s=std::max(sX, sY);
		sX=s;
		sY=s;
		break;
	case Utils::ScalingModes::Stretched:
		//Scale independently.
		//Neither sX nor sY needs to be modified
		break;
	case Utils::ScalingModes::ClampVert:
		//Scale according to sY
		sX=sY;
		break;
	case Utils::ScalingModes::ClampHor:
		//Scale according to sX
		sY=sX;
		break;
	default:
		//This should not happen
		//Don't show anything
		sX=0;
		sY=0;
		break;
	}

	float diffX=1 / (2 * sX); //TODO
	float diffY=1 / (2 * sY);

	printf("%f, %f\n", diffX, diffY);
	return RectangleTexCoords {
		Utils::Vec<2, float>{0.5 - diffX, 	0.5	- diffY},
		Utils::Vec<2, float>{0.5 + diffX, 	0.5	+ diffY},
	};
}

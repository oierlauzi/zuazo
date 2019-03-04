#include "Frame.h"

#include "../Utils/Resolution.h"
#include "../Utils/ScalingModes.h"
#include "../Utils/Vector.h"

namespace Zuazo::Graphics{

MultiPool<size_t, GL::Buffer<GL::BufferTypes::PixelUnpack>> Frame::s_pboPool;
MultiPool<GL::ImageAttributes, GL::Texture> 	Frame::s_texPool;


Shapes::Rectangle::RectangleTexCoords Frame::scaleFrame(
		const Utils::Resolution& dstRes,
		Utils::ScalingModes scalingMode) const
{
	const Utils::Resolution& srcRes=m_attributes.res;
	if(srcRes){
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

		/*
		float diffX = (srcRes.width * sX - dstRes.width) / (2 * srcRes.width * sX);
		float diffY = (srcRes.height * sY - dstRes.height) / (2 * srcRes.height * sY);
		Simplified: */

		float diffX = 0.5 - (dstRes.width) / (2 * srcRes.width * sX);
		float diffY = 0.5 - (dstRes.height) / (2 * srcRes.height * sY);


		return Shapes::Rectangle::RectangleTexCoords {
			Utils::Vec<2, float>{1.0 - diffX, diffY},
			Utils::Vec<2, float>{diffX, 1.0 - diffY},
		};
	} else return Shapes::Rectangle::RectangleTexCoords();
}

}

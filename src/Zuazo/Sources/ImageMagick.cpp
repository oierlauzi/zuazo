#include "ImageMagick.h"

#include <magick/colorspace.h>
#include <magick/magick-type.h>
#include <Magick++/Geometry.h>
#include <Magick++/Image.h>

#include "../Utils/PixelFormat.h"
#include "../Utils/Resolution.h"

using namespace Zuazo::Sources;


void ImageMagick::open(){
	if(m_image){
		Magick::Geometry size=m_image->size();
		if(size._width && size._height){
			//We have a valid image
			Magick::ColorspaceType colorSpace=m_image->colorSpace();

			if(colorSpace != Magick::ColorspaceType::RGBColorspace){
				//Image is not RGB -> quantize it to RGB
				std::shared_ptr<Magick::Image> rgbImage(new Magick::Image(*m_image));
				rgbImage->quantizeColorSpace(Magick::ColorspaceType::RGBColorspace);
				m_image=rgbImage;
			}

			//Get all the pixels
			const Magick::Quantum* pixels=m_image->getConstPixels(0, 0, size._width, size._height);

			ZuazoBase::open();
		}
	}
}

void ImageMagick::close(){
	m_videoSourcePad.reset();
	ZuazoBase::close();
}

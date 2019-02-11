#pragma once

#include "Resolution.h"
#include "PixelTypes.h"

namespace Zuazo::Utils{

struct ImageAttributes{
	Resolution		res;
	PixelTypes		pixType;

	constexpr ImageAttributes() :
			pixType(PixelTypes::RGBA)
	{
	}

	constexpr ImageAttributes(const Resolution& res, PixelTypes pixType) :
			res(res),
			pixType(pixType)
	{
	}

	ImageAttributes(const ImageAttributes& other)=default;
	~ImageAttributes()=default;

	constexpr operator bool() const{
		return res.operator bool();
	}

	constexpr int operator==(const ImageAttributes& right)const{
        return res==right.res && pixType==right.pixType;
    }

	constexpr int operator!=(const ImageAttributes& right)const{
		return res!=right.res || pixType!=right.pixType;
    }

	constexpr size_t size() const{
		size_t pixSize(0);

		switch(pixType){
		case PixelTypes::RED:
			pixSize=PIXEL_SIZE<PixelTypes::RED>;
			break;

		case PixelTypes::GREEN:
			pixSize=PIXEL_SIZE<PixelTypes::GREEN>;
			break;

		case PixelTypes::BLUE:
			pixSize=PIXEL_SIZE<PixelTypes::BLUE>;
			break;

		case PixelTypes::ALPHA:
			pixSize=PIXEL_SIZE<PixelTypes::ALPHA>;
			break;

		case PixelTypes::RGB:
			pixSize=PIXEL_SIZE<PixelTypes::RGB>;
			break;

		case PixelTypes::RGBA:
			pixSize=PIXEL_SIZE<PixelTypes::RGBA>;
			break;

		case PixelTypes::LUMA:
			pixSize=PIXEL_SIZE<PixelTypes::LUMA>;
			break;

		case PixelTypes::LUMA_ALPHA:
			pixSize=PIXEL_SIZE<PixelTypes::LUMA_ALPHA>;
			break;
		}

		return res.size(pixSize);
	}
};
}

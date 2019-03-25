#pragma once

#include <stdlib.h>
#include <sys/types.h>

#include "ImageAttributes.h"

namespace Zuazo::Graphics{

class ImageBuffer{
public:
	ImageAttributes		att;
	u_int8_t* const		data;

	constexpr ImageBuffer() :
		data(nullptr),
		wasAllocated(false)
	{
	}

	ImageBuffer(const ImageAttributes& att) :
		att(att),
		data((u_int8_t*)malloc(att.getSize())),
		wasAllocated(true)
	{
	}

	ImageBuffer(const ImageAttributes& att, u_int8_t* data) :
		att(att),
		data(data),
		wasAllocated(false)
	{
	}

	constexpr ImageBuffer(const ImageBuffer& img) :
		att(img.att),
		data(img.data),
		wasAllocated(false)
	{
	}

	constexpr ImageBuffer(ImageBuffer&& img) :
		att(img.att),
		data(img.data),
		wasAllocated(img.wasAllocated)
	{
		img.wasAllocated=false;
	}

	~ImageBuffer(){
		if(wasAllocated)
			free(data);
	}
private:
	bool wasAllocated;
};
}

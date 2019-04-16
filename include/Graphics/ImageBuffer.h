#pragma once

#include "ImageAttributes.h"

#include <stdlib.h>
#include <sys/types.h>

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

	void						textureImage() const;
	void						textureSubImage() const;
private:
	bool wasAllocated;
};

inline void	ImageBuffer::textureImage() const{
	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			att.pixFmt.getFormat(),
			att.res.width,
			att.res.height,
			0,
			att.pixFmt.getFormat(),
			att.pixFmt.getType(),
			data
	);
	GL::Texture2D::swizzleMask(att.pixFmt.getSwizzleMaskComp());
}

inline void	ImageBuffer::textureSubImage() const{
	glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0,
			0,
			att.res.width,
			att.res.height,
			att.pixFmt.getFormat(),
			att.pixFmt.getType(),
			data
	);
	GL::Texture2D::swizzleMask(att.pixFmt.getSwizzleMaskComp());
}
}

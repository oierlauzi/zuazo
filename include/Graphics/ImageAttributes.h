#pragma once

#include "PixelFormat.h"
#include "GL/Texture2D.h"
#include "../Utils/Resolution.h"
#include "../Utils/ImageAttributes.h"

#include <stddef.h>

namespace Zuazo::Graphics{

typedef Zuazo::Utils::Resolution Resolution;

struct ImageAttributes{
	Resolution			res;
	PixelFormat			pixFmt;

	ImageAttributes()=default;
	constexpr ImageAttributes(const Resolution& res, const PixelFormat& pixFmt):
		res(res),
		pixFmt(pixFmt)
	{
	}

	constexpr ImageAttributes(const Utils::ImageAttributes& att):
		res(att.res),
		pixFmt(att.pixFmt)
	{
	}
	ImageAttributes(const ImageAttributes& other)=default;
	~ImageAttributes()=default;

	constexpr operator bool() const{
		return res && pixFmt;
	}

	constexpr int operator==(const ImageAttributes& other) const{
		return res == other.res && pixFmt == other.pixFmt;
	}

	constexpr int operator!=(const ImageAttributes& other) const{
		return res != other.res || pixFmt != other.pixFmt;
	}

	constexpr int operator<(const ImageAttributes& other) const{
		return getSize() < other.getSize();
	}

	constexpr int operator>(const ImageAttributes& other) const{
		return getSize() > other.getSize();
	}

	constexpr int operator<=(const ImageAttributes& other) const{
		return getSize() <= other.getSize();
	}

	constexpr int operator>=(const ImageAttributes& other) const{
		return getSize() >= other.getSize();
	}

	constexpr size_t getStride() const{
		return pixFmt.getSize() * res.width;
	}

	constexpr size_t getSize() const{
		return getStride() * res.height;
	}

	void textureImage() const;
	void textureSubImage() const;
};


inline void	ImageAttributes::textureImage() const{
	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			pixFmt.getFormat(),
			res.width,
			res.height,
			0,
			pixFmt.getFormat(),
			pixFmt.getType(),
			0
	);
	GL::Texture2D::swizzleMask(pixFmt.getSwizzleMaskComp());
}

inline void	ImageAttributes::textureSubImage() const{
	glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0,
			0,
			res.width,
			res.height,
			pixFmt.getFormat(),
			pixFmt.getType(),
			0
	);
	GL::Texture2D::swizzleMask(pixFmt.getSwizzleMaskComp());
}

}

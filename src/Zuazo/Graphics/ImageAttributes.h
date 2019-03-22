#pragma once

#include <stddef.h>

#include "../Utils/Resolution.h"
#include "PixelFormat.h"


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
	ImageAttributes(const ImageAttributes& other)=default;
	~ImageAttributes()=default;

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
};

}

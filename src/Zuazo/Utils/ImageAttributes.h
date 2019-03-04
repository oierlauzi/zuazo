#pragma once

#include "PixelFormats.h"
#include "Resolution.h"

namespace Zuazo::Utils{

struct ImageAttributes{
	Resolution			res;
	PixelFormat			pixFmt;

	constexpr ImageAttributes() :
		pixFmt(PixelFormats::NONE)
	{
	}

	constexpr ImageAttributes(const Resolution& res, PixelFormat pixFmt) :
		res(res),
		pixFmt(pixFmt)
	{
	}

	ImageAttributes(const ImageAttributes& other)=default;
	~ImageAttributes()=default;

	constexpr operator bool() const{
		return res.operator bool() && pixFmt.fmt != PixelFormats::NONE;
	}

	constexpr int operator==(const ImageAttributes& right)const{
        return res==right.res && pixFmt==right.pixFmt;
    }

	constexpr int operator!=(const ImageAttributes& right)const{
		return res!=right.res || pixFmt!=right.pixFmt;
    }

	size_t getStride(u_int32_t plane) const{
		return av_image_get_linesize(pixFmt.toAVPixelFormat(), res.width, plane);
	}

	void getStrides(int linesizes[4]) const{
	 	av_image_fill_linesizes(
	 			linesizes,
				pixFmt.toAVPixelFormat(),
				res.width
		);
	}

	size_t getPlaneSize(u_int32_t plane) const{
		return getStride(plane) * res.height;
	}

	size_t getSize() const{
	 	return av_image_get_buffer_size(pixFmt.toAVPixelFormat(), res.width, res.height, 1);
	}
};
}

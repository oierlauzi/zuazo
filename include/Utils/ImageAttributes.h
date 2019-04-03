#pragma once

extern "C"{
	#include <libavutil/imgutils.h>
}
#include <stddef.h>
#include <sys/types.h>

#include "PixelFormat.h"
#include "Resolution.h"

namespace Zuazo::Utils{

struct ImageAttributes{
	Resolution			res;
	PixelFormat			pixFmt;

	constexpr ImageAttributes();
	constexpr ImageAttributes(const Resolution& res, PixelFormat pixFmt);
	ImageAttributes(const ImageAttributes& other)=default;
	~ImageAttributes()=default;

	constexpr operator bool() const;
	constexpr int operator==(const ImageAttributes& right)const;
	constexpr int operator!=(const ImageAttributes& right)const;

	size_t getStride(u_int32_t plane) const;
	void getStrides(int linesizes[4]) const;
	size_t getPlaneSize(u_int32_t plane) const;
	size_t getSize() const;
};

constexpr ImageAttributes::ImageAttributes() :
	res(),
	pixFmt(PixelFormats::PIX_FMT_NONE)
{
}

constexpr ImageAttributes::ImageAttributes(const Resolution& res, PixelFormat pixFmt) :
	res(res),
	pixFmt(pixFmt)
{
}

constexpr ImageAttributes::operator bool() const{
	return res.operator bool() && pixFmt != PixelFormats::PIX_FMT_NONE;
}

constexpr int ImageAttributes::operator==(const ImageAttributes& right)const{
	return res==right.res && pixFmt==right.pixFmt;
}

constexpr int ImageAttributes::operator!=(const ImageAttributes& right)const{
	return res!=right.res || pixFmt!=right.pixFmt;
}

inline size_t ImageAttributes::getStride(u_int32_t plane) const{
	return av_image_get_linesize(pixFmt.toAVPixelFormat(), res.width, plane);
}

inline void ImageAttributes::getStrides(int linesizes[4]) const{
 	av_image_fill_linesizes(
 			linesizes,
			pixFmt.toAVPixelFormat(),
			res.width
	);
}

inline size_t ImageAttributes::getPlaneSize(u_int32_t plane) const{
	return getStride(plane) * res.height;
}

inline size_t ImageAttributes::getSize() const{
 	return av_image_get_buffer_size(pixFmt.toAVPixelFormat(), res.width, res.height, 1);
}

}

#pragma once

#include <glad/glad.h>
#include <libavutil/pixfmt.h>
#include <stddef.h>
#include <sys/types.h>
#include <map>

#include "../../Utils/PixelFormats.h"
#include "../../Utils/Resolution.h"

namespace Zuazo::Graphics::GL{

enum class PixelFormats{
	NONE	=GL_NONE,
    RED		=GL_RED,
    GREEN	=GL_GREEN,
    BLUE	=GL_BLUE,
    ALPHA	=GL_ALPHA,
    RGB		=GL_RGB,
    BGR		=GL_BGR,
    RGBA	=GL_RGBA,
    BGRA	=GL_BGRA
};

extern const std::map<PixelFormats, size_t> pixelComponentsMap;
extern const std::map<PixelFormats, Utils::PixelFormats> pixelConversionsMap;

typedef Utils::Resolution Resolution;

struct PixelFormat{
	PixelFormats fmt;

	constexpr PixelFormat() :
		fmt(PixelFormats::NONE)
	{
	}
	constexpr PixelFormat(PixelFormats fmt) :
		fmt(fmt)
	{
	}
	constexpr PixelFormat(const PixelFormat& other) :
		fmt(other.fmt)
	{
	}

	PixelFormat(const Utils::PixelFormat& other) :
		fmt(PixelFormats::NONE)
	{
		for(auto& pixFmt : pixelConversionsMap){
			if(pixFmt.second == other.fmt){
				fmt=pixFmt.first;
				break;
			}
		}
	}
	~PixelFormat()=default;

	constexpr int operator ==(const PixelFormat& other) const{
		return fmt == other.fmt;
	}

	constexpr int operator !=(const PixelFormat& other) const{
		return fmt != other.fmt;
	}

	size_t getPixelComponents() const{
		return pixelComponentsMap.at(fmt);
	}

	Utils::PixelFormat toPixelFormat() const{
		return Utils::PixelFormat(pixelConversionsMap.at(fmt));
	}

	AVPixelFormat toAVPixelFormat() const{
		return toPixelFormat().toAVPixelFormat();
	}

	constexpr GLenum toGLenum() const{
		return (GLenum)fmt;
	}

	constexpr bool hasAlpha() const{
		if(fmt == PixelFormats::BGRA || fmt == PixelFormats::RGBA || fmt == PixelFormats::ALPHA)
			return true;
		else
			return false;
	}
};

struct ImageAttributes{
	Resolution			res;
	PixelFormat			pixFmt;

	ImageAttributes()=default;
	ImageAttributes(const Resolution& res, const PixelFormat& pixFmt):
		res(res),
		pixFmt(pixFmt)
	{
	}
	ImageAttributes(const ImageAttributes& other)=default;
	~ImageAttributes()=default;

	int operator==(const ImageAttributes& other) const{
		return res == other.res && pixFmt == other.pixFmt;
	}

	int operator!=(const ImageAttributes& other) const{
		return res != other.res || pixFmt != other.pixFmt;
	}

	int operator<(const ImageAttributes& other) const{
		return getSize() < other.getSize();
	}

	int operator>(const ImageAttributes& other) const{
		return getSize() > other.getSize();
	}

	int operator<=(const ImageAttributes& other) const{
		return getSize() <= other.getSize();
	}

	int operator>=(const ImageAttributes& other) const{
		return getSize() >= other.getSize();
	}

	size_t getStride() const{
		return pixFmt.getPixelComponents() * res.width;
	}

	size_t getSize() const{
		return getStride() * res.height;
	}
};

struct ImageBuffer{
	ImageAttributes		att;
	u_int8_t*			data;

	constexpr ImageBuffer() :
		data(nullptr)
	{
	}

	constexpr ImageBuffer(const ImageAttributes& att) :
		att(att),
		data(nullptr)
	{
	}

	constexpr ImageBuffer(const ImageBuffer& img) :
		att(img.att),
		data(img.data)
	{
	}

	~ImageBuffer()=default;
};
}

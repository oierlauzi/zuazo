#pragma once

#include "PixelFormat.h"

#include <glad/glad.h>
#include <stddef.h>
#include <sys/types.h>
#include <utility>

extern "C"{
	#include <libavutil/pixfmt.h>
}

#include "../Utils/PixelFormat.h"


namespace Zuazo::Graphics{

class PixelFormat{
public:
	constexpr PixelFormat();
	constexpr PixelFormat(GLenum fmt);
	constexpr PixelFormat(const Utils::PixelFormat& other);
	PixelFormat(const PixelFormat& other)=default;
	~PixelFormat()=default;

	constexpr int 					operator==(const PixelFormat& other) const;
	constexpr int 					operator!=(const PixelFormat& other) const;

	constexpr Utils::PixelFormat 	toPixelFormat() const;
	constexpr GLenum 				toGLenum() const;
	constexpr AVPixelFormat			toAVPixelFormat() const;

	constexpr size_t 				getPixelComponents() const;
	constexpr bool 					hasAlpha() const;
private:
	GLenum fmt;
};

constexpr PixelFormat::PixelFormat() :
			fmt(GL_NONE)
{
}
constexpr PixelFormat::PixelFormat(GLenum fmt) :
			fmt(fmt)
{
}

constexpr int PixelFormat::operator==(const PixelFormat& other) const{
	return fmt == other.fmt;
}

constexpr int PixelFormat::operator!=(const PixelFormat& other) const{
	return fmt != other.fmt;
}

constexpr GLenum PixelFormat::toGLenum() const{
	return fmt;
}

constexpr AVPixelFormat PixelFormat::toAVPixelFormat() const{
	return toPixelFormat().toAVPixelFormat();
}

namespace PixelFormats{

constexpr PixelFormat NONE		=GL_NONE;
constexpr PixelFormat RED		=GL_RED;
constexpr PixelFormat GREEN		=GL_GREEN;
constexpr PixelFormat BLUE		=GL_BLUE;
constexpr PixelFormat ALPHA		=GL_ALPHA;
constexpr PixelFormat RGB		=GL_RGB;
constexpr PixelFormat BGR		=GL_BGR;
constexpr PixelFormat RGBA		=GL_RGBA;
constexpr PixelFormat BGRA		=GL_BGRA;

}

constexpr std::pair<PixelFormat, size_t> pixelComponents[]={
		{PixelFormats::NONE,	0	},
		{PixelFormats::RED,		1	},
		{PixelFormats::GREEN,	1	},
		{PixelFormats::BLUE,	1	},
		{PixelFormats::ALPHA,	1	},
		{PixelFormats::RGB,		3	},
		{PixelFormats::BGR,		3	},
		{PixelFormats::RGBA,	4	},
		{PixelFormats::BGRA,	4	},
};

constexpr std::pair<PixelFormat, Utils::PixelFormat> pixelConversions[]={
		{PixelFormats::NONE,	Utils::PixelFormats::NONE		},
		{PixelFormats::RED,		Utils::PixelFormats::GRAY8		},
		{PixelFormats::GREEN,	Utils::PixelFormats::GRAY8		},
		{PixelFormats::BLUE,	Utils::PixelFormats::GRAY8		},
		{PixelFormats::ALPHA,	Utils::PixelFormats::GRAY8		},
		{PixelFormats::RGB,		Utils::PixelFormats::RGB24		},
		{PixelFormats::BGR,		Utils::PixelFormats::BGR24		},
		{PixelFormats::RGBA,	Utils::PixelFormats::RGBA		},
		{PixelFormats::BGRA,	Utils::PixelFormats::BGRA		},
};

constexpr PixelFormat::PixelFormat(const Utils::PixelFormat& other) :
			PixelFormat()
{
	for(u_int32_t i=0; i<sizeof(pixelConversions); i++){
		if(pixelConversions[i].second == other){
			fmt=pixelConversions[i].first.toGLenum();
			break;
		}
	}
}

constexpr Utils::PixelFormat PixelFormat::toPixelFormat() const{
	Utils::PixelFormat result=Utils::PixelFormats::NONE;

	for(u_int32_t i=0; i<sizeof(pixelConversions); i++){
		if(pixelConversions[i].first == *this){
			result=pixelConversions[i].second;
			break;
		}
	}

	return result;
}

constexpr size_t PixelFormat::getPixelComponents() const{
	size_t result=0;

	for(u_int32_t i=0; i<sizeof(pixelComponents); i++){
		if(pixelComponents[i].first == *this){
			result=pixelComponents[i].second;
			break;
		}
	}

	return result;
}

constexpr bool PixelFormat::hasAlpha() const{
	if(*this == PixelFormats::BGRA || *this == PixelFormats::RGBA || *this == PixelFormats::ALPHA)
		return true;
	else
		return false;
}


}

#pragma once

#include "PixelFormat.h"

#include <glad/glad.h>
#include <stddef.h>
#include <sys/types.h>
#include <utility>

extern "C"{
	#include <libavutil/pixfmt.h>
}

#include "../Utils/PixelFormats.h"


namespace Zuazo::Graphics{

class PixelFormat{
public:
	constexpr PixelFormat() :
		fmt(GL_NONE)
	{
	}
	constexpr PixelFormat(GLenum fmt) :
		fmt(fmt)
	{
	}
	constexpr PixelFormat(const PixelFormat& other) :
		fmt(other.fmt)
	{
	}

	constexpr PixelFormat(const Utils::PixelFormat& other) :
		fmt(PixelFormat::NONE.toGLenum())
	{
		for(u_int32_t i=0; i<sizeof(pixelConversions); i++){
			if(pixelConversions[i].second == other){
				fmt=pixelConversions[i].first.toGLenum();
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

	constexpr size_t getPixelComponents() const{
		size_t result=0;

		for(u_int32_t i=0; i<sizeof(pixelComponents); i++){
			if(pixelComponents[i].first == *this){
				result=pixelComponents[i].second;
				break;
			}
		}

		return result;
	}

	constexpr Utils::PixelFormat toPixelFormat() const{
		Utils::PixelFormat result=Utils::PixelFormat::NONE;

		for(u_int32_t i=0; i<sizeof(pixelConversions); i++){
			if(pixelConversions[i].first == *this){
				result=pixelConversions[i].second;
				break;
			}
		}

		return result;
	}

	constexpr AVPixelFormat toAVPixelFormat() const{
		return toPixelFormat().toAVPixelFormat();
	}

	constexpr GLenum toGLenum() const{
		return fmt;
	}

	constexpr bool hasAlpha() const{
		if(*this == PixelFormat::BGRA || *this == PixelFormat::RGBA || *this == PixelFormat::ALPHA)
			return true;
		else
			return false;
	}

	static const PixelFormat NONE;
	static const PixelFormat RED;
	static const PixelFormat GREEN;
	static const PixelFormat BLUE;
	static const PixelFormat ALPHA;
	static const PixelFormat RGB;
	static const PixelFormat BGR;
	static const PixelFormat RGBA;
	static const PixelFormat BGRA;
private:
	GLenum fmt;

	static const std::pair<PixelFormat, size_t> pixelComponents[9];
	static const std::pair<PixelFormat, Utils::PixelFormat> pixelConversions[9];
};


constexpr const PixelFormat PixelFormat::NONE		=GL_NONE;
constexpr const PixelFormat PixelFormat::RED		=GL_RED;
constexpr const PixelFormat PixelFormat::GREEN		=GL_GREEN;
constexpr const PixelFormat PixelFormat::BLUE		=GL_BLUE;
constexpr const PixelFormat PixelFormat::ALPHA		=GL_ALPHA;
constexpr const PixelFormat PixelFormat::RGB		=GL_RGB;
constexpr const PixelFormat PixelFormat::BGR		=GL_BGR;
constexpr const PixelFormat PixelFormat::RGBA		=GL_RGBA;
constexpr const PixelFormat PixelFormat::BGRA		=GL_BGRA;

constexpr const std::pair<PixelFormat, size_t> PixelFormat::pixelComponents[9]={
		{PixelFormat::NONE,		0	},
		{PixelFormat::RED,		1	},
		{PixelFormat::GREEN,	1	},
		{PixelFormat::BLUE,		1	},
		{PixelFormat::ALPHA,	1	},
		{PixelFormat::RGB,		3	},
		{PixelFormat::BGR,		3	},
		{PixelFormat::RGBA,		4	},
		{PixelFormat::BGRA,		4	},
};

constexpr const std::pair<PixelFormat, Utils::PixelFormat> PixelFormat::pixelConversions[9]={
		{PixelFormat::NONE,		Utils::PixelFormat::NONE		},
		{PixelFormat::RED,		Utils::PixelFormat::GRAY8		},
		{PixelFormat::GREEN,	Utils::PixelFormat::GRAY8		},
		{PixelFormat::BLUE,		Utils::PixelFormat::GRAY8		},
		{PixelFormat::ALPHA,	Utils::PixelFormat::GRAY8		},
		{PixelFormat::RGB,		Utils::PixelFormat::RGB24		},
		{PixelFormat::BGR,		Utils::PixelFormat::BGR24		},
		{PixelFormat::RGBA,		Utils::PixelFormat::RGBA		},
		{PixelFormat::BGRA,		Utils::PixelFormat::BGRA		},
};

}

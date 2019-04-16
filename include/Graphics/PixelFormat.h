#pragma once

#include "GL/Types.h"
#include "../glad/glad.h"
#include "../Utils/PixelFormat.h"

#include <stddef.h>
#include <sys/types.h>
#include <utility>
#include <set>

extern "C"{
	#include <libavutil/pixfmt.h>
}


namespace Zuazo::Graphics{

class PixelFormat{
public:
	constexpr PixelFormat();
	constexpr PixelFormat(GLenum fmt, GLenum type);
	constexpr PixelFormat(const Utils::PixelFormat& other);
	PixelFormat(const PixelFormat& other)=default;
	~PixelFormat()=default;

	constexpr int 					operator==(const PixelFormat& other) const;
	constexpr int 					operator!=(const PixelFormat& other) const;
	constexpr 						operator bool() const;

	constexpr Utils::PixelFormat 	toPixelFormat() const;
	constexpr GLenum 				getType() const;
	constexpr GLenum 				getFormat() const;
	constexpr AVPixelFormat			toAVPixelFormat() const;

	constexpr size_t 				getPixelComponents() const;
	constexpr size_t 				getComponentSize() const;
	constexpr size_t 				getSize() const;
	constexpr bool 					hasAlpha() const;

	static std::set<Utils::PixelFormat> getSupportedPixelFormats();
private:
	GLenum fmt;
	GLenum type;
};

constexpr PixelFormat::PixelFormat() :
	fmt(GL_NONE),
	type(GL_NONE)
{
}
constexpr PixelFormat::PixelFormat(GLenum fmt, GLenum type) :
	fmt(fmt),
	type(type)
{
}

constexpr int PixelFormat::operator==(const PixelFormat& other) const{
	return type == other.type && fmt == other.fmt;
}

constexpr int PixelFormat::operator!=(const PixelFormat& other) const{
	return type != other.type || fmt != other.fmt;
}

constexpr PixelFormat::operator bool() const{
	return fmt != GL_NONE && type != GL_NONE;
}

constexpr GLenum PixelFormat::getFormat() const{
	return fmt;
}

constexpr GLenum PixelFormat::getType() const{
	return type;
}

constexpr AVPixelFormat PixelFormat::toAVPixelFormat() const{
	return toPixelFormat().toAVPixelFormat();
}

namespace PixelFormats{

constexpr PixelFormat NONE		=PixelFormat(GL_NONE, GL_NONE);

constexpr PixelFormat RED8		=PixelFormat(GL_RED, 	GL::GLType<u_int8_t>);
constexpr PixelFormat GREEN8	=PixelFormat(GL_GREEN, 	GL::GLType<u_int8_t>);
constexpr PixelFormat BLUE8		=PixelFormat(GL_BLUE, 	GL::GLType<u_int8_t>);
constexpr PixelFormat ALPHA8	=PixelFormat(GL_ALPHA, 	GL::GLType<u_int8_t>);
constexpr PixelFormat RGB24		=PixelFormat(GL_RGB, 	GL::GLType<u_int8_t>);
constexpr PixelFormat BGR24		=PixelFormat(GL_BGR, 	GL::GLType<u_int8_t>);
constexpr PixelFormat RGBA32	=PixelFormat(GL_RGBA, 	GL::GLType<u_int8_t>);
constexpr PixelFormat BGRA32	=PixelFormat(GL_BGRA, 	GL::GLType<u_int8_t>);

constexpr PixelFormat RED16		=PixelFormat(GL_RED, 	GL::GLType<u_int16_t>);
constexpr PixelFormat GREEN16	=PixelFormat(GL_GREEN, 	GL::GLType<u_int16_t>);
constexpr PixelFormat BLUE16	=PixelFormat(GL_BLUE, 	GL::GLType<u_int16_t>);
constexpr PixelFormat ALPHA16	=PixelFormat(GL_ALPHA, 	GL::GLType<u_int16_t>);
constexpr PixelFormat RGB48		=PixelFormat(GL_RGB, 	GL::GLType<u_int16_t>);
constexpr PixelFormat BGR48		=PixelFormat(GL_BGR, 	GL::GLType<u_int16_t>);
constexpr PixelFormat RGBA64	=PixelFormat(GL_RGBA, 	GL::GLType<u_int16_t>);
constexpr PixelFormat BGRA64	=PixelFormat(GL_BGRA, 	GL::GLType<u_int16_t>);

constexpr PixelFormat RED32		=PixelFormat(GL_RED, 	GL::GLType<u_int32_t>);
constexpr PixelFormat GREEN32	=PixelFormat(GL_GREEN, 	GL::GLType<u_int32_t>);
constexpr PixelFormat BLUE32	=PixelFormat(GL_BLUE, 	GL::GLType<u_int32_t>);
constexpr PixelFormat ALPHA32	=PixelFormat(GL_ALPHA, 	GL::GLType<u_int32_t>);
constexpr PixelFormat RGB96		=PixelFormat(GL_RGB, 	GL::GLType<u_int32_t>);
constexpr PixelFormat BGR96		=PixelFormat(GL_BGR, 	GL::GLType<u_int32_t>);
constexpr PixelFormat RGBA128	=PixelFormat(GL_RGBA, 	GL::GLType<u_int32_t>);
constexpr PixelFormat BGRA128	=PixelFormat(GL_BGRA, 	GL::GLType<u_int32_t>);

constexpr PixelFormat RED32f	=PixelFormat(GL_RED, 	GL::GLType<float>);
constexpr PixelFormat GREEN32f	=PixelFormat(GL_GREEN, 	GL::GLType<float>);
constexpr PixelFormat BLUE32f	=PixelFormat(GL_BLUE, 	GL::GLType<float>);
constexpr PixelFormat ALPHA32f	=PixelFormat(GL_ALPHA, 	GL::GLType<float>);
constexpr PixelFormat RGB96f	=PixelFormat(GL_RGB, 	GL::GLType<float>);
constexpr PixelFormat BGR96f	=PixelFormat(GL_BGR, 	GL::GLType<float>);
constexpr PixelFormat RGBA128f	=PixelFormat(GL_RGBA, 	GL::GLType<float>);
constexpr PixelFormat BGRA128f	=PixelFormat(GL_BGRA, 	GL::GLType<float>);

}

constexpr Utils::PixelFormat compatiblePixelFormats[]={
#ifdef PIX_FMT_GRAY8_SUPPORT
		Utils::PixelFormats::PIX_FMT_GRAY8,
#endif
#ifdef PIX_FMT_RGB24_SUPPORT
		Utils::PixelFormats::PIX_FMT_RGB24,
#endif
#ifdef PIX_FMT_BGR24_SUPPORT
		Utils::PixelFormats::PIX_FMT_BGR24,
#endif
#ifdef PIX_FMT_RGB32_SUPPORT
		Utils::PixelFormats::PIX_FMT_RGB32,
#endif
#ifdef PIX_FMT_BGR32_SUPPORT
		Utils::PixelFormats::PIX_FMT_BGR32,
#endif

#ifdef PIX_FMT_GRAY16_SUPPORT
		Utils::PixelFormats::PIX_FMT_GRAY16,
#endif
#ifdef PIX_FMT_RGB48_SUPPORT
		Utils::PixelFormats::PIX_FMT_RGB48,
#endif
#ifdef PIX_FMT_BGR48_SUPPORT
		Utils::PixelFormats::PIX_FMT_BGR48,
#endif
#ifdef PIX_FMT_RGBA64_SUPPORT
		Utils::PixelFormats::PIX_FMT_RGBA64,
#endif
#ifdef PIX_FMT_BGRA64_SUPPORT
		Utils::PixelFormats::PIX_FMT_BGRA64,
#endif

#ifdef PIX_FMT_GRAYF32_SUPPORT
		Utils::PixelFormats::PIX_FMT_GRAYF32,
#endif

		Utils::PixelFormats::PIX_FMT_NONE
};

constexpr std::pair<PixelFormat, Utils::PixelFormat> pixelConversions[]={
#ifdef PIX_FMT_GRAY8_SUPPORT
		{PixelFormats::RED8,	Utils::PixelFormats::PIX_FMT_GRAY8		},
		{PixelFormats::GREEN8,	Utils::PixelFormats::PIX_FMT_GRAY8		},
		{PixelFormats::BLUE8,	Utils::PixelFormats::PIX_FMT_GRAY8		},
		{PixelFormats::ALPHA8,	Utils::PixelFormats::PIX_FMT_GRAY8		},
#endif
#ifdef PIX_FMT_RGB24_SUPPORT
		{PixelFormats::RGB24,	Utils::PixelFormats::PIX_FMT_RGB24		},
#endif
#ifdef PIX_FMT_BGR24_SUPPORT
		{PixelFormats::BGR24,	Utils::PixelFormats::PIX_FMT_BGR24		},
#endif
#ifdef PIX_FMT_RGB32_SUPPORT
		{PixelFormats::RGBA32,	Utils::PixelFormats::PIX_FMT_RGB32		},
#endif
#ifdef PIX_FMT_BGR32_SUPPORT
		{PixelFormats::BGRA32,	Utils::PixelFormats::PIX_FMT_BGR32		},
#endif

#ifdef PIX_FMT_GRAY16_SUPPORT
		{PixelFormats::RED16,	Utils::PixelFormats::PIX_FMT_GRAY16		},
		{PixelFormats::GREEN16,	Utils::PixelFormats::PIX_FMT_GRAY16		},
		{PixelFormats::BLUE16,	Utils::PixelFormats::PIX_FMT_GRAY16		},
		{PixelFormats::ALPHA16,	Utils::PixelFormats::PIX_FMT_GRAY16		},
#endif
#ifdef PIX_FMT_RGB48_SUPPORT
		{PixelFormats::RGB48,	Utils::PixelFormats::PIX_FMT_RGB48		},
#endif
#ifdef PIX_FMT_BGR48_SUPPORT
		{PixelFormats::BGR48,	Utils::PixelFormats::PIX_FMT_BGR48		},
#endif
#ifdef PIX_FMT_RGBA64_SUPPORT
		{PixelFormats::RGBA64,	Utils::PixelFormats::PIX_FMT_RGBA64		},
#endif
#ifdef PIX_FMT_BGRA64_SUPPORT
		{PixelFormats::BGRA64,	Utils::PixelFormats::PIX_FMT_BGRA64		},
#endif

#ifdef PIX_FMT_GRAYF32_SUPPORT
		{PixelFormats::RED32f,	Utils::PixelFormats::PIX_FMT_GRAYF32	},
		{PixelFormats::GREEN32f,Utils::PixelFormats::PIX_FMT_GRAYF32	},
		{PixelFormats::BLUE32f,	Utils::PixelFormats::PIX_FMT_GRAYF32	},
		{PixelFormats::ALPHA32f,Utils::PixelFormats::PIX_FMT_GRAYF32	},
#endif

		{PixelFormats::NONE,	Utils::PixelFormats::PIX_FMT_NONE		},
};

constexpr PixelFormat::PixelFormat(const Utils::PixelFormat& other) :
			PixelFormat()
{
	for(u_int32_t i=0; pixelConversions[i].first; i++){
		if(pixelConversions[i].second == other){
			fmt=pixelConversions[i].first.getFormat();
			type=pixelConversions[i].first.getType();
			break;
		}
	}
}

constexpr Utils::PixelFormat PixelFormat::toPixelFormat() const{
	Utils::PixelFormat result=Utils::PixelFormats::PIX_FMT_NONE;

	for(u_int32_t i=0; pixelConversions[i].first; i++){
		if(pixelConversions[i].first == *this){
			result=pixelConversions[i].second;
			break;
		}
	}

	return result;
}

constexpr size_t PixelFormat::getPixelComponents() const{
	size_t result=0;

	const std::pair<GLenum, size_t> pixelComponents[]={
			{GL_RED,	1	},
			{GL_GREEN,	1	},
			{GL_BLUE,	1	},
			{GL_ALPHA,	1	},
			{GL_RGB,	3	},
			{GL_BGR,	3	},
			{GL_RGBA,	4	},
			{GL_BGRA,	4	},
			{GL_NONE,	0	},
	};

	for(u_int32_t i=0; pixelComponents[i].first; i++){
		if(pixelComponents[i].first == fmt){
			result=pixelComponents[i].second;
			break;
		}
	}

	return result;
}

constexpr size_t PixelFormat::getComponentSize() const{
	size_t result=0;

	const std::pair<GLenum, size_t> componentSizes[]={
			{GL::GLType<u_int8_t>,		sizeof(u_int8_t)	},
			{GL::GLType<u_int16_t>,		sizeof(u_int16_t)	},
			{GL::GLType<u_int32_t>,		sizeof(u_int32_t)	},
			{GL::GLType<float>,			sizeof(float)		},
			{GL::GLType<double>,		sizeof(double)		},
			//TODO non standard types
			{GL_NONE,					0					},
	};

	for(u_int32_t i=0; componentSizes[i].first; i++){
		if(componentSizes[i].first == type){
			result=componentSizes[i].second;
			break;
		}
	}

	return result;
}

constexpr size_t PixelFormat::getSize() const{
	return getComponentSize() * getPixelComponents();
}

constexpr bool PixelFormat::hasAlpha() const{
	if(fmt == GL_BGRA || fmt == GL_RGBA || fmt == GL_ALPHA)
		return true;
	else
		return false;
}

inline std::set<Utils::PixelFormat> PixelFormat::getSupportedPixelFormats(){
	std::set<Utils::PixelFormat> pixFmts;

	for(u_int32_t i=0; compatiblePixelFormats[i]; i++){
		pixFmts.insert(compatiblePixelFormats[i]);
	}
	return pixFmts;
}

}

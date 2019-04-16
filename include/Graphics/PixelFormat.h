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

#define SWIZZLE_MASK_COMP_ZERO	0x00
#define SWIZZLE_MASK_COMP_ONE	0x01
#define SWIZZLE_MASK_COMP_R 	0x02
#define SWIZZLE_MASK_COMP_G 	0x03
#define SWIZZLE_MASK_COMP_B 	0x04
#define SWIZZLE_MASK_COMP_A 	0x05

#define SWIZZLE_MASK(r, g, b, a) ( (r) << (3*8) | (g) << (2*8) | (b) << (1*8) | (a) << (0*8) )
#define SWIZZLE_MASK_POS(x, pos) ( ((x) >> (8*pos)) & 0xff )
#define MASK_COMP2GL(x) ((x) == SWIZZLE_MASK_COMP_ZERO ? GL_ZERO : ( (x) == SWIZZLE_MASK_COMP_ONE ? GL_ONE : (x) - SWIZZLE_MASK_COMP_R + GL_RED) )
#define GL2MASK_COMP(x) ((x) == GL_ZERO ? SWIZZLE_MASK_COMP_ZERO : ( (x) == GL_ONE ? SWIZZLE_MASK_COMP_ONE : (x) - GL_RED + SWIZZLE_MASK_COMP_R) )

#define SWIZZLE_MASK_NONE 		SWIZZLE_MASK(SWIZZLE_MASK_COMP_ZERO, SWIZZLE_MASK_COMP_ZERO, SWIZZLE_MASK_COMP_ZERO, SWIZZLE_MASK_COMP_ZERO)

#define SWIZZLE_MASK_RGBA 		SWIZZLE_MASK(SWIZZLE_MASK_COMP_R, SWIZZLE_MASK_COMP_G, SWIZZLE_MASK_COMP_B, SWIZZLE_MASK_COMP_A)
#define SWIZZLE_MASK_ARGB 		SWIZZLE_MASK(SWIZZLE_MASK_COMP_A, SWIZZLE_MASK_COMP_R, SWIZZLE_MASK_COMP_G, SWIZZLE_MASK_COMP_B)
#define SWIZZLE_MASK_BGRA 		SWIZZLE_MASK(SWIZZLE_MASK_COMP_B, SWIZZLE_MASK_COMP_G, SWIZZLE_MASK_COMP_R, SWIZZLE_MASK_COMP_A)
#define SWIZZLE_MASK_ABGR 		SWIZZLE_MASK(SWIZZLE_MASK_COMP_A, SWIZZLE_MASK_COMP_B, SWIZZLE_MASK_COMP_G, SWIZZLE_MASK_COMP_R)
#define SWIZZLE_MASK_GBAR		SWIZZLE_MASK(SWIZZLE_MASK_COMP_G, SWIZZLE_MASK_COMP_B, SWIZZLE_MASK_COMP_A, SWIZZLE_MASK_COMP_R)
#define SWIZZLE_MASK_GRAB		SWIZZLE_MASK(SWIZZLE_MASK_COMP_G, SWIZZLE_MASK_COMP_R, SWIZZLE_MASK_COMP_A, SWIZZLE_MASK_COMP_B)
#define SWIZZLE_MASK_R2Y		SWIZZLE_MASK(SWIZZLE_MASK_COMP_R, SWIZZLE_MASK_COMP_R, SWIZZLE_MASK_COMP_R, SWIZZLE_MASK_COMP_A) //Red to luminance
#define SWIZZLE_MASK_RG2YA		SWIZZLE_MASK(SWIZZLE_MASK_COMP_R, SWIZZLE_MASK_COMP_R, SWIZZLE_MASK_COMP_R, SWIZZLE_MASK_COMP_G) //Red and green to luminance and alpha

#define SWIZZLE_MASK_POS_R(x) 	SWIZZLE_MASK_POS(x, 3)
#define SWIZZLE_MASK_POS_G(x) 	SWIZZLE_MASK_POS(x, 2)
#define SWIZZLE_MASK_POS_B(x) 	SWIZZLE_MASK_POS(x, 1)
#define SWIZZLE_MASK_POS_A(x) 	SWIZZLE_MASK_POS(x, 0)

class PixelFormat{
public:
	constexpr PixelFormat();
	constexpr PixelFormat(GLenum fmt, GLenum type, u_int32_t swizzleMask);
	constexpr PixelFormat(const Utils::PixelFormat& other);
	PixelFormat(const PixelFormat& other)=default;
	~PixelFormat()=default;

	constexpr int 					operator==(const PixelFormat& other) const;
	constexpr int 					operator!=(const PixelFormat& other) const;
	constexpr 						operator bool() const;

	constexpr Utils::PixelFormat 	toPixelFormat() const;
	constexpr GLenum 				getType() const;
	constexpr GLenum 				getFormat() const;
	constexpr u_int32_t 			getSwizzleMask() const;
	const GLint*					getSwizzleMaskComp() const;
	constexpr AVPixelFormat			toAVPixelFormat() const;

	constexpr size_t 				getPixelComponents() const;
	constexpr size_t 				getComponentSize() const;
	constexpr size_t 				getSize() const;
	constexpr bool 					hasAlpha() const;

	static std::set<Utils::PixelFormat> getSupportedPixelFormats();
private:
	GLenum 							fmt;
	GLenum 							type;
	u_int32_t						swizzleMask;
	GLint							swizzleMaskComp[4];
};

constexpr PixelFormat::PixelFormat() :
	fmt(GL_NONE),
	type(GL_NONE),
	swizzleMask(SWIZZLE_MASK_NONE),
	swizzleMaskComp{
		GL_ZERO,
		GL_ZERO,
		GL_ZERO,
		GL_ZERO
	}
{
}
constexpr PixelFormat::PixelFormat(GLenum fmt, GLenum type, u_int32_t swizzleMask) :
	fmt(fmt),
	type(type),
	swizzleMask(swizzleMask),
	swizzleMaskComp{
		(GLint)MASK_COMP2GL(SWIZZLE_MASK_POS_R(swizzleMask)),
		(GLint)MASK_COMP2GL(SWIZZLE_MASK_POS_G(swizzleMask)),
		(GLint)MASK_COMP2GL(SWIZZLE_MASK_POS_B(swizzleMask)),
		(GLint)MASK_COMP2GL(SWIZZLE_MASK_POS_A(swizzleMask))
	}
{

}

constexpr int PixelFormat::operator==(const PixelFormat& other) const{
	return type == other.type && fmt == other.fmt && swizzleMask == other.swizzleMask;
}

constexpr int PixelFormat::operator!=(const PixelFormat& other) const{
	return type != other.type || fmt != other.fmt || swizzleMask != other.swizzleMask;
}

constexpr PixelFormat::operator bool() const{
	return fmt != GL_NONE && type != GL_NONE && swizzleMask != SWIZZLE_MASK_NONE;
}

constexpr GLenum PixelFormat::getFormat() const{
	return fmt;
}

constexpr GLenum PixelFormat::getType() const{
	return type;
}

constexpr u_int32_t PixelFormat::getSwizzleMask() const{
	return swizzleMask;
}

inline const GLint* PixelFormat::getSwizzleMaskComp() const{
	return swizzleMaskComp;
}

constexpr AVPixelFormat PixelFormat::toAVPixelFormat() const{
	return toPixelFormat().toAVPixelFormat();
}

namespace PixelFormats{

constexpr PixelFormat NONE			=PixelFormat();

constexpr PixelFormat RED8			=PixelFormat(GL_RED, 	GL::GLType<u_int8_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat GREEN8		=PixelFormat(GL_GREEN, 	GL::GLType<u_int8_t>, 	SWIZZLE_MASK_RGBA);
constexpr PixelFormat BLUE8			=PixelFormat(GL_BLUE, 	GL::GLType<u_int8_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat ALPHA8		=PixelFormat(GL_ALPHA, 	GL::GLType<u_int8_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat LUMMA8		=PixelFormat(GL_RED, 	GL::GLType<u_int8_t>,	SWIZZLE_MASK_R2Y);
constexpr PixelFormat LUMMA_ALPHA8	=PixelFormat(GL_RG, 	GL::GLType<u_int8_t>,	SWIZZLE_MASK_RG2YA);
constexpr PixelFormat RGB24			=PixelFormat(GL_RGB, 	GL::GLType<u_int8_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat BGR24			=PixelFormat(GL_RGB, 	GL::GLType<u_int8_t>,	SWIZZLE_MASK_BGRA);
constexpr PixelFormat RGBA32		=PixelFormat(GL_RGBA, 	GL::GLType<u_int8_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat BGRA32		=PixelFormat(GL_RGBA, 	GL::GLType<u_int8_t>,	SWIZZLE_MASK_BGRA);
constexpr PixelFormat ARGB32		=PixelFormat(GL_RGBA, 	GL::GLType<u_int8_t>,	SWIZZLE_MASK_GBAR);
constexpr PixelFormat ABGR32		=PixelFormat(GL_RGBA, 	GL::GLType<u_int8_t>,	SWIZZLE_MASK_ABGR);

constexpr PixelFormat RED16			=PixelFormat(GL_RED, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat GREEN16		=PixelFormat(GL_GREEN, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat BLUE16		=PixelFormat(GL_BLUE, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat ALPHA16		=PixelFormat(GL_ALPHA, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat LUMMA16		=PixelFormat(GL_RED, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_R2Y);
constexpr PixelFormat LUMMA_ALPHA16	=PixelFormat(GL_RG, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_RG2YA);
constexpr PixelFormat RGB48			=PixelFormat(GL_RGB, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat BGR48			=PixelFormat(GL_RGB, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_BGRA);
constexpr PixelFormat RGBA64		=PixelFormat(GL_RGBA, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat BGRA64		=PixelFormat(GL_RGBA, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_BGRA);
constexpr PixelFormat ARGB64		=PixelFormat(GL_RGBA, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_GBAR);
constexpr PixelFormat ABGR64		=PixelFormat(GL_RGBA, 	GL::GLType<u_int16_t>,	SWIZZLE_MASK_ABGR);

constexpr PixelFormat RED32			=PixelFormat(GL_RED, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat GREEN32		=PixelFormat(GL_GREEN, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat BLUE32		=PixelFormat(GL_BLUE, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat ALPHA32		=PixelFormat(GL_ALPHA, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat LUMMA32		=PixelFormat(GL_RED, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_R2Y);
constexpr PixelFormat LUMMA_ALPHA32	=PixelFormat(GL_RG, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_RG2YA);
constexpr PixelFormat RGB96			=PixelFormat(GL_RGB, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat BGR96			=PixelFormat(GL_RGB, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_BGRA);
constexpr PixelFormat RGBA128		=PixelFormat(GL_RGBA, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_RGBA);
constexpr PixelFormat BGRA128		=PixelFormat(GL_RGBA, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_BGRA);
constexpr PixelFormat ARGB128		=PixelFormat(GL_RGBA, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_GBAR);
constexpr PixelFormat ABGR128		=PixelFormat(GL_RGBA, 	GL::GLType<u_int32_t>,	SWIZZLE_MASK_ABGR);

constexpr PixelFormat RED32f		=PixelFormat(GL_RED, 	GL::GLType<float>,		SWIZZLE_MASK_RGBA);
constexpr PixelFormat GREEN32f		=PixelFormat(GL_GREEN, 	GL::GLType<float>,		SWIZZLE_MASK_RGBA);
constexpr PixelFormat BLUE32f		=PixelFormat(GL_BLUE, 	GL::GLType<float>,		SWIZZLE_MASK_RGBA);
constexpr PixelFormat ALPHA32f		=PixelFormat(GL_ALPHA, 	GL::GLType<float>,		SWIZZLE_MASK_RGBA);
constexpr PixelFormat LUMMA32f		=PixelFormat(GL_RED, 	GL::GLType<float>,		SWIZZLE_MASK_R2Y);
constexpr PixelFormat LUMMA_ALPHA32f=PixelFormat(GL_RG, 	GL::GLType<float>,		SWIZZLE_MASK_RG2YA);
constexpr PixelFormat RGB96f		=PixelFormat(GL_RGB, 	GL::GLType<float>,		SWIZZLE_MASK_RGBA);
constexpr PixelFormat BGR96f		=PixelFormat(GL_RGB, 	GL::GLType<float>,		SWIZZLE_MASK_BGRA);
constexpr PixelFormat RGBA128f		=PixelFormat(GL_RGBA, 	GL::GLType<float>,		SWIZZLE_MASK_RGBA);
constexpr PixelFormat BGRA128f		=PixelFormat(GL_RGBA, 	GL::GLType<float>,		SWIZZLE_MASK_BGRA);
constexpr PixelFormat ARGB128f		=PixelFormat(GL_RGBA, 	GL::GLType<float>,		SWIZZLE_MASK_GBAR);
constexpr PixelFormat ABGR128f		=PixelFormat(GL_RGBA, 	GL::GLType<float>,		SWIZZLE_MASK_ABGR);

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
#ifdef PIX_FMT_RGB32_1_SUPPORT
		Utils::PixelFormats::PIX_FMT_RGB32_1,
#endif
#ifdef PIX_FMT_BGR32_1_SUPPORT
		Utils::PixelFormats::PIX_FMT_BGR32_1,
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
		{PixelFormats::LUMMA8,	Utils::PixelFormats::PIX_FMT_GRAY8		},
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
#ifdef PIX_FMT_RGB32_1_SUPPORT
		{PixelFormats::ARGB32,	Utils::PixelFormats::PIX_FMT_RGB32_1	},
#endif
#ifdef PIX_FMT_BGR32_1_SUPPORT
		{PixelFormats::ABGR32,	Utils::PixelFormats::PIX_FMT_BGR32_1	},
#endif

#ifdef PIX_FMT_GRAY16_SUPPORT
		{PixelFormats::LUMMA16,	Utils::PixelFormats::PIX_FMT_GRAY16		},
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
		{PixelFormats::LUMMA32f,Utils::PixelFormats::PIX_FMT_GRAYF32	},
#endif

		{PixelFormats::NONE,	Utils::PixelFormats::PIX_FMT_NONE		},
};

constexpr PixelFormat::PixelFormat(const Utils::PixelFormat& other) :
			PixelFormat()
{
	for(u_int32_t i=0; pixelConversions[i].first; i++){
		if(pixelConversions[i].second == other){
			*this=pixelConversions[i].first;
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
			{GL_RG,		2	},
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

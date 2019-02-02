#pragma once

#include <GL/glew.h>
#include <sys/types.h>

#include "../../Utils/Resolution.h"

namespace Zuazo::Graphics::GL{

enum class PixelType{
	RGB		=1<<0,
	A		=1<<1,
	RGBA	=RGB | A,
};

struct PixelBuffer{
	Utils::Resolution	res;
	PixelType			pixelType;
	const u_int8_t *	data;
};

class PixelUnpackBuffer{
public:
	PixelUnpackBuffer();
	PixelUnpackBuffer(const PixelUnpackBuffer& other);
	~PixelUnpackBuffer();
private:
	GLuint				m_pbo;
};

class PixelackBuffer{
public:

private:

};
}
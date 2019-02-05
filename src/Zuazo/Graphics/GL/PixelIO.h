#pragma once

#include <GL/glew.h>
#include <sys/types.h>

#include "../../Utils/Resolution.h"
#include "PixelTypes.h"

namespace Zuazo::Graphics::GL{

struct PixelBuffer{
	Utils::Resolution	res;
	const u_int8_t *	data;
};

class PixelUnpackBuffer{
public:
	PixelUnpackBuffer();
	PixelUnpackBuffer(const PixelUnpackBuffer& other);
	~PixelUnpackBuffer();

	void				upload(const PixelBuffer& pix);
private:
	GLuint				m_pbo;
};

class PixelackBuffer{
public:

private:

};
}

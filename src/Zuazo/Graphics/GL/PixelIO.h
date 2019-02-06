#pragma once

#include <GL/glew.h>
#include <sys/types.h>

#include "UniqueBinding.h"
#include "../../Utils/Resolution.h"
#include "../../Utils/PixelTypes.h"

namespace Zuazo::Graphics::GL{

template <Utils::PixelTypes dataType>
struct PixelBuffer{
	Utils::Resolution	res;
	const u_int8_t *	data;
};

template <Utils::PixelTypes dataType>
class PixelUnpackBuffer : public Bindable{
public:
	PixelUnpackBuffer();
	PixelUnpackBuffer(const PixelUnpackBuffer& other);
	~PixelUnpackBuffer();

	void				upload(const PixelBuffer<dataType>& pix);
private:
	GLuint				m_pbo;
};

template <Utils::PixelTypes dataType>
class PixelackBuffer{
public:

private:

};

}

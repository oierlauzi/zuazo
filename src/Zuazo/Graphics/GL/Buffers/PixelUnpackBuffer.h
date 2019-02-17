#pragma once

#include "../../../Utils/ImageBuffer.h"
#include "Buffer.h"

namespace Zuazo::Graphics::GL::Buffers{

class PixelUnpackBuffer : public BufferBase<GL_PIXEL_UNPACK_BUFFER>{
public:
	PixelUnpackBuffer()=default;
	PixelUnpackBuffer(const Utils::ImageBuffer& pix);
	PixelUnpackBuffer(const PixelUnpackBuffer& other)=delete;
	PixelUnpackBuffer(PixelUnpackBuffer&& other);
	~PixelUnpackBuffer()=default;

	const Utils::Resolution& 		getRes() const;
	Utils::PixelTypes 				getPixelType() const;
	const Utils::ImageAttributes& 	getAttributes() const;

	void							upload(const Utils::ImageBuffer& buf);
private:
	Utils::ImageAttributes			m_attributes;
};

/*
 * INLINE METHOD DEFINITIONS
 */

inline PixelUnpackBuffer::PixelUnpackBuffer(const Utils::ImageBuffer& pix) : BufferBase(){
	m_attributes=pix.att;
	BufferBase<GL_PIXEL_UNPACK_BUFFER>::allocate(pix.att.size(), GL_STATIC_DRAW);
	BufferBase<GL_PIXEL_UNPACK_BUFFER>::write(pix.data);
}

inline PixelUnpackBuffer::PixelUnpackBuffer(PixelUnpackBuffer&& other) :
		BufferBase(static_cast<BufferBase<GL_PIXEL_UNPACK_BUFFER>&&>(other))
{
	m_attributes=other.m_attributes;
	other.m_attributes=Utils::ImageAttributes();
}

inline const Utils::Resolution& PixelUnpackBuffer::getRes() const{
	return m_attributes.res;
}

inline Utils::PixelTypes PixelUnpackBuffer::getPixelType()  const{
	return m_attributes.pixType;
}

inline const Utils::ImageAttributes& PixelUnpackBuffer::getAttributes() const{
	return m_attributes;
}

inline void	PixelUnpackBuffer::upload(const Utils::ImageBuffer& buf){
	m_attributes=buf.att;
	BufferBase<GL_PIXEL_UNPACK_BUFFER>::allocate(buf.att.size(), GL_STREAM_DRAW);
	BufferBase<GL_PIXEL_UNPACK_BUFFER>::write(buf.data);
}
}

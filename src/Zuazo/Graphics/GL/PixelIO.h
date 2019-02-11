#pragma once

#include <GL/glew.h>
#include <cstddef>
#include <cstring>

#include "UniqueBinding.h"
#include "../../Utils/ImageAttributes.h"
#include "../../Utils/ImageBuffer.h"
#include "Pool.h"

namespace Zuazo::Graphics::GL{


class PixelUnpackBuffer : public Bindable{
public:
	PixelUnpackBuffer();
	PixelUnpackBuffer(const Utils::ImageBuffer& pix);
	PixelUnpackBuffer(const PixelUnpackBuffer& other);
	PixelUnpackBuffer(PixelUnpackBuffer&& other);
	~PixelUnpackBuffer();

	void							bind() const override;
	void							unbind() const override;

	const Utils::Resolution& 		getRes() const;
	Utils::PixelTypes 				getPixelType() const;
	const Utils::ImageAttributes& 	getAttributes() const;
	size_t							getSize() const;


	void							sub(const Utils::ImageBuffer& buf);
	void							sub(const PixelUnpackBuffer& buf);
private:
	Utils::ImageAttributes			m_attributes;

	GLuint							m_pbo;
};

/**
 * @brief PixelUnpackBuffer pool, organized by buffer's size
 */
extern MultiPool<size_t, PixelUnpackBuffer> pboPool;

class PixelPackBuffer : public Bindable{
public:

private:

};


/*
 * PIXEL UNPACK BUFFER METOHDS
 */


inline PixelUnpackBuffer::PixelUnpackBuffer(){
	glGenBuffers(1, &m_pbo);
}


inline PixelUnpackBuffer::PixelUnpackBuffer(const PixelUnpackBuffer& other) : PixelUnpackBuffer(){
	sub(other);
}


inline PixelUnpackBuffer::PixelUnpackBuffer(const Utils::ImageBuffer& buf) : PixelUnpackBuffer(){
	sub(buf);
}


inline PixelUnpackBuffer::PixelUnpackBuffer(PixelUnpackBuffer&& other){
	//Copy values
	m_pbo=other.m_pbo;
	m_attributes=other.m_attributes;

	//Delete other's values
	other.m_pbo=0;
	other.m_attributes=Utils::ImageAttributes();
}


inline PixelUnpackBuffer::~PixelUnpackBuffer(){
	if(m_pbo)
		glDeleteBuffers(1, &m_pbo);
}


inline void PixelUnpackBuffer::bind() const{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
}


inline void PixelUnpackBuffer::unbind() const{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
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

inline size_t PixelUnpackBuffer::getSize() const{
	return m_attributes.size();
}



inline void PixelUnpackBuffer::sub(const Utils::ImageBuffer& buf){
	UniqueBinding<PixelUnpackBuffer> bind(*this);

	m_attributes=buf.att;
	size_t dataSize=m_attributes.size();

	//Reallocate data
	glBufferData(
			GL_PIXEL_UNPACK_BUFFER,							//target
			dataSize,										//buffer size
			nullptr,										//data
			GL_STREAM_DRAW									//usage
	);

	//Map buffer's data onto memory
	u_int8_t* glData=(u_int8_t*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	if(glData){
		//Copy the data
		memcpy(glData, buf.data, dataSize);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	}
}


inline void PixelUnpackBuffer::sub(const PixelUnpackBuffer& buf){
	//Bind buffers. Not using RAII due to the need of binding the
	//source as GL_PIXEL_PACK_BUFFER instead of GL_PIXEL_UNPACK_BUFFER
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, buf.m_pbo);

	m_attributes=buf.m_attributes;
	size_t dataSize=m_attributes.size();

	//Reallocate data
	glBufferData(
			GL_PIXEL_UNPACK_BUFFER,							//target
			dataSize,										//buffer size
			nullptr,										//data
			GL_STREAM_COPY									//usage
	);

	//Copy the data
	glCopyBufferSubData(
			GL_PIXEL_PACK_BUFFER,							//read target
			GL_PIXEL_UNPACK_BUFFER,							//write target
			0,												//read offset
			0,												//write offset
			dataSize										//size
	);

	//Unbind buffers
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

}

#include "PixelIO.h"

#include <sys/types.h>
#include <cstring>

using namespace Zuazo::Graphics::GL;

/*
 * POOL DEFINITIONS
 */
MultiPool<size_t, PixelUnpackBuffer> Zuazo::Graphics::GL::pboPool;

/*
 * PIXEL UNPACK BUFFER'S METHOD DEFINITIONS
 */

PixelUnpackBuffer::PixelUnpackBuffer(){
	glGenBuffers(1, &m_pbo);
}


PixelUnpackBuffer::PixelUnpackBuffer(const PixelUnpackBuffer& other) : PixelUnpackBuffer(){
	sub(other);
}


PixelUnpackBuffer::PixelUnpackBuffer(const Utils::ImageBuffer& buf) : PixelUnpackBuffer(){
	sub(buf);
}


PixelUnpackBuffer::PixelUnpackBuffer(PixelUnpackBuffer&& other){
	//Copy values
	m_pbo=other.m_pbo;
	m_attributes=other.m_attributes;

	//Delete other's values
	other.m_pbo=0;
	other.m_attributes=Utils::ImageAttributes();
}


PixelUnpackBuffer::~PixelUnpackBuffer(){
	if(m_pbo)
		glDeleteBuffers(1, &m_pbo);
}

void PixelUnpackBuffer::sub(const Utils::ImageBuffer& buf){
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


void PixelUnpackBuffer::sub(const PixelUnpackBuffer& buf){
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

/*
 * PIXEL PACK BUFFER'S METHOD DEFINITIONS
 */

#pragma once

#include <GL/glew.h>
#include <cstddef>
#include <cstring>

#include "UniqueBinding.h"
#include "../../Utils/Resolution.h"
#include "../../Utils/PixelTypes.h"

namespace Zuazo::Graphics::GL{

template <Utils::PixelTypes type>
struct PixelBuffer{
	Utils::Resolution			res;
	u_int8_t *					data;
};

template <Utils::PixelTypes type>
class PixelUnpackBuffer : public Bindable{
public:
	PixelUnpackBuffer();
	PixelUnpackBuffer(const PixelBuffer<type>& pix);
	PixelUnpackBuffer(const PixelUnpackBuffer<type>& other);
	PixelUnpackBuffer(PixelUnpackBuffer&& other);
	~PixelUnpackBuffer();

	void						bind() const override;
	void						unbind() const override;

	const Utils::Resolution& 	getRes() const;

	void						sub(const PixelBuffer<type>& buf);
	void						sub(const PixelUnpackBuffer<type>& buf);
private:
	Utils::Resolution			m_resolution;
	GLuint						m_pbo;
};

template <Utils::PixelTypes type>
class PixelPackBuffer : public Bindable{
public:

private:

};


/*
 * PIXEL UNPACK BUFFER METOHDS
 */

template <Utils::PixelTypes type>
inline PixelUnpackBuffer<type>::PixelUnpackBuffer(){
	glGenBuffers(1, &m_pbo);
}

template <Utils::PixelTypes type>
inline PixelUnpackBuffer<type>::PixelUnpackBuffer(const PixelUnpackBuffer<type>& other) : PixelUnpackBuffer(){
	sub(other);
}

template <Utils::PixelTypes type>
inline PixelUnpackBuffer<type>::PixelUnpackBuffer(const PixelBuffer<type>& buf) : PixelUnpackBuffer(){
	sub(buf);
}

template <Utils::PixelTypes type>
inline PixelUnpackBuffer<type>::PixelUnpackBuffer(PixelUnpackBuffer&& other){
	//Copy values
	m_pbo=other.m_pbo;
	m_resolution=other.m_resolution;

	//Delete other's values
	other.m_pbo=0;
	other.m_resolution=Utils::Resolution();
}

template <Utils::PixelTypes type>
inline PixelUnpackBuffer<type>::~PixelUnpackBuffer(){
	if(m_pbo)
		glDeleteBuffers(1, &m_pbo);
}

template <Utils::PixelTypes type>
inline void PixelUnpackBuffer<type>::bind() const{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
}

template <Utils::PixelTypes type>
inline void PixelUnpackBuffer<type>::unbind() const{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

template <Utils::PixelTypes type>
inline const Utils::Resolution& PixelUnpackBuffer<type>::getRes() const{
	return m_resolution;
}

template <Utils::PixelTypes type>
inline void PixelUnpackBuffer<type>::sub(const PixelBuffer<type>& buf){
	UniqueBinding<PixelUnpackBuffer<type>> bind(*this);

	m_resolution=buf.res;
	size_t dataSize=m_resolution.getSize(Utils::PIXEL_SIZE<type>);

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

template <Utils::PixelTypes type>
inline void PixelUnpackBuffer<type>::sub(const PixelUnpackBuffer<type>& buf){
	//Bind buffers. Not using RAII due to the need of binding the
	//source as GL_PIXEL_PACK_BUFFER instead of GL_PIXEL_UNPACK_BUFFER
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, buf.m_pbo);

	m_resolution=buf.m_resolution;
	size_t dataSize=m_resolution.getSize(Utils::PIXEL_SIZE<type>);

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

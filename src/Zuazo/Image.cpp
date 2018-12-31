#include "Image.h"

#include <cstdio>
#include <cstring>

#include "Context.h"
#include "Surface.h"

using namespace Zuazo;


/********************************
 *	CONSTRUCTOR / DESTRUCTOR	*
 ********************************/


Image::Image() {
	m_res={0, 0};
	m_size=0;

	//Generate the PBO
	UniqueContext ctx(Context::mainCtx);
	glGenBuffers(1, &m_pbo);
}

Image::Image(const Image& image) : Image(){
	copy(image);
}

Image::Image(const ExtImage& extImage) : Image(){
	copy(extImage);
}

Image::Image(const Surface& surface) : Image(){
	copy(surface);
}

Image::~Image() {
	//Delete the PBO
	UniqueContext ctx(Context::mainCtx);
	glDeleteBuffers(1, &m_pbo);
}

/********************************
 *		I/O OPERATIONS			*
 ********************************/

void Image::copy(const Image& image){
	std::lock_guard<std::mutex> lock(m_mutex);

	resize(image.m_res);

	//Get a context
	UniqueContext ctx;

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);

	//Reallocate the data
	glBufferData(GL_PIXEL_UNPACK_BUFFER, m_size, 0, GL_STREAM_COPY);

	//Copy the data
	glBindBuffer(GL_PIXEL_PACK_BUFFER, image.m_pbo);
	glCopyBufferSubData(	GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER,
							0, 0, m_size);

	//Unbind everything
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void Image::copy(const ExtImage& extImage) {
	std::lock_guard<std::mutex> lock(m_mutex);

	resize(extImage.res);

	//Get a context
	UniqueContext ctx;

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);

	//Reallocate the data
	glBufferData(GL_PIXEL_UNPACK_BUFFER, m_size, 0, GL_STREAM_DRAW);

	//Map the PBO into memory
	u_int8_t * glData=(u_int8_t*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	if(glData){
		//Copy the content to it
		memcpy(glData, extImage.data, m_size);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	}

	//Unbind the PBO
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void Image::copy(const Surface& surface){
	std::lock_guard<std::mutex> lock(m_mutex);

	resize(surface.m_res);

	//Get a context
	UniqueContext ctx(Context::mainCtx);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);

	//Reallocate the data
	glBufferData(GL_PIXEL_UNPACK_BUFFER, m_size, 0, GL_STREAM_COPY);

	//TODO
}

ExtImage Image::read(){
	std::lock_guard<std::mutex> lock(m_mutex);

	ExtImage extImg;
	extImg.res=m_res;
	extImg.data=(u_int8_t*)malloc(m_size);

	UniqueContext ctx;

	glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo);

	//Map the PBO into memory
	u_int8_t * glData=(u_int8_t*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	if(glData){
		//Copy the content to it
		memcpy(extImg.data, glData, m_size);
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	}

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	return extImg;
}

/********************************
 *		PRIVATE FUNCTIONS		*
 ********************************/

inline void Image::resize(const Resolution& res){
	m_res=res;
	m_size=res.width * res.height * 4;
}

inline void Image::resize(u_int32_t width, u_int32_t height){
	m_res={width, height};
	m_size=width * height * 4;
}

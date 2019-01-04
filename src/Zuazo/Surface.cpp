#include "Surface.h"

#include <mutex>

using namespace Zuazo;

/********************************
 *	CONSTRUCTOR / DESTRUCTOR	*
 ********************************/

Surface::Surface(const Resolution& res) : Surface(res.width, res.height){}

Surface::Surface(u_int32_t width, u_int32_t height){
	//Get a context
	UniqueContext ctx(Context::mainCtx);

	//Generate the texture and the FBO
	glGenTextures(1, &m_texture);
	glGenFramebuffers(1, &m_fbo);

	//Bind the texture to the FBO
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

	//Set linear scaling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Unbind everything
	glBindTexture(GL_TEXTURE_2D, 0);

	//Set the proper resolution
	resize(width, height);
}

/*
 * Copy constructors
 */

Surface::Surface(const Surface& frame) : Surface(){
	copy(frame);
}

Surface::Surface(const Image& image) : Surface(){
	copy(image);
}

Surface::Surface(const ExtImage& extImage) : Surface(){
	copy(extImage);
}



Surface::~Surface() {
	//Get a context
	UniqueContext ctx(Context::mainCtx);

	//Delete the texture and the FBO
	glDeleteTextures(1, &m_texture);
	glDeleteFramebuffers(1, &m_fbo);
}





/********************************
 *		DRAWING ACTIONS			*
 ********************************/

/*
 * @brief Copies the content of the given frame
 * @param frame: The frame to be copied
 */
void Surface::copy(const Surface& frame){
	//Get a context
	UniqueContext ctx(Context::mainCtx);

	resize(frame.m_res);

	if(m_res){
		glBindFramebuffer(GL_FRAMEBUFFER, frame.m_fbo);

		//Copy the texture
		glBindTexture(GL_TEXTURE_2D, m_texture);
	    glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
	                    0, 0,
	                    0, 0,
	                    m_res.width, m_res.height);

	    //Unbind everything
	    glBindTexture(GL_TEXTURE_2D, 0);
	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

/*
 * @brief Copies the last frame from the image
 * @param image: The source of the frame
 */
void Surface::copy(const Image& image){
	Image& ncImage=const_cast<Image&>(image);

	//Lock the image
	std::lock_guard<std::mutex> lock(ncImage.m_mutex);

	//Get a context
	UniqueContext ctx(Context::mainCtx);

	//Set the new size
	resize(ncImage.m_res);

	if(m_res){
		//Copy the content of the buffer into the texture
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, ncImage.m_pbo);

		//Load texture from PBO
		glTexSubImage2D(GL_TEXTURE_2D, 0,
						0, 0,
						m_res.width, m_res.height,
						GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
}

/*
 * @brief Copies an image from memory
 * @param extImage: a ExtImage structure filled with the size and a
 *  				pointer to the images's data
 */
void Surface::copy(const ExtImage& extImage){
	//Get a context
	UniqueContext ctx(Context::mainCtx);

	resize(extImage.res);

	if(m_res){
		//Copy it to the texture
		glBindTexture(GL_TEXTURE_2D, m_texture);
	    glTexSubImage2D(GL_TEXTURE_2D, 0,
	                    0, 0,
	                    m_res.width, m_res.height,
						GL_RGBA,  GL_UNSIGNED_BYTE,
						extImage.data);
	    glBindTexture(GL_TEXTURE_2D, 0);
	}
}

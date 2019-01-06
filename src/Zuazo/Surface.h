#pragma once

#include <GL/glew.h>
#include <sys/types.h>

#include "Context.h"
#include "Utils/Resolution.h"
#include "Utils/ImgBuffer.h"

namespace Zuazo{

class Image;
class Window;

class Surface{
	friend Image;
	friend Window;
public:
	enum class Blending{
		NONE,
		NORMAL,
		ADD,
		MULTIPLY,
		SCREEN,
	};

	Surface(const Resolution& res);
	Surface(u_int32_t width=0, u_int32_t height=0);
	Surface(const Surface& frame);
	Surface(const Image& image);
	Surface(const ImgBuffer& extImage);
	virtual ~Surface();

	void			setRes(const Resolution& res);
	void			setRes(u_int32_t width, u_int32_t height);

	Resolution		getRes() const;
	void			getRes(u_int32_t* width, u_int32_t* height) const;
	u_int32_t		getWidth() const;
	u_int32_t		getHeight() const;

	GLuint			getTexture() const;

	void			copy(const Surface& frame);
	void			copy(const Image& image);
	void			copy(const ImgBuffer& extImage);
	/* TODO
	void			placeFrame(const Frame& frame, const Shapes::Base& area, Blending blend=Blending::NORMAL);
	void			placeFrame(const Frame& frame, const Shapes::Base& area, const Key& key, Blending blend=Blending::NORMAL);
	void			placeFrame3D(const Frame& frame, const Shapes::Base& area, const Camera& cam, Blending blend=Blending::NORMAL);
	void			placeFrame3D(const Frame& frame, const Shapes::Base& area, const Camera& cam, const Key& key, Blending blend=Blending::NORMAL);
	void			drawPerimeter(const Shapes::Base& shape, float width, const Color& color, Blending blend=Blending::NORMAL);
	void			drawSolid(const Shapes::Base& shape, const Color& color, Blending blend=Blending::NORMAL);
	*/
private:
	Resolution		m_res;

	GLuint			m_texture;
	GLuint			m_fbo;
	GLsync			m_fence;


	float			m_nearClip;
	float			m_farClip;
	float			m_fov;
	float 			m_cameraDistance;
	double 			m_cameraDistanceCompensation;

	void			calculateCamera();

	void			resize(const Resolution& res);
	void			resize(u_int32_t width, u_int32_t height);
	//void			toPerspective(const Camera& cam) const;
	void			toOrtho() const;
	/*void			setBlending(Blending blending) const;
	void			setActiveTexture(GLuint fill=0, GLuint key=0) const;
	void			setActiveTexture(const Frame& all) const;
	void			setActiveTexture(const Frame& fill, const Key& key=NULL) const;*/
	void			stablishFence();
	void			waitFence() const;

};


/*
 * INLINE FUNCTIONS
 */

/********************************
 *			SETS				*
 ********************************/

/*
 * @brief sets frame's resolution. There should be an active context
 * @param res: the desired resolution
 */
inline void Surface::setRes(const Resolution& res) {
	setRes(res.width, res.height);
}

/*
 * @brief sets frame's resolution. There should be an active context
 * @param width: the desired width
 * @param height: the desired height
 */
inline void Surface::setRes(u_int32_t width, u_int32_t height) {
	//Get a context
	UniqueContext ctx(Context::mainCtx);

	resize(width, height);
	//TODO save the old content
}


/********************************
 *			GETS				*
 ********************************/

/*
 * @brief returns a Resolution structure with the resolution of the frame
 * @return The resolution structure
 */
inline Resolution Surface::getRes() const {
	return m_res;
}

/*
 * @brief sets the given pointer's values according to the resolution of this frame.
 * @param width: Pointer to a unsigned int which will be set to the frame's width. NULL is NOT accepted
 * @param height: Pointer to a unsigned int which will be set to the frame's height. NULL is NOT accepted
 */
inline void Surface::getRes(u_int32_t* width, u_int32_t* height) const {
	*width=m_res.width;
	*height=m_res.height;
}

/*
 * @brief returns the width of the frame
 * @return a unsigned int with the width of the frame
 */
inline u_int32_t Surface::getWidth() const{
	return m_res.width;
}

/*
 * @brief returns the height of the frame
 * @return a unsigned int with the height of the frame
 */
inline u_int32_t Surface::getHeight() const{
	return m_res.height;
}


/*
 * @brief returns the OpenGL texture id of this frame
 * @return a OpenGL texture id
 */
inline GLuint Surface::getTexture() const {
	return m_texture;
}

inline void	 Surface::waitFence() const{
	glWaitSync(m_fence, 0, GL_TIMEOUT_IGNORED);
}

}

#pragma once

#include <GL/glew.h>
#include <sys/types.h>

#include "Primitives.h"

namespace Zuazo{

class Uploader;

class Surface{
public:
	class Loop{
		//TODO
	};



	enum class Blending{
		NONE,
		NORMAL,
		ADD,
		MULTIPLY,
		SCREEN,
	};

	struct Camera{
		Vec3		pos;
		Vec3		rotation;
	};

	Surface(const Resolution& res);
	Surface(u_int32_t width=0, u_int32_t height=0);
	Surface(const Surface& frame);
	Surface(const Uploader& uploader);
	Surface(const ExtImage& extImage);
	virtual ~Surface();

	void			setRes(const Resolution& res);
	void			setRes(u_int32_t width, u_int32_t height);

	Resolution		getRes() const;
	void			getRes(u_int32_t* width, u_int32_t* height) const;
	u_int32_t		getWidth() const;
	u_int32_t		getHeight() const;

	GLuint			getTexture() const;

	void			copy(const Surface& frame);
	void			copy(const Uploader& uploader);
	void			copy(const ExtImage& extImage);
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
	void			toPerspective(const Camera& cam);
	void			toOrtho();
	/*void			setBlending(Blending blending);
	void			setActiveTexture(GLuint fill=0, GLuint key=0);
	void			setActiveTexture(const Frame& fill, const Key& key=NULL);
	void			stablishFence();*/

};

}

#pragma once

#include <GL/glew.h>

#include "../../Utils/Resolution.h"
#include "../../Utils/PixelTypes.h"
#include "PixelIO.h"
#include "UniqueBinding.h"


namespace Zuazo::Graphics::GL{
class FrameBuffer;

template <Utils::PixelTypes type>
class Texture2D : public Bindable{
	friend FrameBuffer;
public:
	Texture2D();
	Texture2D(const Utils::Resolution& res);
	Texture2D(const PixelBuffer<type>& buf);
	Texture2D(const PixelUnpackBuffer<type>& buf);
	Texture2D(const Texture2D& other);
	virtual ~Texture2D();

	void						bind() const override;
	void						unbind() const override;

	void						setRes(const Utils::Resolution& res);
	const Utils::Resolution&	getRes() const;

	void						subImage(const PixelBuffer<type>& buf);
	void						subImage(const PixelUnpackBuffer<type>& buf);
protected:
	GLuint						m_texture;
	Utils::Resolution			m_resolution;
};

/*
 * Method implementations
 */

template <Utils::PixelTypes type>
inline Texture2D<type>::Texture2D(){
	glGenTextures(1, &m_texture);
}

template <Utils::PixelTypes type>
inline Texture2D<type>::Texture2D(const Utils::Resolution& res) : Texture2D(){
	setRes(res);
}

template <Utils::PixelTypes type>
inline Texture2D<type>::Texture2D(const Texture2D<type>& other){
	//TODO
}

template <Utils::PixelTypes type>
inline Texture2D<type>::~Texture2D(){
	glDeleteTextures(1, &m_texture);
}


template <Utils::PixelTypes type>
inline void	Texture2D<type>::bind() const{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

template <Utils::PixelTypes type>
inline void	Texture2D<type>::unbind() const{
	glBindTexture(GL_TEXTURE_2D, 0);
}


template <Utils::PixelTypes type>
inline void	Texture2D<type>::setRes(const Utils::Resolution& res){
	if(m_resolution!=res){
		//Resolution has changed
		m_resolution=res;

		UniqueBinding<Texture2D<type>> binding(*this);

		//Resize the GL texture
		glTexImage2D(
				GL_TEXTURE_2D,			//target
				0,						//mip-map level
				(GLenum)type,			//internal format
				m_resolution.width,		//width
				m_resolution.height,	//height
				0,						//border (must be 0)
				0,						//format
				0,						//data type
				NULL					//data
		);

	}
}

template <Utils::PixelTypes type>
inline const Utils::Resolution&	Texture2D<type>::getRes() const{
	return m_resolution;
}


template <Utils::PixelTypes type>
inline void Texture2D<type>::subImage(const PixelBuffer<type>& buf){
	UniqueBinding<Texture2D<type>> texBinding(*this);

	if(m_resolution!=buf.res){
		//Resolution has changed
		m_resolution=buf.res;

		//Resize the GL texture and upload data
		glTexImage2D(
				GL_TEXTURE_2D,			//target
				0,						//mip-map level
				(GLenum)type,			//internal format
				m_resolution.width,		//width
				m_resolution.height,	//height
				0,						//border (must be 0)
				(GLenum)type,			//format
				GL_UNSIGNED_BYTE,		//data type
				buf.data				//data
		);
	}else{
		//Same resolution
		glTexSubImage2D(
				GL_TEXTURE_2D,			//target
				0,						//mip-map-level
				0,						//x offset
				0,						//y offset
				m_resolution.width,		//width
				m_resolution.height,	//height
				(GLenum)type,			//format
				GL_UNSIGNED_BYTE,		//data type
				buf.data				//data
		);
	}
}

template <Utils::PixelTypes type>
inline void	Texture2D<type>::subImage(const PixelUnpackBuffer<type>& buf){
	UniqueBinding<Texture2D<type>> texBinding(*this);
	UniqueBinding<PixelUnpackBuffer<type>> bufBinding(buf);

	if(m_resolution!=buf.res){
		//Resolution has changed
		m_resolution=buf.res;

		//Resize the GL texture and upload data
		glTexImage2D(
				GL_TEXTURE_2D,			//target
				0,						//mip-map level
				(GLenum)type,			//internal format
				m_resolution.width,		//width
				m_resolution.height,	//height
				0,						//border (must be 0)
				(GLenum)type,			//format
				GL_UNSIGNED_BYTE,		//data type
				NULL					//data
		);
	}else{
		//Same resolution
		glTexSubImage2D(
				GL_TEXTURE_2D,			//target
				0,						//mip-map-level
				0,						//x offset
				0,						//y offset
				m_resolution.width,		//width
				m_resolution.height,	//height
				(GLenum)type,			//format
				GL_UNSIGNED_BYTE,		//data type
				NULL					//data
		);
	}
}


}

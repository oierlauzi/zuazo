#pragma once

#include <GL/glew.h>

#include "../../Utils/ImageAttributes.h"
#include "../../Utils/ImageBuffer.h"
#include "PixelIO.h"
#include "UniqueBinding.h"
#include "Pool.h"


namespace Zuazo::Graphics::GL{
class FrameBuffer;

class Texture : public Bindable{
	friend FrameBuffer;
public:
	Texture();
	Texture(const Utils::ImageAttributes& att);
	Texture(const Utils::ImageBuffer& buf);
	Texture(const PixelUnpackBuffer& buf);
	Texture(const Texture& other);
	Texture(Texture&& other);
	virtual ~Texture();

	void							bind() const override;
	void							unbind() const override;

	void							setRes(const Utils::Resolution& res);
	void							setPixelType(Utils::PixelTypes pixType);
	void							setAttributes(const Utils::ImageAttributes& att);

	const Utils::Resolution&		getRes() const;
	Utils::PixelTypes				getPixelType() const;
	const Utils::ImageAttributes& 	getAttributes() const;
	size_t							getSize() const;

	void							subImage(const Utils::ImageBuffer& buf);
	void							subImage(const PixelUnpackBuffer& buf);
	void							subImage(const Texture& buf);

	void							getImage(Utils::ImageBuffer* buf) const;
private:
	Utils::ImageAttributes			m_attributes;

	GLuint							m_texture;
};

/**
 * @brief Texture pool, organized by ImageAttributes
 */
extern MultiPool<Utils::ImageAttributes, Texture> texturePool;

/*
 * Method implementations
 */


inline Texture::Texture(){
	glGenTextures(1, &m_texture);

	UniqueBinding<Texture> binding(*this);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}


inline Texture::Texture(const Utils::ImageAttributes& att) : Texture(){
	setAttributes(att);
}


inline Texture::Texture(const Utils::ImageBuffer& buf) : Texture(){
	subImage(buf);
}


inline Texture::Texture(const PixelUnpackBuffer& buf) : Texture(){
	subImage(buf);
}


inline Texture::Texture(const Texture& other) : Texture(){
	subImage(other);
}


inline Texture::Texture(Texture&& other){
	//Copy values
	m_texture=other.m_texture;
	m_attributes=other.m_attributes;

	//Delete other's values
	other.m_texture=0;
	other.m_attributes=Utils::ImageAttributes();
}


inline Texture::~Texture(){
	if(m_texture)
		glDeleteTextures(1, &m_texture);
}



inline void	Texture::bind() const{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}


inline void	Texture::unbind() const{
	glBindTexture(GL_TEXTURE_2D, 0);
}



inline void	Texture::setRes(const Utils::Resolution& res){
	setAttributes(Utils::ImageAttributes(res, m_attributes.pixType));
}

inline void	Texture::setPixelType(Utils::PixelTypes pixType){
	setAttributes(Utils::ImageAttributes(m_attributes.res, pixType));
}

inline void	Texture::setAttributes(const Utils::ImageAttributes& att){
	if(m_attributes != att){
		//Attributes have changed
		m_attributes=att;

		UniqueBinding<Texture> binding(*this);

		//Resize the GL texture
		glTexImage2D(
				GL_TEXTURE_2D,					//target
				0,								//mip-map level
				(GLenum)m_attributes.pixType,	//internal format
				m_attributes.res.width,			//width
				m_attributes.res.height,		//height
				0,								//border (must be 0)
				(GLenum)m_attributes.pixType,	//format
				GL_UNSIGNED_BYTE,				//data type
				nullptr							//data
		);

	}
}


inline const Utils::Resolution&	Texture::getRes() const{
	return m_attributes.res;
}

inline Utils::PixelTypes Texture::getPixelType() const{
	return m_attributes.pixType;
}

inline const Utils::ImageAttributes& Texture::getAttributes() const{
	return m_attributes;
}

inline size_t Texture::getSize() const{
	return m_attributes.size();
}


inline void Texture::subImage(const Utils::ImageBuffer& buf){
	UniqueBinding<Texture> texBinding(*this);

	if(m_attributes != buf.att){
		//Resolution has changed
		m_attributes=buf.att;

		//Resize the GL texture and upload data
		glTexImage2D(
				GL_TEXTURE_2D,					//target
				0,								//mip-map level
				(GLenum)m_attributes.pixType,	//internal format
				m_attributes.res.width,			//width
				m_attributes.res.height,		//height
				0,								//border (must be 0)
				(GLenum)m_attributes.pixType,	//format
				GL_UNSIGNED_BYTE,				//data type
				buf.data						//data
		);
	}else{
		//Same resolution
		glTexSubImage2D(
				GL_TEXTURE_2D,					//target
				0,								//mip-map-level
				0,								//x offset
				0,								//y offset
				m_attributes.res.width,			//width
				m_attributes.res.height,		//height
				(GLenum)m_attributes.pixType,	//format
				GL_UNSIGNED_BYTE,				//data type
				buf.data						//data
		);
	}
}


inline void	Texture::subImage(const PixelUnpackBuffer& buf){
	UniqueBinding<Texture> texBinding(*this);
	UniqueBinding<PixelUnpackBuffer> bufBinding(buf);

	if(m_attributes != buf.getAttributes()){
		//Resolution has changed
		m_attributes=buf.getAttributes();

		//Resize the GL texture and upload data
		glTexImage2D(
				GL_TEXTURE_2D,					//target
				0,								//mip-map level
				(GLenum)m_attributes.pixType,	//internal format
				m_attributes.res.width,			//width
				m_attributes.res.height,		//height
				0,								//border (must be 0)
				(GLenum)m_attributes.pixType,	//format
				GL_UNSIGNED_BYTE,				//data type
				nullptr							//data
		);
	}else{
		//Same resolution
		glTexSubImage2D(
				GL_TEXTURE_2D,					//target
				0,								//mip-map-level
				0,								//x offset
				0,								//y offset
				m_attributes.res.width,			//width
				m_attributes.res.height,		//height
				(GLenum)m_attributes.pixType,	//format
				GL_UNSIGNED_BYTE,				//data type
				nullptr							//data
		);
	}
}


inline void	Texture::subImage(const Texture& other){
	//Resize if needed
	setAttributes(other.m_attributes);

	//TODO
}


inline void	Texture::getImage(Utils::ImageBuffer* buf) const{
	UniqueBinding<Texture> texBinding(*this);

	//Just copy the content
	glGetTexImage(
			GL_TEXTURE_2D,						//target
			0,									//mip-map level
			(GLenum)m_attributes.pixType,		//format
            GL_UNSIGNED_BYTE,					//data type
			buf->data							//data
	);
}
}

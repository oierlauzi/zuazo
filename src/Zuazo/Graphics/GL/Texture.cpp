#include "Texture.h"

#include "../../Utils/ImageBuffer.h"
#include "../../Utils/Resolution.h"
#include "UniqueBinding.h"
#include "Types.h"

using namespace Zuazo::Graphics::GL;

/*
 * METHOD DEFINITIONS
 */


Texture::Texture(){
	glGenTextures(1, &m_texture);

	UniqueBinding<Texture> binding(*this);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Set up wrapping
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}


Texture::Texture(const Utils::ImageAttributes& att) : Texture(){
	setAttributes(att);
}


Texture::Texture(const Utils::ImageBuffer& buf) : Texture(){
	subImage(buf);
}


Texture::Texture(const Buffers::PixelUnpackBuffer& buf) : Texture(){
	subImage(buf);
}


Texture::Texture(const Texture& other) : Texture(){
	subImage(other);
}


Texture::Texture(Texture&& other){
	//Copy values
	m_texture=other.m_texture;
	m_attributes=other.m_attributes;

	//Delete other's values
	other.m_texture=0;
	other.m_attributes=Utils::ImageAttributes();
}


Texture::~Texture(){
	if(m_texture)
		glDeleteTextures(1, &m_texture);
}




void Texture::setAttributes(const Utils::ImageAttributes& att){
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



void Texture::subImage(const Utils::ImageBuffer& buf){
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


void Texture::subImage(const Buffers::PixelUnpackBuffer& buf){
	UniqueBinding<Texture> texBinding(*this);
	UniqueBinding<Buffers::PixelUnpackBuffer> bufBinding(buf);

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


void Texture::subImage(const Texture& other){
	//Resize if needed
	setAttributes(other.m_attributes);

	//TODO
}


void Texture::getImage(Utils::ImageBuffer* buf) const{
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

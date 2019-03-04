#pragma once

#include <glad/glad.h>
#include <stddef.h>

#include "Buffer.h"
#include "ImageBuffer.h"

namespace Zuazo::Graphics::GL{
class FrameBuffer;

class Texture{
	friend FrameBuffer;
public:
	Texture();
	Texture(const ImageAttributes& att);
	Texture(const ImageBuffer& buf);
	Texture(const Buffer<BufferTypes::PixelUnpack>& buf);
	Texture(const Texture& other);
	Texture(Texture&& other);
	virtual ~Texture();

	template<int binding=0>
	void							bind() const;
	template<int binding=0>
	void							unbind() const;

	void							setRes(const Resolution& res);
	void							setPixelType(PixelFormat pixFmt);
	void							setAttributes(const ImageAttributes& att);

	const Resolution&				getRes() const;
	PixelFormat						getPixelType() const;
	const ImageAttributes& 			getAttributes() const;
	size_t							getSize() const;

	void							subImage(const ImageBuffer& buf);
	void							subImage(const Buffer<BufferTypes::PixelUnpack>& buf);
	void							subImage(const Texture& buf);

	void							getImage(ImageBuffer* buf) const;
private:
	ImageAttributes					m_attributes;

	GLuint							m_texture;
};

/*
 * Method implementations
 */

template<int binding>
inline void	Texture::bind() const{
	glActiveTexture(GL_TEXTURE0 + binding);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

template<int binding>
inline void	Texture::unbind() const{
	glActiveTexture(GL_TEXTURE0 + binding);
	glBindTexture(GL_TEXTURE_2D, 0);
}



inline void	Texture::setRes(const Resolution& res){
	setAttributes(ImageAttributes(res, m_attributes.pixFmt));
}

inline void	Texture::setPixelType(PixelFormat pixFmt){
	setAttributes(ImageAttributes(m_attributes.res, pixFmt));
}

inline const Resolution& Texture::getRes() const{
	return m_attributes.res;
}

inline PixelFormat Texture::getPixelType() const{
	return m_attributes.pixFmt;
}

inline const ImageAttributes& Texture::getAttributes() const{
	return m_attributes;
}

inline size_t Texture::getSize() const{
	return m_attributes.getSize();
}
}

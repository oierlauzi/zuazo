#pragma once

#include <GL/glew.h>
#include <stddef.h>

#include "../../Utils/ImageAttributes.h"
#include "../../Utils/PixelTypes.h"
#include "PixelIO.h"
#include "Pool.h"


namespace Zuazo::Graphics::GL{
class FrameBuffer;

class Texture{
	friend FrameBuffer;
public:
	Texture();
	Texture(const Utils::ImageAttributes& att);
	Texture(const Utils::ImageBuffer& buf);
	Texture(const PixelUnpackBuffer& buf);
	Texture(const Texture& other);
	Texture(Texture&& other);
	virtual ~Texture();

	template<int binding=0>
	void							bind() const;
	template<int binding=0>
	void							unbind() const;

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



inline void	Texture::setRes(const Utils::Resolution& res){
	setAttributes(Utils::ImageAttributes(res, m_attributes.pixType));
}

inline void	Texture::setPixelType(Utils::PixelTypes pixType){
	setAttributes(Utils::ImageAttributes(m_attributes.res, pixType));
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
}

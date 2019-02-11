#pragma once

#include <GL/glew.h>
#include <stddef.h>

#include "../../Utils/ImageAttributes.h"
#include "../../Utils/PixelTypes.h"
#include "PixelIO.h"
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

#pragma once

#include <GL/glew.h>
#include <cstddef>

#include "../../Utils/ImageAttributes.h"
#include "../../Utils/ImageBuffer.h"
#include "../../Utils/PixelTypes.h"
#include "Pool.h"
#include "UniqueBinding.h"

namespace Zuazo::Graphics::GL{


class PixelUnpackBuffer : public Bindable{
public:
	PixelUnpackBuffer();
	PixelUnpackBuffer(const Utils::ImageBuffer& pix);
	PixelUnpackBuffer(const PixelUnpackBuffer& other);
	PixelUnpackBuffer(PixelUnpackBuffer&& other);
	~PixelUnpackBuffer();

	void							bind() const override;
	void							unbind() const override;

	const Utils::Resolution& 		getRes() const;
	Utils::PixelTypes 				getPixelType() const;
	const Utils::ImageAttributes& 	getAttributes() const;
	size_t							getSize() const;


	void							sub(const Utils::ImageBuffer& buf);
	void							sub(const PixelUnpackBuffer& buf);
private:
	Utils::ImageAttributes			m_attributes;

	GLuint							m_pbo;
};

/**
 * @brief PixelUnpackBuffer pool, organized by buffer's size
 */
extern MultiPool<size_t, PixelUnpackBuffer> pboPool;

class PixelPackBuffer : public Bindable{
public:

private:

};


/*
 * PIXEL UNPACK BUFFER'S INLINE METHOD DEFINITIONS
 */

inline void PixelUnpackBuffer::bind() const{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
}


inline void PixelUnpackBuffer::unbind() const{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}


inline const Utils::Resolution& PixelUnpackBuffer::getRes() const{
	return m_attributes.res;
}

inline Utils::PixelTypes PixelUnpackBuffer::getPixelType()  const{
	return m_attributes.pixType;
}

inline const Utils::ImageAttributes& PixelUnpackBuffer::getAttributes() const{
	return m_attributes;
}

inline size_t PixelUnpackBuffer::getSize() const{
	return m_attributes.size();
}

/*
 * PIXEL PACK BUFFER'S INLINE METHOD DEFINITIONS
 */

}

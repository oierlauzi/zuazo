#pragma once

#include <stddef.h>
#include <memory>

#include "GL/Buffer.h"
#include "GL/Texture2D.h"
#include "ImageAttributes.h"
#include "ImageBuffer.h"
#include "Pools.h"

namespace Zuazo::Graphics{

class Frame{
public:
	Frame()=default;
	Frame(const ImageBuffer& buf);
	Frame(std::unique_ptr<GL::PixelUnpackBuffer> buf, const ImageAttributes& att);
	Frame(std::unique_ptr<GL::Texture2D> tex, const ImageAttributes& att);
	Frame(Frame&& other);
	virtual ~Frame();

	const GL::Texture2D& 								getTexture() const;

	const ImageAttributes& 								getAttributes() const;

	static std::unique_ptr<GL::Texture2D>				createTexture(const ImageAttributes& attrib);
	static std::unique_ptr<GL::PixelUnpackBuffer>		createPixelUnpackBuffer(size_t size);
private:
	mutable std::unique_ptr<GL::Texture2D>				m_texture;
	mutable std::unique_ptr<GL::PixelUnpackBuffer> 		m_pbo;

	ImageAttributes										m_attributes;

	static MultiPool<size_t, GL::PixelUnpackBuffer> 	s_pboPool;
	static MultiPool<ImageAttributes, GL::Texture2D> 	s_texPool;
};

/*
 * INLINE METHOD DEFINITIONS
 */

inline const ImageAttributes& Frame::getAttributes() const{
	return m_attributes;
}

inline std::unique_ptr<GL::Texture2D>	Frame::createTexture(const ImageAttributes& attrib){
	return s_texPool.pop(attrib);
}

inline std::unique_ptr<GL::PixelUnpackBuffer> Frame::createPixelUnpackBuffer(size_t size){
	return s_pboPool.pop(size);
}

}

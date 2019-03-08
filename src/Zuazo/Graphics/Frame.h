#pragma once

#include <stddef.h>
#include <algorithm>
#include <memory>

#include "../Utils/ScalingModes.h"
#include "GL/Buffer.h"
#include "GL/Texture2D.h"
#include "ImageAttributes.h"
#include "Pool.h"
#include "Shapes/Rectangle.h"

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
	void												bind() const;
	void												unbind() const;
	void												bindFill() const;
	void												unbindFill() const;
	void												bindKey() const;
	void												unbindKey() const;

	const ImageAttributes& 								getAttributes() const;
	Shapes::Rectangle::RectangleTexCoords				scaleFrame(const Resolution& dstRes, Utils::ScalingModes scaling) const;


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

inline void Frame::bind() const{
	bindFill();
	bindKey();
}
inline void	Frame::unbind() const{
	unbindFill();
	unbindKey();
}

inline void	Frame::bindFill() const{
	getTexture().bind(0);
}
inline void	Frame::unbindFill() const{
	getTexture().unbind(0);
}

inline void	Frame::bindKey() const{
	getTexture().bind(1);
}
inline void	Frame::unbindKey() const{
	getTexture().unbind(1);
}

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

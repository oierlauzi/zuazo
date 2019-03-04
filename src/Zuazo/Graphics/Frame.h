#pragma once

#include <stddef.h>
#include <algorithm>
#include <deque>
#include <memory>

#include "../Utils/ScalingModes.h"
#include "GL/Buffer.h"
#include "GL/ImageBuffer.h"
#include "GL/Texture.h"
#include "Pool.h"
#include "Shapes/Rectangle.h"

namespace Zuazo::Graphics{

class Frame{
public:
	Frame()=default;
	Frame(std::unique_ptr<GL::PixelUnpackBuffer> buf, const GL::ImageAttributes& att);
	Frame(std::unique_ptr<GL::Texture> tex);
	Frame(Frame&& other);
	virtual ~Frame();

	const GL::Texture& 									getTexture() const;
	void												bind() const;
	void												unbind() const;
	void												bindFill() const;
	void												unbindFill() const;
	void												bindKey() const;
	void												unbindKey() const;

	const GL::ImageAttributes& 							getAttributes() const;
	Shapes::Rectangle::RectangleTexCoords				scaleFrame(const GL::Resolution& dstRes, Utils::ScalingModes scaling) const;


	static std::unique_ptr<GL::Texture>					createTexture(const GL::ImageAttributes& attrib);
	static std::unique_ptr<GL::PixelUnpackBuffer>		createPixelUnpackBuffer(size_t size);
	static void											recycleTexture(std::unique_ptr<GL::Texture> tex);
	static void											recyclePixelUnpackBuffer(std::unique_ptr<GL::PixelUnpackBuffer> buf);
private:
	mutable std::unique_ptr<GL::Texture>				m_texture;
	mutable std::unique_ptr<GL::PixelUnpackBuffer> 		m_pbo;

	GL::ImageAttributes									m_attributes;

	static MultiPool<size_t, GL::PixelUnpackBuffer> 	s_pboPool;
	static MultiPool<GL::ImageAttributes, GL::Texture> 	s_texPool;
};

/*
 * INLINE METHOD DEFINITIONS
 */

inline Frame::Frame(std::unique_ptr<GL::PixelUnpackBuffer> buf, const GL::ImageAttributes& att){
	m_attributes=att;
	m_pbo=std::move(buf);
}

inline Frame::Frame(std::unique_ptr<GL::Texture> tex){
	m_attributes=tex->getAttributes();
	m_texture=std::move(tex);
}

inline Frame::Frame(Frame&& other){
	m_attributes=other.m_attributes;
	m_texture=std::move(other.m_texture);
	m_pbo=std::move(other.m_pbo);
}

inline Frame::~Frame(){
	//You should always recycle, its good for the environment
	//Therefore recycle textures and PBOs
	if(m_texture){
		recycleTexture(std::move(m_texture));
	}

	if(m_pbo){
		recyclePixelUnpackBuffer(std::move(m_pbo));
	}
}

inline const GL::Texture& Frame::getTexture() const{
	if(m_texture){
		return *m_texture;
	}else{
		m_texture=createTexture(m_attributes);

		if(m_pbo){
			if(m_texture->getAttributes() != m_attributes){
				m_texture->setAttributes(m_attributes);
			}

			m_texture->subImage(*m_pbo);
		}

		return *m_texture;
	}
}

inline void Frame::bind() const{
	bindFill();
	bindKey();
}
inline void	Frame::unbind() const{
	unbindFill();
	unbindKey();
}

inline void	Frame::bindFill() const{
	getTexture().bind<0>();
}
inline void	Frame::unbindFill() const{
	getTexture().unbind<0>();
}

inline void	Frame::bindKey() const{
	getTexture().bind<1>();
}
inline void	Frame::unbindKey() const{
	getTexture().unbind<1>();
}

inline const GL::ImageAttributes& Frame::getAttributes() const{
	return m_attributes;
}

inline std::unique_ptr<GL::Texture>	Frame::createTexture(const GL::ImageAttributes& attrib){
	return s_texPool.pop(attrib);
}

inline std::unique_ptr<GL::PixelUnpackBuffer> Frame::createPixelUnpackBuffer(size_t size){
	return s_pboPool.pop(size);
}

inline void	Frame::recycleTexture(std::unique_ptr<GL::Texture> tex){
	const GL::ImageAttributes& att=tex->getAttributes();
	s_texPool.push(
			att,
			std::move(tex)
	);
}

inline void	Frame::recyclePixelUnpackBuffer(std::unique_ptr<GL::PixelUnpackBuffer> buf){
	size_t size=buf->getSize();
	s_pboPool.push(
			size,
			std::move(buf)
	);
}

}

#pragma once

#include <stddef.h>
#include <algorithm>
#include <memory>

#include "../Utils/ImageAttributes.h"
#include "../Utils/ImageBuffer.h"
#include "../Utils/PixelTypes.h"
#include "../Utils/ScalingModes.h"
#include "GL/Buffers/PixelUnpackBuffer.h"
#include "GL/Texture.h"
#include "Pool.h"
#include "Shapes/Rectangle.h"

namespace Zuazo::Graphics{

class Frame{
public:
	Frame()=default;
	Frame(const Utils::ImageBuffer& buf);
	Frame(const GL::Buffers::PixelUnpackBuffer& buf);
	Frame(std::unique_ptr<GL::Buffers::PixelUnpackBuffer> pbo);
	Frame(std::unique_ptr<GL::Texture> tex);
	Frame(const Frame& other);
	Frame(Frame&& other);
	virtual ~Frame();

	const GL::Texture& 										getTexture() const;
	void													bind() const;
	void													unbind() const;
	void													bindFill() const;
	void													unbindFill() const;
	void													bindKey() const;
	void													unbindKey() const;

	const Utils::Resolution&								getRes() const;
	Utils::PixelTypes 										getPixelType() const;
	const Utils::ImageAttributes& 							getAttributes() const;
	Shapes::Rectangle::RectangleTexCoords					scaleFrame(const Utils::Resolution& dstRes, Utils::ScalingModes scaling) const;


	static std::unique_ptr<GL::Buffers::PixelUnpackBuffer>	newPixelUnpackBuffer(size_t size);
	static std::unique_ptr<GL::Texture>						newTexture(const Utils::ImageAttributes& attrib);
	static void												recycle(std::unique_ptr<GL::Buffers::PixelUnpackBuffer> pbo);
	static void												recycle(std::unique_ptr<GL::Texture> tex);
private:
	mutable std::unique_ptr<GL::Texture>					m_texture;
	mutable std::unique_ptr<GL::Buffers::PixelUnpackBuffer>	m_pbo;

	static MultiPool<size_t, GL::Buffers::PixelUnpackBuffer> pboPool;
	static MultiPool<Utils::ImageAttributes, GL::Texture> 	texPool;
};

/*
 * INLINE METHOD DEFINITIONS
 */

inline Frame::Frame(const Utils::ImageBuffer& buf){
	m_pbo=newPixelUnpackBuffer(buf.att.size());
	m_pbo->upload(buf);
}

inline Frame::Frame(const GL::Buffers::PixelUnpackBuffer& buf){
	m_texture=newTexture(buf.getAttributes());
	m_texture->subImage(buf);
}

inline Frame::Frame(std::unique_ptr<GL::Buffers::PixelUnpackBuffer> pbo){
	m_pbo=std::move(pbo);
}

inline Frame::Frame(std::unique_ptr<GL::Texture> tex){
	m_texture=std::move(tex);
}

inline Frame::Frame(const Frame& other){
	m_texture=std::unique_ptr<GL::Texture>(new GL::Texture(other.getTexture()));
}

inline Frame::Frame(Frame&& other){
	m_texture=std::move(other.m_texture);
	m_pbo=std::move(other.m_pbo);
}

inline Frame::~Frame(){
	//You should always recycle, its good for the environment
	//Therefore recycle textures and PBOs
	if(m_texture){
		recycle(std::move(m_texture));
	}

	if(m_pbo){
		recycle(std::move(m_pbo));
	}
}

inline const GL::Texture& Frame::getTexture() const{
	if(m_texture){
		return *m_texture;
	}else if(m_pbo){
		m_texture=newTexture(m_pbo->getAttributes());
		m_texture->subImage(*m_pbo);
		return *m_texture;
	}else{
		m_texture=newTexture(Utils::ImageAttributes());
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




inline const Utils::Resolution& Frame::getRes() const{
	return getAttributes().res;
}

inline Utils::PixelTypes Frame::getPixelType() const{
	return getAttributes().pixType;
}

inline const Utils::ImageAttributes& Frame::getAttributes() const{
	if(m_texture){
		return m_texture->getAttributes();
	}else if(m_pbo){
		return m_pbo->getAttributes();
	}else{
		m_texture=newTexture(Utils::ImageAttributes());
		return m_texture->getAttributes();
	}
}

inline std::unique_ptr<GL::Texture>	Frame::newTexture(const Utils::ImageAttributes& attrib){
	return texPool.pop(attrib);
}

inline std::unique_ptr<GL::Buffers::PixelUnpackBuffer> Frame::newPixelUnpackBuffer(size_t size){
	return pboPool.pop(size);
}

inline void	Frame::recycle(std::unique_ptr<GL::Buffers::PixelUnpackBuffer> pbo){
	// 1 hour lost until I figured out that the compiler resets the pointer before
	// it gets read if I put this instruction inside XD
	size_t size=pbo->getSize();
	pboPool.push(
			size,
			std::move(pbo)
	);
}

inline void	Frame::recycle(std::unique_ptr<GL::Texture> tex){
	const Utils::ImageAttributes& att=tex->getAttributes();
	texPool.push(
			att,
			std::move(tex)
	);
}

}

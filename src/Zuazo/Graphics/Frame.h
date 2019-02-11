#pragma once

#include <memory>

#include "../Utils/PixelTypes.h"
#include "GL/PixelIO.h"
#include "GL/Texture.h"
#include "GL/Pool.h"

namespace Zuazo::Graphics{

class Frame{
public:
	Frame()=default;
	Frame(const Utils::ImageBuffer& buf);
	Frame(std::unique_ptr<GL::PixelUnpackBuffer>& pbo);
	Frame(std::unique_ptr<GL::Texture>& tex);
	Frame(const Frame& other);
	Frame(Frame&& other);
	~Frame();

	const GL::Texture& 								getTexture() const;

	const Utils::Resolution&						getRes() const;
	Utils::PixelTypes 								getPixelType() const;
	const Utils::ImageAttributes& 					getAttributes() const;
private:
	mutable std::unique_ptr<GL::Texture>			m_texture;
	mutable std::unique_ptr<GL::PixelUnpackBuffer>	m_pbo;
};

/*
 * INLINE METHOD DEFINITIONS
 */

inline Frame::Frame(const Utils::ImageBuffer& buf){
	m_pbo=GL::pboPool.pop(buf.att.size());
	m_pbo->sub(buf);
}

inline Frame::Frame(std::unique_ptr<GL::PixelUnpackBuffer>& pbo){
	m_pbo=std::move(pbo);
}

inline Frame::Frame(std::unique_ptr<GL::Texture>& tex){
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
	//You should always recycle, its good for the planet
	//Therefore recycle textures and PBOs
	if(m_texture){
		GL::texturePool.push(
				m_texture->getAttributes(),
				m_texture
		);
	}

	if(m_pbo){
		GL::pboPool.push(
				m_pbo->getSize(),
				m_pbo
		);
	}
}

inline const typename GL::Texture& Frame::getTexture() const{
	if(m_texture){
		return *m_texture;
	}else if(m_pbo){
		m_texture=GL::texturePool.pop(m_pbo->getAttributes());
		m_texture->subImage(*m_pbo);
		return *m_texture;
	}else{
		m_texture=GL::texturePool.pop(Utils::ImageAttributes());
		return *m_texture;
	}
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
		m_texture=GL::texturePool.pop(Utils::ImageAttributes());
		return m_texture->getAttributes();
	}
}

}

#pragma once

#include <memory>

#include "../Utils/PixelTypes.h"
#include "GL/PixelIO.h"
#include "GL/Texture.h"
#include "GL/Pool.h"

namespace Zuazo::Graphics{

enum class FrameTypes{
	FILL		=0,
	ALPHA		=1,
	FILL_ALPHA
};

class FrameBase{
public:
	FrameBase()=default;
	FrameBase(const Utils::ImageBuffer& buf);
	FrameBase(std::unique_ptr<GL::PixelUnpackBuffer>& pbo);
	FrameBase(std::unique_ptr<GL::Texture>& tex);
	FrameBase(const FrameBase& other);
	FrameBase(FrameBase&& other);
	virtual ~FrameBase();

	const GL::Texture& 								getTexture() const;

	const Utils::Resolution&						getRes() const;
	Utils::PixelTypes 								getPixelType() const;
	const Utils::ImageAttributes& 					getAttributes() const;
private:
	mutable std::unique_ptr<GL::Texture>			m_texture;
	mutable std::unique_ptr<GL::PixelUnpackBuffer>	m_pbo;
};

template<FrameTypes type>
class Frame :
		public virtual FrameBase,
		public virtual GL::Bindable
{
public:
	using FrameBase::FrameBase; //Inherit constructors

	void bind() const override{
		glActiveTexture(GL_TEXTURE0 + static_cast<int>(type));
		getTexture().bind();
	}

	void unbind() const override{
		glActiveTexture(GL_TEXTURE0 + static_cast<int>(type));
		getTexture().unbind();
	}
};

template<>
class Frame<FrameTypes::FILL_ALPHA> :
	public Frame<FrameTypes::FILL>,
	public Frame<FrameTypes::ALPHA>,
	public virtual FrameBase,
	public virtual GL::Bindable
{
public:
	using FrameBase::FrameBase; //Inherit constructors

	void bindFill() const{
		Frame<FrameTypes::FILL>::bind();
	}

	void unbindFill() const{
		Frame<FrameTypes::FILL>::unbind();
	}

	void bindAlpha() const{
		Frame<FrameTypes::ALPHA>::bind();
	}

	void unbindAlpha() const{
		Frame<FrameTypes::ALPHA>::unbind();
	}

	void bind() const override{
		bindFill();
		bindAlpha();
	}

	void unbind() const override{
		unbindFill();
		unbindAlpha();
	}
};

/*
 * INLINE METHOD DEFINITIONS
 */

inline FrameBase::FrameBase(const Utils::ImageBuffer& buf){
	m_pbo=GL::pboPool.pop(buf.att.size());
	m_pbo->sub(buf);
}

inline FrameBase::FrameBase(std::unique_ptr<GL::PixelUnpackBuffer>& pbo){
	m_pbo=std::move(pbo);
}

inline FrameBase::FrameBase(std::unique_ptr<GL::Texture>& tex){
	m_texture=std::move(tex);
}

inline FrameBase::FrameBase(const FrameBase& other){
	m_texture=std::unique_ptr<GL::Texture>(new GL::Texture(other.getTexture()));
}

inline FrameBase::FrameBase(FrameBase&& other){
	m_texture=std::move(other.m_texture);
	m_pbo=std::move(other.m_pbo);
}

inline FrameBase::~FrameBase(){
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

inline const typename GL::Texture& FrameBase::getTexture() const{
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

inline const Utils::Resolution& FrameBase::getRes() const{
	return getAttributes().res;
}

inline Utils::PixelTypes FrameBase::getPixelType() const{
	return getAttributes().pixType;
}

inline const Utils::ImageAttributes& FrameBase::getAttributes() const{
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

#pragma once

#include <memory>

#include "../Utils/PixelTypes.h"
#include "GL/Texture2D.h"
#include "GL/PixelIO.h"
#include "Pool.h"

namespace Zuazo::Graphics{

template <Utils::PixelTypes type>
class Frame{
public:
	using PixelBuffer=GL::PixelBuffer<type>;
	using PixelUnpackBuffer=GL::PixelUnpackBuffer<type>;
	using Texture=GL::Texture2D<type>;

	Frame()=default;
	Frame(const PixelBuffer& buf);
	Frame(std::unique_ptr<PixelUnpackBuffer>& pbo);
	Frame(std::unique_ptr<Texture>& tex);
	Frame(const Frame<type>& other);
	Frame(Frame<type>&& other);
	~Frame()=default;

	const Texture& 								getTexture() const;
	const Utils::Resolution&					getRes() const;
private:
	mutable std::unique_ptr<Texture>			m_texture;
	mutable std::unique_ptr<PixelUnpackBuffer>	m_pbo;
};

template <Utils::PixelTypes type>
inline Frame<type>::Frame(const PixelBuffer& buf){
	m_pbo=PixelUnpackPool<type>::s_globalPool.pop(buf.res);
	m_pbo->sub(buf);
}

template <Utils::PixelTypes type>
inline Frame<type>::Frame(std::unique_ptr<PixelUnpackBuffer>& pbo){
	m_pbo=std::move(pbo);
}

template <Utils::PixelTypes type>
inline Frame<type>::Frame(std::unique_ptr<Texture>& tex){
	m_texture=std::move(tex);
}

template <Utils::PixelTypes type>
inline Frame<type>::Frame(const Frame<type>& other){
	m_texture=std::unique_ptr<const Texture>(new Texture(other.getTexture()));
}

template <Utils::PixelTypes type>
inline Frame<type>::Frame(Frame<type>&& other){
	m_texture=std::move(other.m_texture);
	m_pbo=std::move(other.m_pbo);
}

template <Utils::PixelTypes type>
inline const typename Frame<type>::Texture& Frame<type>::getTexture() const{
	if(m_texture){
		return *m_texture;
	}else if(m_pbo){
		m_texture=Texture2DPool<type>::s_globalPool.pop(m_pbo->getRes());
		m_texture->subImage(*m_pbo);
		return *m_texture;
	}else{
		m_texture=Texture2DPool<type>::s_globalPool.pop(Utils::Resolution());
		return *m_texture;
	}
}

template <Utils::PixelTypes type>
inline const Utils::Resolution& Frame<type>::getRes() const{
	if(m_texture){
		return m_texture->getRes();
	}else if(m_pbo){
		return m_pbo->getRes();
	}else{
		return Utils::Resolution();
	}
}




}

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

	Frame();
	Frame(const PixelBuffer& buf);
	Frame(std::unique_ptr<const PixelUnpackBuffer>& pbo);
	Frame(std::unique_ptr<const Texture>& tex);
	Frame(const Frame<type>& other);
	~Frame();

	const Texture& 											getTexture() const;
	const Utils::Resolution&								getRes() const;
private:
	Utils::Resolution										m_resolution;

	mutable std::unique_ptr<const Texture>					m_texture;
	mutable std::unique_ptr<const PixelUnpackBuffer>		m_pbo;

	static Pool<Utils::Resolution, const Texture>			s_texturePool;
	static Pool<Utils::Resolution, const PixelUnpackBuffer> s_pixelUnpackPool;
};

}

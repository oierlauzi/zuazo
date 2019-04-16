#include <Graphics/Frame.h>

#include <Graphics/Context.h>
#include <Graphics/GL/UniqueBinding.h>
#include <Utils/Resolution.h>
#include <Utils/Vector.h>

#include <algorithm>

namespace Zuazo::Graphics{

MultiPool<size_t, GL::PixelUnpackBuffer> Frame::s_pboPool;
MultiPool<ImageAttributes, GL::Texture2D>  Frame::s_texPool;

Frame::Frame(std::unique_ptr<GL::PixelUnpackBuffer> buf, const ImageAttributes& att) :
	m_attributes(att),
	m_pbo(std::move(buf))
{
}

Frame::Frame(std::unique_ptr<GL::Texture2D> tex, const ImageAttributes& att) :
	m_attributes(att),
	m_texture(std::move(tex))
{
}

Frame::~Frame(){
	//You should always recycle, its good for the environment
	//Therefore recycle textures and PBOs
	if(m_texture){
		s_texPool.push(
				m_attributes,
				std::move(m_texture)
		);
	}

	if(m_pbo){
		s_pboPool.push(
				m_attributes.getSize(),
				std::move(m_pbo)
		);
	}
}

const GL::Texture2D& Frame::getTexture() const{
	if(m_texture){
		return *m_texture;
	}else{
		m_texture=createTexture(m_attributes);

		if(m_pbo){
			if (!m_texture){
				//There was not an available texture in the pool
				//Set the main context active to create the texture
				UniqueContext ctx(Context::getMainCtx());

				//Create the texture
				m_texture=std::unique_ptr<GL::Texture2D>(new GL::Texture2D);

				//Bind texture and PBO
				GL::UniqueBinding<GL::PixelUnpackBuffer> bufferBinding(*m_pbo);
				GL::UniqueBinding<GL::Texture2D> texBinding(*m_texture);

				m_attributes.textureImage();
			}else{
				//Bind texture and PBO
				GL::UniqueBinding<GL::PixelUnpackBuffer> bufferBinding(*m_pbo);
				GL::UniqueBinding<GL::Texture2D> texBinding(*m_texture);
				m_attributes.textureSubImage();
			}
		}

		return *m_texture;
	}
}

}

#include "Frame.h"

#include <glad/glad.h>

#include "../Utils/Resolution.h"
#include "../Utils/Vector.h"
#include "Context.h"
#include "GL/UniqueBinding.h"
#include "PixelFormat.h"

namespace Zuazo::Graphics{

MultiPool<size_t, GL::PixelUnpackBuffer> Frame::s_pboPool;
MultiPool<ImageAttributes, GL::Texture2D>  Frame::s_texPool;




Frame::Frame(std::unique_ptr<GL::PixelUnpackBuffer> buf, const ImageAttributes& att){
	m_attributes=att;
	m_pbo=std::move(buf);
}

Frame::Frame(std::unique_ptr<GL::Texture2D> tex, const ImageAttributes& att){
	m_attributes=att;
	m_texture=std::move(tex);
}

Frame::Frame(Frame&& other){
	m_attributes=other.m_attributes;
	other.m_attributes=ImageAttributes();

	m_texture=std::move(other.m_texture);
	m_pbo=std::move(other.m_pbo);
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

				glTexImage2D(
						GL_TEXTURE_2D,
						0,
						m_attributes.pixFmt.toGLenum(),
						m_attributes.res.width,
						m_attributes.res.height,
						0,
						m_attributes.pixFmt.toGLenum(),
						GL_UNSIGNED_BYTE,
						0
				);
			}else{
				//Bind texture and PBO
				GL::UniqueBinding<GL::PixelUnpackBuffer> bufferBinding(*m_pbo);
				GL::UniqueBinding<GL::Texture2D> texBinding(*m_texture);

				glTexSubImage2D(
						GL_TEXTURE_2D,
						0,
						0,
						0,
						m_attributes.res.width,
						m_attributes.res.height,
						m_attributes.pixFmt.toGLenum(),
						GL_UNSIGNED_BYTE,
						0
				);
			}
		}

		return *m_texture;
	}
}

Shapes::Rectangle::RectangleTexCoords Frame::scaleFrame(
		const Utils::Resolution& dstRes,
		Utils::ScalingModes scalingMode) const
{
	const Utils::Resolution& srcRes=m_attributes.res;
	if(srcRes){
		float sX, sY, s;

		//Calculate the scale
		sX=(float)dstRes.width / srcRes.width;
		sY=(float)dstRes.height / srcRes.height;


		//Evaluate drawing area's dimensions
		switch(scalingMode){
		case Utils::ScalingModes::Boxed:
			//Scale with the minimum factor
			s=std::min(sX, sY);
			sX=s;
			sY=s;
			break;
		case Utils::ScalingModes::Cropped:
			//Scale with the maximum factor
			s=std::max(sX, sY);
			sX=s;
			sY=s;
			break;
		case Utils::ScalingModes::Stretched:
			//Scale independently.
			//Neither sX nor sY needs to be modified
			break;
		case Utils::ScalingModes::ClampVert:
			//Scale according to sY
			sX=sY;
			break;
		case Utils::ScalingModes::ClampHor:
			//Scale according to sX
			sY=sX;
			break;
		default:
			//This should not happen
			//Don't show anything
			sX=0;
			sY=0;
			break;
		}

		/*
		float diffX = (srcRes.width * sX - dstRes.width) / (2 * srcRes.width * sX);
		float diffY = (srcRes.height * sY - dstRes.height) / (2 * srcRes.height * sY);
		Simplified: */

		float diffX = 0.5 - (dstRes.width) / (2 * srcRes.width * sX);
		float diffY = 0.5 - (dstRes.height) / (2 * srcRes.height * sY);


		return Shapes::Rectangle::RectangleTexCoords {
			Utils::Vec<2, float>{diffX, 		1.0 - diffY		},//Invert Y coordinates, as buffer starts on top
			Utils::Vec<2, float>{1.0 - diffX, 	diffY			}
		};
	} else return Shapes::Rectangle::RectangleTexCoords();
}

}

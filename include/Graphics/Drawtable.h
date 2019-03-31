#pragma once

#include "ImageAttributes.h"
#include "GL/Texture2D.h"
#include "GL/RenderBuffer.h"
#include "GL/FrameBuffer.h"

#include <memory>

namespace Zuazo::Graphics{

class Frame;
class UniqueContext;

namespace GL{
	class Texture2D;
}

class Drawtable{
public:
	Drawtable();
	Drawtable(const ImageAttributes& att);
	Drawtable(const Drawtable& other)=delete;
	Drawtable(Drawtable&& other)=default;
	~Drawtable()=default;

	void								resize(const ImageAttributes& att);

	void 								begin();
	std::unique_ptr<const Frame> 		finish();
private:
	ImageAttributes						m_attributes;

	std::unique_ptr<GL::FrameBuffer> 	m_frameBuffer;
	std::unique_ptr<GL::RenderBuffer>	m_depthBuffer;

	std::unique_ptr<GL::Texture2D>		m_renderTarget;
};
}

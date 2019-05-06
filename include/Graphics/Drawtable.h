#pragma once

#include "ImageAttributes.h"
#include "GL/Texture2D.h"
#include "GL/RenderBuffer.h"
#include "GL/FrameBuffer.h"

#include <memory>
#include <stack>

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

	bool								getDepthTest() const;
	void								setDepthTest(bool depth);

	void 								begin();
	std::unique_ptr<const Frame> 		finish();

	static std::set<PixelFormat>		getSupportedPixelFormats();
private:
	ImageAttributes						m_attributes;
	bool								m_isDepthTesting;

	std::unique_ptr<GL::FrameBuffer> 	m_frameBuffer;
	std::unique_ptr<GL::RenderBuffer>	m_depthBuffer;

	std::unique_ptr<GL::Texture2D>		m_renderTarget;

	static thread_local	std::stack<const Drawtable*> s_activeDrawtables;
};

inline bool	Drawtable::getDepthTest() const{
	return m_isDepthTesting;
}

inline std::set<PixelFormat> Drawtable::getSupportedPixelFormats(){
	return std::set<PixelFormat>{
		PixelFormats::RED8,
		PixelFormats::GREEN8,
		PixelFormats::BLUE8,
		PixelFormats::ALPHA8,
		PixelFormats::RGB24,
		PixelFormats::RGBA32,

		PixelFormats::RED16,
		PixelFormats::GREEN16,
		PixelFormats::BLUE16,
		PixelFormats::ALPHA16,
		PixelFormats::RGB48,
		PixelFormats::RGBA64,

		PixelFormats::RED32,
		PixelFormats::GREEN32,
		PixelFormats::BLUE32,
		PixelFormats::ALPHA32,
		PixelFormats::RGB96,
		PixelFormats::RGBA128,

		PixelFormats::RED32f,
		PixelFormats::GREEN32f,
		PixelFormats::BLUE32f,
		PixelFormats::ALPHA32f,
		PixelFormats::RGB96f,
		PixelFormats::RGBA128f,
	};
}
}

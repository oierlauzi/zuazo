#include <Graphics/Drawtable.h>

#include <Graphics/Frame.h>
#include <Graphics/Context.h>
#include <Graphics/GL/Texture2D.h>
#include <Graphics/GL/RenderBuffer.h>
#include <Graphics/GL/UniqueBinding.h>

#define DEPTHBUFFER_PRECISION GL_DEPTH_COMPONENT24

using namespace Zuazo::Graphics;

thread_local std::stack<const Zuazo::Graphics::Drawtable*> Drawtable::s_activeDrawtables;


Drawtable::Drawtable()
{
	m_frameBuffer=std::unique_ptr<GL::FrameBuffer>(new GL::FrameBuffer); //Create a frame buffer for it
	m_depthBuffer=std::unique_ptr<GL::RenderBuffer>(new GL::RenderBuffer); //Create a rbo for depth buffering

	//Attach the depthbuffer to the framebuffer
	GL::UniqueBinding<GL::FrameBuffer> fboBinding(*m_frameBuffer);
	glFramebufferRenderbuffer(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER,
			m_depthBuffer->get()
	);
}

Drawtable::Drawtable(const ImageAttributes& att) :
	m_attributes(att)
{
	m_frameBuffer=std::unique_ptr<GL::FrameBuffer>(new GL::FrameBuffer); //Create a frame buffer for it
	m_depthBuffer=std::unique_ptr<GL::RenderBuffer>(new GL::RenderBuffer); //Create a rbo for depth buffering

	//Allocate space for the depthbuffer
	GL::UniqueBinding<GL::RenderBuffer> rboBinding(*m_depthBuffer);
	GL::RenderBuffer::allocate(att.res.width, att.res.height, DEPTHBUFFER_PRECISION);

	//Attach the depthbuffer to the framebuffer
	GL::UniqueBinding<GL::FrameBuffer> fboBinding(*m_frameBuffer);
	glFramebufferRenderbuffer(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER,
			m_depthBuffer->get()
	);

}

void Drawtable::resize(const ImageAttributes& att){
	m_attributes=att;
	GL::UniqueBinding<GL::RenderBuffer> rboBinding(*m_depthBuffer);
	GL::RenderBuffer::allocate(att.res.width, att.res.height, DEPTHBUFFER_PRECISION);
}


void Drawtable::begin(){
	m_renderTarget=Frame::createTexture(m_attributes);

	if(!m_renderTarget){
		//There wasn't an available texture in the pool, create it
		m_renderTarget=std::unique_ptr<GL::Texture2D>(new GL::Texture2D);

		GL::UniqueBinding<GL::Texture2D> texBinding(*m_renderTarget);
		m_attributes.textureImage(); //Resize the texture accordingly
	}

	s_activeDrawtables.push(this);
	m_frameBuffer->bind(); //Bind the fbo to start drawing to it

	//Attach the texture to the FBO
	glFramebufferTexture2D(
			GL_FRAMEBUFFER,					//Target
			GL_COLOR_ATTACHMENT0 + 0,		//Attachment
			GL_TEXTURE_2D,					//Texture type
			m_renderTarget->get(),			//Texture name
			0								//Mip-map level
	);

	glViewport(0, 0, m_attributes.res.width, m_attributes.res.height);

	glClearColor(0, 0, 0, 0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

std::unique_ptr<const Frame> Drawtable::finish(){
	//Detach the texture from the FBO
	glFramebufferTexture2D(
			GL_FRAMEBUFFER,					//Target
			GL_COLOR_ATTACHMENT0 + 0,		//Attachment
			GL_TEXTURE_2D,					//Texture type
			0,								//Texture name
			0								//Mip-map level
	);

	s_activeDrawtables.pop();
	if(s_activeDrawtables.empty()){
		m_frameBuffer->unbind();
	}else{
		//There was an active drawtable before, bind its framebuffer again
		s_activeDrawtables.top()->m_frameBuffer->bind();
	}

	return std::unique_ptr<const Frame>(new Frame(std::move(m_renderTarget), m_attributes));
}

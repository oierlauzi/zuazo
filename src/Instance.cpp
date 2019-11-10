#include <Instance.h>

#include <Graphics/Window.h>

namespace Zuazo {

Timing::MainLoop& Instance::getMainLoop(){
	return m_loop;
}
const Timing::MainLoop& Instance::getMainLoop() const{
	return m_loop;
}

Graphics::Instance& Instance::getGraphics(){
	return m_graphics;
}
const Graphics::Instance& Instance::getGraphics() const{
	return m_graphics;
}

}
#include <ZuazoBase.h>

namespace Zuazo{

ZuazoBase::ZuazoBase(const Instance& instance, std::string&& name)
	: Layout(std::move(name))
	, m_instance(instance)
	, m_isOpen(false)
	, m_videoMode(instance.getApplicationInfo().defaultVideoMode)
{
}

const Instance& ZuazoBase::getInstance() const{
	return m_instance;
}

void ZuazoBase::open(){
	m_isOpen = true;
}
void ZuazoBase::close(){
	m_isOpen = false;
}

bool ZuazoBase::isOpen() const{
	return m_isOpen;
}

void ZuazoBase::setVideoMode(const VideoMode& videoMode){
	m_videoMode = videoMode;
}

const VideoMode& ZuazoBase::getVideoMode() const {
	return m_videoMode;
}


}
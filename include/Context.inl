#include "Context.h"

#include "Graphics/Window.h"

namespace Zuazo {


inline std::atomic<size_t> Context::s_activeContextCount = 0;

inline Context::Context(Instance& inst) :
	m_mainLoopLock(inst.getMainLoop().getMutex())
{
	incrementActiveCounter();
}

inline Context::~Context() {
	decrementActiveCounter();
}

inline Context::operator bool(){
	return isActive();
}

inline void Context::activate(){
	m_mainLoopLock.lock();

	incrementActiveCounter();
}

inline void Context::deactivate(){
	m_mainLoopLock.unlock();

	decrementActiveCounter();
}

inline bool Context::isActive() const{
	return m_mainLoopLock.owns_lock();
}

inline void Context::incrementActiveCounter(){
	if(s_activeContextCount.fetch_add(1) == 0){
		//The first context to activate
		Graphics::Window::setCallbacksEnabled(false); 
	} 
}

inline void Context::decrementActiveCounter(){
	if(s_activeContextCount.fetch_sub(1) == 1){
		//The last context to deactivate
		Graphics::Window::setCallbacksEnabled(true); 
	} 
}

}
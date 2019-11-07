#include "Context.h"

#include "Graphics/Window.h"

namespace Zuazo {


inline std::atomic<size_t> Context::s_activeContextCount = 0;
inline bool	Context::s_callbacksEnabled;

inline Context::Context(Instance& inst) :
	m_mainLoopLock(inst.getMainLoop().getMutex())
{
	incrementActiveCounter();
}

inline Context::~Context() {
	decrementActiveCounter();
}

inline Context::operator bool(){
	return m_mainLoopLock.owns_lock();
}

inline void Context::use(){
	m_mainLoopLock.lock();

	incrementActiveCounter();
}

inline void Context::unuse(){
	m_mainLoopLock.unlock();

	decrementActiveCounter();
}

inline void Context::incrementActiveCounter(){
	if(s_activeContextCount++ == 0){
		s_callbacksEnabled = Graphics::Window::getCallbacksEnabled();
		if(s_callbacksEnabled) Graphics::Window::setCallbacksEnabled(false); 
	} 
}

inline void Context::decrementActiveCounter(){
	if(--s_activeContextCount == 0){
		if(s_callbacksEnabled) Graphics::Window::setCallbacksEnabled(true); 
	} 
}

}
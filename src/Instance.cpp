#include <Instance.h>

#include <Graphics/Window.h>

namespace Zuazo {

std::atomic<size_t> Instance::s_instaceCount = 0;

Instance::Instance() {
	if(s_instaceCount++ == 0){
		//I am the first one
		Graphics::Window::init();
	}
}

Instance::~Instance() {
	if(--s_instaceCount == 0){
		//I am the last one
		Graphics::Window::terminate();
	}
}

bool operator==(const Instance& a, const Instance& b){
	return &a == &b;
}

bool operator!=(const Instance& a, const Instance& b){
	return &a != &b;
}

Timing::MainLoop& Instance::getMainLoop(){
	return m_loop;
}
const Timing::MainLoop& Instance::getMainLoop() const{
	return m_loop;
}

Graphics::Vulkan& Instance::getVulkan(){
	return m_vulkan;
}
const Graphics::Vulkan& Instance::getVulkan() const{
	return m_vulkan;
}

uint32_t Instance::getInstaceCount(){
	return s_instaceCount;
}

}
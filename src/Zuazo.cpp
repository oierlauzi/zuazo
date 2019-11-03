#include <Zuazo.h>

#include <Graphics/Window.h>

#include <iostream>

namespace Zuazo {

const Version runtimeVersion = version;



static ApplicationInfo applicationInfo;

void setApplicationInfo(ApplicationInfo&& appInfo){
	if(Instance::getInstaceCount() == 0) {
		applicationInfo = std::forward<ApplicationInfo>(appInfo);
	} else {
		//TODO
	}
}

const ApplicationInfo& getApplicationInfo(){
	return applicationInfo;
}




uint32_t Instance::s_instaceCount = 0;

Instance::Instance() {
	if(s_instaceCount++ == 0){
		//I am the first one
		Graphics::Window::init();
	}
}

Instance::~Instance() {
	if(--s_instaceCount == 0){
		//I am the last one
		Graphics::Window::end();
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
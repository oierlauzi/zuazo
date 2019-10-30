#include <Zuazo.h>

#include <Graphics/Window.h>
#include <Graphics/Vulkan.h>
#include <Timing/MainLoop.h>

#include <iostream>

namespace Zuazo {

static Version sharedLibraryVersion = version;
static ApplicationInfo applicationInfo;

void init(ApplicationInfo&& appInfo){
	applicationInfo = std::forward<ApplicationInfo>(appInfo);
	Graphics::Vulkan::init();
	Graphics::Window::init();
	Timing::MainLoop::init();
}

void end(){
	Timing::MainLoop::end();
	Graphics::Window::end();
	Graphics::Vulkan::end();
}

const Version& getRuntimeVersion(){
	return sharedLibraryVersion;
}

const ApplicationInfo& getApplicationInfo(){
	return applicationInfo;
}

}
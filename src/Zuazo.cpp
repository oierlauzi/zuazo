#include <Zuazo.h>

#include <Graphics/Window.h>

#include <iostream>

namespace Zuazo {

const Version runtimeVersion = version;
static ApplicationInfo applicationInfo;

void setApplicationInfo(ApplicationInfo&& appInfo){
	applicationInfo = std::forward<ApplicationInfo>(appInfo);
}

const ApplicationInfo& getApplicationInfo(){
	return applicationInfo;
}

}
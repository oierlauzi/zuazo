#include <Instance.h>

namespace Zuazo {

Instance::Instance(ApplicationInfo&& applicationInfo)
	: m_applicationInfo(std::move(applicationInfo))
	, m_vulkan(m_applicationInfo.name, m_applicationInfo.version, getVulkanVerbosity(m_applicationInfo.isDebug), m_applicationInfo.deviceScoreFunc)
{
}

const Instance::ApplicationInfo& Instance::getApplicationInfo() const {
	return m_applicationInfo;
}

const Timing::MainLoop& Instance::getMainLoop() const{
	return m_loop;
}

Timing::MainLoop& Instance::getMainLoop(){
	return m_loop;
}


const Graphics::Vulkan& Instance::getVulkan() const{
	return m_vulkan;
}

Graphics::Vulkan& Instance::getVulkan(){
	return m_vulkan;
}


Graphics::Vulkan::Verbosity	Instance::getVulkanVerbosity(bool isDebug){
	using Verbosity = Graphics::Vulkan::Verbosity;
	return isDebug ? Verbosity::ENABLE_VALIDATION_LAYERS : Verbosity::DISABLE_VALIDATION_LAYERS;
}
}
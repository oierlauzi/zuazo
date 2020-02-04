#include <Instance.h>

#include <Zuazo.h>

#include <iostream>
#include <sstream>

namespace Zuazo {

Instance::Instance(ApplicationInfo&& applicationInfo)
	: m_applicationInfo(std::move(applicationInfo))
	, m_vulkan(
		m_applicationInfo.name, 
		m_applicationInfo.version, 
		m_applicationInfo.deviceScoreFunc
	)
{
	log(Verbosity::INFO, generateInitMessage());
}

const Instance::ApplicationInfo& Instance::getApplicationInfo() const {
	return m_applicationInfo;
}

void Instance::log(Verbosity severity, const std::string_view& msg) const{
	if(m_applicationInfo.logFunc && (severity <= m_applicationInfo.verbosity)){
		m_applicationInfo.logFunc(*this, severity, msg);
	}
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

void Instance::defaultLogFunc(const Instance& inst, Verbosity severity, const std::string_view& msg){
	std::ostream& output = std::cerr;

	switch(severity){
		case Verbosity::INFO: output << "[INFO] "; break;
		case Verbosity::WARNING: output << "[WARN] "; break;
		case Verbosity::ERROR: output << "[ERRO] "; break;
		default: break;
	}

	output << &inst << ": " << msg << std::endl;
}

std::string Instance::generateInitMessage() const {
	std::ostringstream message;

	//Show Version
	message << "Instantiated Zuazo " 
			<< runtimeVersion.getMajor() << "." 
			<< runtimeVersion.getMinor() << "." 
			<< runtimeVersion.getPatch();

	//Show selected device
	const auto deviceProperties = m_vulkan.getPhysicalDevice().getProperties(m_vulkan.getDispatcher());
	message << "\n\t- Selected GPU: " << deviceProperties.deviceName;

	return message.str();
}

}
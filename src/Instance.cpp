#include <Instance.h>

#include <Zuazo.h>
#include <StringConversions.h>
#include <Graphics/VulkanConversions.h>

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
	setupSupportedFormats();
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

const std::vector<std::tuple<PixelFormat, PixelEncoding>>& Instance::getSupportedInputFormats() const{
	return m_inputFormats;
}

const std::vector<std::tuple<PixelFormat, PixelEncoding>>& Instance::getSupportedInputYcbcrFormats() const{
	return m_inputYcbcrFormats;
}

const std::vector<std::tuple<PixelFormat, PixelEncoding>>& Instance::getSupportedOutputFormats() const{
	return m_outputFormats;
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



void Instance::setupSupportedFormats(){
	for(auto format = PixelFormat::NONE; format < PixelFormat::COUNT; format++){
		for(auto encoding = PixelEncoding::NONE; encoding < PixelEncoding::COUNT; encoding++){
			const auto [f, s] = Graphics::toVulkan(format, encoding);

			if(f == vk::Format::eUndefined) continue;

			const auto features = m_vulkan.getFormatFeatures(f);
			const auto pix = std::tuple<PixelFormat, PixelEncoding>(format, encoding);

			if(Graphics::hasSamplerSupport(features)) {
				m_inputFormats.emplace_back(pix);

				if(Graphics::hasYCbCrSupport(features)){
					m_inputYcbcrFormats.emplace_back(pix);

					if(encoding == PixelEncoding::UINT){
						const auto pixNar = std::tuple<PixelFormat, PixelEncoding>(format, PixelEncoding::UINT_NARROW);
						m_inputFormats.emplace_back(pixNar);
						m_inputYcbcrFormats.emplace_back(pixNar);
					}
				}
			}

			if(Graphics::hasFramebufferSupport(features) && s == vk::ComponentMapping()) {
				m_outputFormats.emplace_back(pix);
			}
		}
	}
}

std::string Instance::generateInitMessage() const {
	std::ostringstream message;

	//Show Version
	message << "Instantiated Zuazo " 
			<< runtimeVersion.getMajor() << "." 
			<< runtimeVersion.getMinor() << "." 
			<< runtimeVersion.getPatch() << "\n";

	//Show selected device
	const auto deviceProperties = m_vulkan.getPhysicalDevice().getProperties(m_vulkan.getDispatcher());
	message << "\t- Selected GPU: " << deviceProperties.deviceName << "\n";

	//Show supported formats
	message << "\t- Supported input pixel formats:\n";
	for(const auto& fmt : m_inputFormats ){
		message << "\t\t- " << toString(std::get<PixelFormat>(fmt)) << ", " 
				<< toString(std::get<PixelEncoding>(fmt)) << "\n";
	}

	message << "\t- Supported input YCbCr pixel formats:\n";
	for(const auto& fmt : m_inputYcbcrFormats ){
		message << "\t\t- " << toString(std::get<PixelFormat>(fmt)) << ", " 
				<< toString(std::get<PixelEncoding>(fmt)) << "\n";
	}

	message << "\t- Supported destination pixel formats:\n";
	for(const auto& fmt : m_outputFormats ){
		message << "\t\t- " << toString(std::get<PixelFormat>(fmt)) << ", " 
				<< toString(std::get<PixelEncoding>(fmt)) << "\n";
	}

	return message.str();
}

}
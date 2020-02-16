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
		m_applicationInfo.name.c_str(), 
		Graphics::toVulkan(m_applicationInfo.version), 
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

const std::vector<ColorFormat>& Instance::getSupportedInputFormats() const{
	return m_inputFormats;
}

const std::vector<ColorFormat>& Instance::getSupportedInputYcbcrFormats() const{
	return m_inputYcbcrFormats;
}

const std::vector<ColorFormat>& Instance::getSupportedOutputFormats() const{
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
	for(auto format = ColorFormat::NONE; format < ColorFormat::COUNT; format++){
		const auto [f, s] = Graphics::optimizeFormat(Graphics::toVulkan(format));

		if(f == vk::Format::eUndefined) continue;

		const auto features = getFormatFeatures(m_vulkan, f);

		if(Graphics::hasSamplerSupport(features)) {
			m_inputFormats.emplace_back(format);

			if(Graphics::hasYCbCrSupport(features)){
				m_inputYcbcrFormats.emplace_back(format);
			}
		}

		if(Graphics::hasFramebufferSupport(features) && s == vk::ComponentMapping()) {
			m_outputFormats.emplace_back(format);
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
		message << "\t\t- " << toString(fmt) << "\n";
	}

	message << "\t- Supported input YCbCr pixel formats:\n";
	for(const auto& fmt : m_inputYcbcrFormats ){
		message << "\t\t- " << toString(fmt) << "\n";
	}

	message << "\t- Supported output pixel formats:\n";
	for(const auto& fmt : m_outputFormats ){
		message << "\t\t- " << toString(fmt) << "\n";
	}

	return message.str();
}

uint32_t Instance::defaultDeviceScoreFunc(const vk::DispatchLoaderDynamic& disp,
										vk::PhysicalDevice device )
{
	//Scores
	constexpr uint32_t MINIMUM_SCORE = 1;
	constexpr uint32_t DISCRETE_GPU_SCORE = 128;
	constexpr uint32_t YCBCR_SAMPLER_SCORE = 64;
	constexpr uint32_t SAMPLER_FORMAT_SCORE = 4;
	constexpr uint32_t FRAMEBUFFER_FORMAT_SCORE = 8;
	constexpr uint32_t YCBCR_FORMAT_SCORE = 4;
	constexpr uint32_t TEXTURE_RESOLUTION_REDUCTION = 1024;
	constexpr uint32_t FRAMEBUFFER_RESOLUTION_REDUCTION = 1024;

	//Sum all the scores
	int32_t score = MINIMUM_SCORE;

	//Retrieve data about the device	
	auto features = device.getFeatures2<vk::PhysicalDeviceFeatures2, 
										vk::PhysicalDeviceSamplerYcbcrConversionFeatures>(disp);
	const auto properties = device.getProperties(disp);

	//Give the score based on the properties and features
	if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu){
		score += DISCRETE_GPU_SCORE;
	}

	if(features.get<vk::PhysicalDeviceSamplerYcbcrConversionFeatures>().samplerYcbcrConversion){
		score += YCBCR_SAMPLER_SCORE;
	}

	score += properties.limits.maxImageDimension2D / TEXTURE_RESOLUTION_REDUCTION;
	score += properties.limits.maxFramebufferWidth / FRAMEBUFFER_RESOLUTION_REDUCTION;
	score += properties.limits.maxFramebufferHeight / FRAMEBUFFER_RESOLUTION_REDUCTION;

	//Give the score based on the supported formats
	for(auto format = ColorFormat::NONE; format < ColorFormat::COUNT; format++){
		const auto [f, s] = Graphics::toVulkan(format);
		if(f == vk::Format::eUndefined) continue;

		const auto features = device.getFormatProperties(f, disp);

		if(Graphics::hasSamplerSupport(features)) {
			score += SAMPLER_FORMAT_SCORE;
		}

		if(Graphics::hasFramebufferSupport(features) && s == vk::ComponentMapping()) {
			score += FRAMEBUFFER_FORMAT_SCORE;
		}

		if(Graphics::hasYCbCrSupport(features)){
			score += YCBCR_FORMAT_SCORE;
		}
	}

	return score;
}

}
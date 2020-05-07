#include <Instance.h>

#include "Graphics/VulkanUtils.h"
#include "Timing/MainLoop.h"
#include "Timing/Scheduler.h"

#include <Zuazo.h>
#include <ZuazoBase.h>
#include <Graphics/VulkanConversions.h>
#include <Graphics/GLFW.h>

#include <mutex>
#include <iostream>
#include <sstream>
#include <type_traits>

namespace Zuazo {

struct Instance::Impl {
	ApplicationInfo				applicationInfo;
	
	Graphics::GLFW				glfw;
	Graphics::Vulkan			vulkan;

	std::mutex					mutex;
	Timing::Scheduler			scheduler;
	Timing::MainLoop			loop;

	FormatSupport				formatSupport;
	ResolutionSupport			resolutionSupport;

	ScheduledCallback			presentImages;

	Impl(	ApplicationInfo&& appInfo,
			const DeviceScoreFunc& deviceScoreFunc )
		: applicationInfo(std::move(appInfo))
		, glfw()
		, vulkan(
			applicationInfo.name.c_str(), 
			Graphics::toVulkan(applicationInfo.version), 
			deviceScoreFunc
		)
		, mutex()
		, scheduler()
		, loop(scheduler, mutex)
		, formatSupport(queryFormatSupport(vulkan))
		, resolutionSupport(queryResolutionSupport(vulkan))
		, presentImages(std::bind(&Graphics::Vulkan::presentAll, std::cref(vulkan)))
	{
		std::lock_guard<Impl> lock(*this);
		addRegularCallback(presentImages, PRESENT_PRIORITY);
	}

	~Impl() {
		std::lock_guard<Impl> lock(*this);
		removeRegularCallback(presentImages);
	}

	const Instance::ApplicationInfo& getApplicationInfo() const {
		return applicationInfo;
	}

	const Graphics::Vulkan& getVulkan() const{
		return vulkan;
	}

	const FormatSupport& getFormatSupport() const {
		return formatSupport;
	}

	const ResolutionSupport& getResolutionSupport() const {
		return resolutionSupport;
	}

	void addRegularCallback(const ScheduledCallback& cbk, Priority prior) {
		scheduler.addRegularCallback(cbk, prior);
		loop.interrupt();
	}

	void removeRegularCallback(const ScheduledCallback& cbk) {
		scheduler.removeRegularCallback(cbk);
		loop.interrupt();
	}

	void addPeriodicCallback(const ScheduledCallback& cbk, Priority prior, Timing::Duration period) {
		scheduler.addPeriodicCallback(cbk, prior, period);
		loop.interrupt();
	}

	void removePeriodicCallback(const ScheduledCallback& cbk) {
		scheduler.removePeriodicCallback(cbk);
		loop.interrupt();
	}

	Timing::TimePoint getTime() const {
		return scheduler.getTime();
	}

	Timing::TimePoint getEpoch() const {
		return scheduler.getEpoch();
	}

	Timing::Duration getDeltaT() const {
		return scheduler.getDeltaT();
	}


	void lock() {
		glfw.disableCallbacks();
		mutex.lock();
	}

	bool try_lock() {
		glfw.disableCallbacks();
		const bool success = mutex.try_lock();

		//Undo all changes
		if(!success) {
			glfw.enableCallbacks();
		}

		return success;
	}

	void unlock() {
		mutex.unlock();
		glfw.enableCallbacks();
	}



	std::string	generateInitMessage() const {
		std::ostringstream message;

		//Show Version
		message << "Instantiated Zuazo " << toString(runtimeVersion) << "\n";

		//Show selected device
		const auto deviceProperties = vulkan.getPhysicalDevice().getProperties(vulkan.getDispatcher());
		message << "\t- Selected GPU: " << deviceProperties.deviceName << "\n";

		//Show supported formats
		message << "\t- Supported input pixel formats:\n";
		for(const auto& fmt : formatSupport.inputFormats ){
			message << "\t\t- " << toString(fmt) << "\n";
		}

		message << "\t- Supported output pixel formats:\n";
		for(const auto& fmt : formatSupport.outputFormats ){
			message << "\t\t- " << toString(fmt) << "\n";
		}

		//Show resolution limits
		message << "\t- Maximum input resolution: " <<  toString(resolutionSupport.maxInputResolution) << "\n";
		message << "\t- Maximum output resolution: " <<  toString(resolutionSupport.maxOutputResolution) << "\n";

		return message.str();
	}

private:
	static FormatSupport queryFormatSupport(const Graphics::Vulkan& vulkan) {
		const auto& supported = vulkan.getFormatSupport();

		return { 
			Graphics::getSamplerFormats(supported.sampler), 
			Graphics::getFramebufferFormats(supported.framebuffer)
		};
	}

	static ResolutionSupport queryResolutionSupport(const Graphics::Vulkan& vulkan) {
		const auto prop = vulkan.getPhysicalDevice().getProperties(vulkan.getDispatcher());

		return ResolutionSupport {
			Resolution(prop.limits.maxImageDimension2D, prop.limits.maxImageDimension2D),
			Resolution(prop.limits.maxFramebufferWidth, prop.limits.maxFramebufferHeight)
		};
	}

};


Instance::Instance(	ApplicationInfo&& applicationInfo,
					const DeviceScoreFunc& deviceScoreFunc )
	: m_impl(std::move(applicationInfo), deviceScoreFunc)
{
	ZUAZO_LOG(*this, Severity::INFO, m_impl->generateInitMessage());
}

Instance::Instance(Instance&& other) = default;

Instance::~Instance() = default; 

Instance& Instance::operator=(Instance&& other) = default;

const Instance::ApplicationInfo& Instance::getApplicationInfo() const {
	return m_impl->getApplicationInfo();
}

const Graphics::Vulkan& Instance::getVulkan() const{
	return m_impl->getVulkan();
}

const Instance::FormatSupport& Instance::getFormatSupport() const {
	return m_impl->getFormatSupport();
}

const Instance::ResolutionSupport& Instance::getResolutionSupport() const {
	return m_impl->getResolutionSupport();
}

void Instance::addRegularCallback(	const ScheduledCallback& cbk, 
									Priority prior ) 
{
	m_impl->addRegularCallback(cbk, prior);
}

void Instance::removeRegularCallback(const ScheduledCallback& cbk) {
	m_impl->removeRegularCallback(cbk);
}


void Instance::addPeriodicCallback(	const ScheduledCallback& cbk, 
									Priority prior, 
									Timing::Duration period ) 
{
	m_impl->addPeriodicCallback(cbk, prior, period );
}

void Instance::removePeriodicCallback(const ScheduledCallback& cbk) {
	m_impl->removePeriodicCallback(cbk);
}

Timing::TimePoint Instance::getTime() const {
	return m_impl->getTime();
}

Timing::TimePoint Instance::getEpoch() const {
	return m_impl->getEpoch();
}

Timing::Duration Instance::getDeltaT() const {
	return m_impl->getDeltaT();
}



void Instance::lock() {
	m_impl->lock();
}

bool Instance::try_lock() {
	return m_impl->try_lock();
}

void Instance::unlock() {
	m_impl->unlock();
}




void Instance::defaultInstanceLogFunc(	const Instance& inst, 
										Severity severity, 
										std::string_view msg )
{
	std::ostream& output = std::cerr;

	output << "[" << Utils::toString(severity) << "] ";
	output << &inst << " (" << inst.getApplicationInfo().name << "): ";
	output << msg;
	output << std::endl;
}

void Instance::defaultElementLogFunc(	const ZuazoBase& base, 
										Severity severity, 
										std::string_view msg )
{
	std::ostream& output = std::cerr;
	Instance& inst = base.getInstance();

	output << "[" << Utils::toString(severity) << "] ";
	output << &inst << " (" << inst.getApplicationInfo().name << "): ";
	output << "on " << &base << " (" << base.getName() << "): ";
	output << msg;
	output << std::endl;
}

uint32_t Instance::defaultDeviceScoreFunc(	const vk::DispatchLoaderDynamic& disp,
											vk::PhysicalDevice device )
{
	//Scores
	constexpr uint32_t MINIMUM_SCORE = 1;
	constexpr uint32_t DISCRETE_GPU_SCORE = 128;
	constexpr uint32_t SAMPLER_FORMAT_SCORE = 4;
	constexpr uint32_t FRAMEBUFFER_FORMAT_SCORE = 8;
	constexpr uint32_t TEXTURE_RESOLUTION_REDUCTION = 1024;
	constexpr uint32_t FRAMEBUFFER_RESOLUTION_REDUCTION = 1024;
	constexpr uint32_t SIMULTANEOUS_ALLOCATION_REDUCTION = 64;

	//Sum all the scores
	int32_t score = MINIMUM_SCORE;

	//Retrieve data about the device	
	const auto properties = device.getProperties(disp);

	//Give the score based on the properties and features
	if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu){
		score += DISCRETE_GPU_SCORE;
	}

	score += properties.limits.maxImageDimension2D / TEXTURE_RESOLUTION_REDUCTION;
	score += properties.limits.maxFramebufferWidth / FRAMEBUFFER_RESOLUTION_REDUCTION;
	score += properties.limits.maxFramebufferHeight / FRAMEBUFFER_RESOLUTION_REDUCTION;
	score += properties.limits.maxMemoryAllocationCount / SIMULTANEOUS_ALLOCATION_REDUCTION;

	//Give the score based on the supported formats
	for(size_t i = VK_FORMAT_BEGIN_RANGE; i < VK_FORMAT_END_RANGE; i++){
		const auto format = static_cast<vk::Format>(i);
		const auto formatProperties = device.getFormatProperties(format, disp);

		if(Graphics::hasSamplerSupport(formatProperties)) {
			score += SAMPLER_FORMAT_SCORE;
		}

		if(Graphics::hasFramebufferSupport(formatProperties)) {
			score += FRAMEBUFFER_FORMAT_SCORE;
		}
	}

	return score;
}

}
#include <zuazo/Instance.h>

#include "Graphics/VulkanUtils.h"
#include "Timing/MainLoop.h"
#include "Timing/Scheduler.h"

#include <zuazo/Zuazo.h>
#include <zuazo/ZuazoBase.h>
#include <zuazo/Graphics/VulkanConversions.h>

#include <mutex>
#include <iostream>
#include <sstream>
#include <type_traits>

namespace Zuazo {

struct Instance::Impl {
	Instance&					instance;

	ApplicationInfo				applicationInfo;

	Graphics::Vulkan			vulkan;

	std::mutex					mutex;
	Timing::Scheduler			scheduler;
	Timing::MainLoop			loop;

	FormatSupport				formatSupport;
	ResolutionSupport			resolutionSupport;

	std::shared_ptr<ScheduledCallback> presentImages;

	Impl(	Instance& instance,
			ApplicationInfo appInfo,
			const DeviceScoreFunc& deviceScoreFunc )
		: instance(instance)
		, applicationInfo(std::move(appInfo))
		, vulkan(
			applicationInfo.getName().c_str(), 
			Graphics::toVulkan(applicationInfo.getVersion()), 
			applicationInfo.getVerbosity(),
			getRequiredVulkanInstanceExtensions(applicationInfo.getModules()),
			getRequiredVulkanDeviceExtensions(applicationInfo.getModules()),
			std::bind(&Impl::vulkanLogCallback, std::ref(*this), std::placeholders::_1, std::placeholders::_2),
			std::bind(&Impl::getPresentationSupport, std::cref(applicationInfo.getModules()), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
			deviceScoreFunc
		)
		, mutex()
		, scheduler()
		, loop(scheduler, mutex)
		, formatSupport(queryFormatSupport(vulkan))
		, resolutionSupport(queryResolutionSupport(vulkan))
		, presentImages(createPresentCallback(vulkan))
	{
		std::lock_guard<Impl> lock(*this);
		addRegularCallback(presentImages, PRESENT_PRIORITY);

		for(Module& module : applicationInfo.getModules()) {
			module.initialize(instance);
		}

		ZUAZO_LOG(instance, Severity::INFO, generateInitMessage());
	}

	~Impl() {
		std::lock_guard<Impl> lock(*this);

		for(Module& module : applicationInfo.getModules()) {
			module.terminate(instance);
		}

		removeRegularCallback(presentImages);
		ZUAZO_LOG(instance, Severity::INFO, "Terminated");
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

	void addRegularCallback(const std::shared_ptr<ScheduledCallback>& cbk, Priority prior) {
		scheduler.addRegularCallback(cbk, prior);
		loop.interrupt();

		ZUAZO_LOG(instance, Severity::VERBOSE, generateAddRegularEventMessage(cbk, prior));
	}

	void removeRegularCallback(const std::shared_ptr<ScheduledCallback>& cbk) {
		scheduler.removeRegularCallback(cbk);
		loop.interrupt();

		ZUAZO_LOG(instance, Severity::VERBOSE, generateRemoveRegularEventMessage(cbk));
	}

	void addPeriodicCallback(const std::shared_ptr<ScheduledCallback>& cbk, Priority prior, Duration period) {
		scheduler.addPeriodicCallback(cbk, prior, period);
		loop.interrupt();

		ZUAZO_LOG(instance, Severity::VERBOSE, generateAddPeriodicEventMessage(cbk, prior, period));
	}

	void removePeriodicCallback(const std::shared_ptr<ScheduledCallback>& cbk) {
		scheduler.removePeriodicCallback(cbk);
		loop.interrupt();

		ZUAZO_LOG(instance, Severity::VERBOSE, generateRemovePeriodicEventMessage(cbk));
	}

	TimePoint getTime() const {
		return scheduler.getTime();
	}

	TimePoint getEpoch() const {
		return scheduler.getEpoch();
	}

	Duration getDeltaT() const {
		return scheduler.getDeltaT();
	}


	void lock() {
		mutex.lock();
	}

	bool try_lock() {
		return mutex.try_lock();
	}

	void unlock() {
		mutex.unlock();
	}


private:
	static Module::VulkanExtensions getRequiredVulkanInstanceExtensions(const ApplicationInfo::Modules& modules) {
		Module::VulkanExtensions result;

		for(const auto& module : modules) {
			const auto extensions = static_cast<const Module&>(module).getRequiredVulkanInstanceExtensions();
			std::copy(
				extensions.cbegin(), extensions.cend(),
				std::back_inserter(result)
			);
		}

		return result;
	}

	static Module::VulkanExtensions getRequiredVulkanDeviceExtensions(const ApplicationInfo::Modules& modules) {
		Module::VulkanExtensions result;

		for(const auto& module : modules) {
			const auto extensions = static_cast<const Module&>(module).getRequiredVulkanDeviceExtensions();
			std::copy(
				extensions.cbegin(), extensions.cend(),
				std::back_inserter(result)
			);
		}

		return result;
	}

	static bool getPresentationSupport(	const ApplicationInfo::Modules& modules,
										vk::Instance instance,
										vk::PhysicalDevice device, 
										uint32_t queueIndex ) 
	{
		return std::all_of(
			modules.cbegin(), modules.cend(),
			[instance, device, queueIndex] (const Module& module) -> bool {
				return module.getPresentationSupport(instance, device, queueIndex);
			}
		);
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

	std::string generateAddRegularEventMessage(const std::shared_ptr<ScheduledCallback>& cbk, Priority prior) const {
		std::ostringstream message;

		message << "Regular event added: " << cbk << " ";
		message << "(Priority: " << prior << ")";

		return message.str();
	}
	
	std::string generateRemoveRegularEventMessage(const std::shared_ptr<ScheduledCallback>& cbk) const {
		std::ostringstream message;

		message << "Regular event removed: " << cbk << " ";

		return message.str();
	}

	std::string generateAddPeriodicEventMessage(const std::shared_ptr<ScheduledCallback>& cbk, Priority prior, Duration period) const {
		std::ostringstream message;

		message << "Periodic event added: " << cbk << " ";
		message << "(Priority: " << prior << ", ";
		message << "Rate: " << getRate(period) << " Hz)";

		return message.str();
	}

	std::string generateRemovePeriodicEventMessage(const std::shared_ptr<ScheduledCallback>& cbk) const {
		std::ostringstream message;

		message << "Periodic event removed: " << cbk << " ";

		return message.str();
	}

	void vulkanLogCallback(Severity severity, std::string msg) {
		if(applicationInfo.getInstanceLogFunc()) {
			applicationInfo.getInstanceLogFunc()(
				instance, 
				severity, 
				"Vulkan:\n" + msg
			);
		}
	}

	static FormatSupport queryFormatSupport(const Graphics::Vulkan& vulkan) {
		const auto& supported = vulkan.getFormatSupport();

		return { 
			Graphics::getSamplerFormats(supported.sampler), 
			Graphics::getFramebufferFormats(supported.framebuffer)
		};
	}

	static ResolutionSupport queryResolutionSupport(const Graphics::Vulkan& vulkan) {
		const auto& limits = vulkan.getPhysicalDeviceProperties().limits;

		return ResolutionSupport {
			Resolution(limits.maxImageDimension2D, limits.maxImageDimension2D),
			Resolution(limits.maxFramebufferWidth, limits.maxFramebufferHeight)
		};
	}

	static std::shared_ptr<ScheduledCallback> createPresentCallback(const Graphics::Vulkan& vulkan) {
		return std::make_shared<ScheduledCallback>(std::bind(&Graphics::Vulkan::presentAll, std::cref(vulkan)));
	}

};


Instance::Instance(	ApplicationInfo applicationInfo,
					const DeviceScoreFunc& deviceScoreFunc )
	: m_impl({}, *this, std::move(applicationInfo), deviceScoreFunc)
{
}

Instance::~Instance() = default; 

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

void Instance::addRegularCallback(	const std::shared_ptr<ScheduledCallback>& cbk, 
									Priority prior ) 
{
	m_impl->addRegularCallback(cbk, prior);
}

void Instance::removeRegularCallback(const std::shared_ptr<ScheduledCallback>& cbk) {
	m_impl->removeRegularCallback(cbk);
}


void Instance::addPeriodicCallback(	const std::shared_ptr<ScheduledCallback>& cbk, 
									Priority prior, 
									Duration period ) 
{
	m_impl->addPeriodicCallback(cbk, prior, period );
}

void Instance::removePeriodicCallback(const std::shared_ptr<ScheduledCallback>& cbk) {
	m_impl->removePeriodicCallback(cbk);
}

TimePoint Instance::getTime() const {
	return m_impl->getTime();
}

TimePoint Instance::getEpoch() const {
	return m_impl->getEpoch();
}

Duration Instance::getDeltaT() const {
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
	for(const auto& range : Graphics::Vulkan::FORMAT_RANGES){
		for(auto i = range.getMin(); i <= range.getMax(); reinterpret_cast<int&>(i)++){
			const auto format = static_cast<vk::Format>(i);
			const auto formatProperties = device.getFormatProperties(format, disp);

			if(Graphics::hasSamplerSupport(formatProperties)) {
				score += SAMPLER_FORMAT_SCORE;
			}

			if(Graphics::hasFramebufferSupport(formatProperties)) {
				score += FRAMEBUFFER_FORMAT_SCORE;
			}
		}
	}

	return score;
}



/*
 * Instance::Module
 */

void Instance::Module::initialize(Instance&) {}

void Instance::Module::terminate(Instance&) {}

Instance::Module::VulkanExtensions Instance::Module::getRequiredVulkanInstanceExtensions() const {
	return {};
}

Instance::Module::VulkanExtensions Instance::Module::getRequiredVulkanDeviceExtensions() const {
	return {};
}

bool Instance::Module::getPresentationSupport(	vk::Instance, 
												vk::PhysicalDevice, 
												uint32_t ) const 
{
	return true;
}



/*
 * Instance::ApplicationInfo
 */

Instance::ApplicationInfo::ApplicationInfo(	std::string name,
											Version version,
											Verbosity verbosity,
											Modules modules,
											InstanceLogFunc	instanceLogFunc,
											ElementLogFunc elementLogFunc )
	: m_name(std::move(name))
	, m_version(version)
	, m_verbosity(verbosity)
	, m_modules(std::move(modules))
	, m_instanceLogFunc(std::move(instanceLogFunc))
	, m_elementLogFunc(std::move(elementLogFunc))
{
}



const std::string& Instance::ApplicationInfo::getName() const {
	return m_name;
}

Version Instance::ApplicationInfo::getVersion() const {
	return m_version;
}

Verbosity Instance::ApplicationInfo::getVerbosity() const {
	return m_verbosity;
}

const Instance::ApplicationInfo::Modules& Instance::ApplicationInfo::getModules() const {
	return m_modules;
}

const Instance::ApplicationInfo::InstanceLogFunc& Instance::ApplicationInfo::getInstanceLogFunc() const {
	return m_instanceLogFunc;
}

const Instance::ApplicationInfo::ElementLogFunc& Instance::ApplicationInfo::getElementLogFunc() const {
	return m_elementLogFunc;
}



void Instance::ApplicationInfo::defaultInstanceLogFunc(	const Instance& inst, 
														Severity severity, 
														std::string_view msg )
{
	std::ostream& output = std::cerr;

	output << "[" << toString(severity) << "] ";
	output << &inst << " (" << inst.getApplicationInfo().getName() << "): ";
	output << msg;
	output << std::endl;
}

void Instance::ApplicationInfo::defaultElementLogFunc(	const ZuazoBase& base, 
														Severity severity, 
														std::string_view msg )
{
	std::ostream& output = std::cerr;
	Instance& inst = base.getInstance();

	output << "[" << toString(severity) << "] ";
	output << &inst << " (" << inst.getApplicationInfo().getName() << "): ";
	output << "on " << &base << " (" << base.getName() << "): ";
	output << msg;
	output << std::endl;
}


}
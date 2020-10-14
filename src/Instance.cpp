#include <zuazo/Instance.h>

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

		for(const Module& module : applicationInfo.getModules()) {
			module.initialize(instance);
		}
	}

	~Impl() {
		std::lock_guard<Impl> lock(*this);

		for(const Module& module : applicationInfo.getModules()) {
			module.terminate(instance);
		}

		removeRegularCallback(presentImages);
	}

	const Instance::ApplicationInfo& getApplicationInfo() const noexcept {
		return applicationInfo;
	}

	const Graphics::Vulkan& getVulkan() const noexcept {
		return vulkan;
	}

	const FormatSupport& getFormatSupport() const noexcept {
		return formatSupport;
	}

	const ResolutionSupport& getResolutionSupport() const noexcept {
		return resolutionSupport;
	}

	void addRegularCallback(const std::shared_ptr<ScheduledCallback>& cbk, Priority prior) {
		scheduler.addRegularCallback(cbk, prior);
		loop.interrupt();
	}

	void removeRegularCallback(const std::shared_ptr<ScheduledCallback>& cbk) {
		scheduler.removeRegularCallback(cbk);
		loop.interrupt();
	}

	void addPeriodicCallback(const std::shared_ptr<ScheduledCallback>& cbk, Priority prior, Duration period) {
		scheduler.addPeriodicCallback(cbk, prior, period);
		loop.interrupt();
	}

	void removePeriodicCallback(const std::shared_ptr<ScheduledCallback>& cbk) {
		scheduler.removePeriodicCallback(cbk);
		loop.interrupt();
	}

	TimePoint getTime() const noexcept {
		return scheduler.getTime();
	}

	TimePoint getEpoch() const noexcept {
		return scheduler.getEpoch();
	}

	Duration getDeltaT() const noexcept {
		return scheduler.getDeltaT();
	}


	void lock() noexcept {
		mutex.lock();
	}

	bool try_lock() noexcept {
		return mutex.try_lock();
	}

	void unlock() noexcept {
		mutex.unlock();
	}



	std::string	generateInitMessage() const {
		std::ostringstream message;

		//Show Version
		message << "Instantiated Zuazo " << runtimeVersion << "\n";

		//Show modules
		message << "\t- Loaded modules:\n";
		for(const Module& module : applicationInfo.getModules() ){
			message << "\t\t- " << module.getName() << " " << module.getVersion() << "\n";
		}

		//Show selected device
		const auto deviceProperties = vulkan.getPhysicalDevice().getProperties(vulkan.getDispatcher());
		message << "\t- Selected GPU: " << deviceProperties.deviceName << "\n";

		//Show supported formats
		message << "\t- Supported input pixel formats:\n";
		for(const auto& fmt : formatSupport.inputFormats ){
			message << "\t\t- " << fmt << "\n";
		}

		message << "\t- Supported output pixel formats:\n";
		for(const auto& fmt : formatSupport.outputFormats ){
			message << "\t\t- " << fmt << "\n";
		}

		//Show resolution limits
		message << "\t- Maximum input resolution: " << resolutionSupport.maxInputResolution << "\n";
		message << "\t- Maximum output resolution: " << resolutionSupport.maxOutputResolution << "\n";

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
	ZUAZO_LOG(*this, Severity::INFO, m_impl->generateInitMessage());
}

Instance::~Instance() {
	ZUAZO_LOG(*this, Severity::INFO, "Terminated");
}

const Instance::ApplicationInfo& Instance::getApplicationInfo() const noexcept {
	return m_impl->getApplicationInfo();
}

const Graphics::Vulkan& Instance::getVulkan() const noexcept {
	return m_impl->getVulkan();
}

const Instance::FormatSupport& Instance::getFormatSupport() const noexcept {
	return m_impl->getFormatSupport();
}

const Instance::ResolutionSupport& Instance::getResolutionSupport() const noexcept {
	return m_impl->getResolutionSupport();
}

void Instance::addRegularCallback(	const std::shared_ptr<ScheduledCallback>& cbk, 
									Priority prior ) 
{
	m_impl->addRegularCallback(cbk, prior);
	ZUAZO_LOG(*this, Severity::VERBOSE, m_impl->generateAddRegularEventMessage(cbk, prior));
}

void Instance::removeRegularCallback(const std::shared_ptr<ScheduledCallback>& cbk) {
	m_impl->removeRegularCallback(cbk);
	ZUAZO_LOG(*this, Severity::VERBOSE, m_impl->generateRemoveRegularEventMessage(cbk));
}


void Instance::addPeriodicCallback(	const std::shared_ptr<ScheduledCallback>& cbk, 
									Priority prior, 
									Duration period ) 
{
	m_impl->addPeriodicCallback(cbk, prior, period );
	ZUAZO_LOG(*this, Severity::VERBOSE, m_impl->generateAddPeriodicEventMessage(cbk, prior, period));
}

void Instance::removePeriodicCallback(const std::shared_ptr<ScheduledCallback>& cbk) {
	m_impl->removePeriodicCallback(cbk);
	ZUAZO_LOG(*this, Severity::VERBOSE, m_impl->generateRemovePeriodicEventMessage(cbk));
}

TimePoint Instance::getTime() const noexcept {
	return m_impl->getTime();
}

TimePoint Instance::getEpoch() const noexcept {
	return m_impl->getEpoch();
}

Duration Instance::getDeltaT() const noexcept {
	return m_impl->getDeltaT();
}



void Instance::lock() noexcept {
	m_impl->lock();
}

bool Instance::try_lock() noexcept {
	return m_impl->try_lock();
}

void Instance::unlock() noexcept {
	m_impl->unlock();
}


uint32_t Instance::defaultDeviceScoreFunc(	const vk::DispatchLoaderDynamic& disp,
											vk::PhysicalDevice device ) noexcept
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

Instance::Module::Module(std::string name, Version version) noexcept
	: m_name(std::move(name))
	, m_version(version)
{
}

const std::string& Instance::Module::getName() const noexcept {
	return m_name;
}

Version Instance::Module::getVersion() const noexcept {
	return m_version;
}


void Instance::Module::initialize(Instance&) const {}

void Instance::Module::terminate(Instance&) const {}

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
											ElementLogFunc elementLogFunc ) noexcept
	: m_name(std::move(name))
	, m_version(version)
	, m_verbosity(verbosity)
	, m_modules(std::move(modules))
	, m_instanceLogFunc(std::move(instanceLogFunc))
	, m_elementLogFunc(std::move(elementLogFunc))
{
}



const std::string& Instance::ApplicationInfo::getName() const noexcept {
	return m_name;
}

Version Instance::ApplicationInfo::getVersion() const noexcept {
	return m_version;
}

Verbosity Instance::ApplicationInfo::getVerbosity() const noexcept {
	return m_verbosity;
}

const Instance::ApplicationInfo::Modules& Instance::ApplicationInfo::getModules() const noexcept {
	return m_modules;
}

const Instance::ApplicationInfo::InstanceLogFunc& Instance::ApplicationInfo::getInstanceLogFunc() const noexcept {
	return m_instanceLogFunc;
}

const Instance::ApplicationInfo::ElementLogFunc& Instance::ApplicationInfo::getElementLogFunc() const noexcept {
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
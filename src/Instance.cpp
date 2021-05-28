#include <zuazo/Instance.h>

#include "Timing/MainLoop.h"
#include "Timing/Scheduler.h"
#include "Timing/EventQueue.h"

#include <zuazo/Zuazo.h>
#include <zuazo/ZuazoBase.h>
#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Utils/Bit.h>

#include <mutex>
#include <iostream>
#include <sstream>
#include <type_traits>

namespace Zuazo {

struct Instance::Impl {
	Instance&						instance;

	ApplicationInfo					applicationInfo;

	Graphics::Vulkan				vulkan;

	std::mutex						mutex;
	Timing::Scheduler				scheduler;
	Timing::EventQueue				eventQueue;
	Timing::MainLoop				loop;

	Utils::Discrete<ColorFormat>	formatSupport;
	Utils::Discrete<DepthStencilFormat>	depthStencilFormatSupport;
	Utils::Range<Resolution>		resolutionSupport;

	ScheduledCallback 				processEventsCallback;
	ScheduledCallback 				presentImagesCallback;

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
		, eventQueue()
		, loop(scheduler, mutex)
		, formatSupport(queryFormatSupport(vulkan))
		, depthStencilFormatSupport(queryDepthStencilSupport(vulkan))
		, resolutionSupport(queryResolutionSupport(vulkan))
		, processEventsCallback(createEventProcessingCallback(eventQueue))
		, presentImagesCallback(createPresentCallback(vulkan))
	{
		std::lock_guard<Impl> lock(*this);
		addRegularCallback(processEventsCallback, eventHandlingPriority);
		addRegularCallback(presentImagesCallback, presentPriority);

		for(const Module& module : applicationInfo.getModules()) {
			module.initialize(instance);
		}
	}

	~Impl() {
		std::lock_guard<Impl> lock(*this);

		for(const Module& module : applicationInfo.getModules()) {
			module.terminate(instance);
		}

		removeRegularCallback(presentImagesCallback);
		removeRegularCallback(processEventsCallback);
	}

	const Instance::ApplicationInfo& getApplicationInfo() const noexcept {
		return applicationInfo;
	}

	const Graphics::Vulkan& getVulkan() const noexcept {
		return vulkan;
	}

	const Utils::Discrete<ColorFormat>& getFormatSupport() const noexcept {
		return formatSupport;
	}

	const Utils::Discrete<DepthStencilFormat>& getDepthStencilFormatSupport() const noexcept {
		return depthStencilFormatSupport;
	}

	const Utils::Range<Resolution>& getResolutionSupport() const noexcept {
		return resolutionSupport;
	}


	void addRegularCallback(const ScheduledCallback& cbk, Priority prior) {
		scheduler.addRegularCallback(cbk, prior);
		loop.interrupt();
		ZUAZO_LOG(instance, Severity::verbose, generateAddRegularEventMessage(cbk, prior));
	}

	void removeRegularCallback(const ScheduledCallback& cbk) {
		scheduler.removeRegularCallback(cbk);
		loop.interrupt();
		ZUAZO_LOG(instance, Severity::verbose, generateRemoveRegularEventMessage(cbk));
	}

	void addPeriodicCallback(const ScheduledCallback& cbk, Priority prior, Duration period) {
		scheduler.addPeriodicCallback(cbk, prior, period);
		loop.interrupt();
		ZUAZO_LOG(instance, Severity::verbose, generateAddPeriodicEventMessage(cbk, prior, period));
	}

	void removePeriodicCallback(const ScheduledCallback& cbk) {
		scheduler.removePeriodicCallback(cbk);
		loop.interrupt();
		ZUAZO_LOG(instance, Severity::verbose, generateRemovePeriodicEventMessage(cbk));
	}

	void addEvent(size_t emitterId, ScheduledCallback cbk) {
		eventQueue.addEvent(emitterId, std::move(cbk));
		loop.interrupt();
	}

	void removeEvent(size_t emitterId) {
		eventQueue.removeEvent(emitterId);
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
		message << "\t- Supported color formats:\n";
		for(const auto& fmt : formatSupport){
			message << "\t\t- " << fmt << "\n";
		}

		//Show resolution limits
		message << "\t- Supported resolutions: " << "\n";
		message << "\t\t-min: " << resolutionSupport.getMin() << "\n";
		message << "\t\t-max: " << resolutionSupport.getMax() << "\n";

		return message.str();
	}

	std::string generateAddRegularEventMessage(const ScheduledCallback& cbk, Priority prior) const {
		std::ostringstream message;

		message << "Regular event added: " << &cbk << " ";
		message << "(Priority: " << prior << ")";

		return message.str();
	}
	
	std::string generateRemoveRegularEventMessage(const ScheduledCallback& cbk) const {
		std::ostringstream message;

		message << "Regular event removed: " << &cbk << " ";

		return message.str();
	}

	std::string generateAddPeriodicEventMessage(const ScheduledCallback& cbk, Priority prior, Duration period) const {
		std::ostringstream message;

		message << "Periodic event added: " << &cbk << " ";
		message << "(Priority: " << prior << ", ";
		message << "Rate: " << getRate(period) << " Hz)";

		return message.str();
	}

	std::string generateRemovePeriodicEventMessage(const ScheduledCallback& cbk) const {
		std::ostringstream message;

		message << "Periodic event removed: " << &cbk << " ";

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

	static Utils::Discrete<ColorFormat> queryFormatSupport(const Graphics::Vulkan& vulkan) {
		Utils::Discrete<ColorFormat> result;

		//List the formats
		constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
			vk::FormatFeatureFlagBits::eSampledImage ;
		const auto& supportedFormats = vulkan.listSupportedFormatsOptimal(DESIRED_FLAGS);
		assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend())); //For binary_search

		for(ColorFormat i = Utils::EnumTraits<ColorFormat>::first(); i <= Utils::EnumTraits<ColorFormat>::last(); ++i) {
			//Convert it into a vulkan format
			const auto conversion = Graphics::toVulkan(i);

			//Find the end of the range
			const auto endIte = std::find_if(
				conversion.cbegin(), conversion.cend(),
				[] (const std::tuple<vk::Format, vk::ComponentMapping>& conv) -> bool {
					return std::get<0>(conv) == vk::Format::eUndefined;
				}
			);

			//Check if it is supported
			const auto supported = std::all_of(
				conversion.cbegin(), endIte,
				[&supportedFormats] (const std::tuple<vk::Format, vk::ComponentMapping>& conv) -> bool {
					return std::binary_search(supportedFormats.cbegin(), supportedFormats.cend(), std::get<0>(conv));
				}
			);

			if(supported) {
				result.push_back(i);
			}
		}

		assert(std::is_sorted(result.cbegin(), result.cend()));
		return result;
	}

	static Utils::Discrete<DepthStencilFormat> queryDepthStencilSupport(const Graphics::Vulkan& vulkan) {
		Utils::Discrete<DepthStencilFormat> result;

		//List the formats
		constexpr vk::FormatFeatureFlags DESIRED_FLAGS =
			vk::FormatFeatureFlagBits::eDepthStencilAttachment ;
		const auto& supportedFormats = vulkan.listSupportedFormatsOptimal(DESIRED_FLAGS);

		//Convert all the formats
		result.reserve(supportedFormats.size());
		for(const auto& format : supportedFormats) {
			const auto conversion = Graphics::fromVulkanDepthStencil(format);
			if(conversion != DepthStencilFormat::none) {
				result.push_back(conversion);
			}
		}

		//Sort the result
		std::sort(result.begin(), result.end());
		return result;
	}

	static Utils::Range<Resolution> queryResolutionSupport(const Graphics::Vulkan& vulkan) {
		const auto& limits = vulkan.getPhysicalDeviceProperties().limits;
		return Utils::Range<Resolution>(
			Resolution(1, 1),
			Resolution(limits.maxImageDimension2D, limits.maxImageDimension2D)
		);
	}

	static ScheduledCallback createEventProcessingCallback(Timing::EventQueue& eventQueue) {
		return std::bind(&Timing::EventQueue::process, std::ref(eventQueue));
	}

	static ScheduledCallback createPresentCallback(const Graphics::Vulkan& vulkan) {
		return std::bind(&Graphics::Vulkan::presentAll, std::cref(vulkan));
	}

};


Instance::Instance(	ApplicationInfo applicationInfo,
					const DeviceScoreFunc& deviceScoreFunc )
	: m_impl({}, *this, std::move(applicationInfo), deviceScoreFunc)
{
	ZUAZO_LOG(*this, Severity::info, m_impl->generateInitMessage());
}

Instance::~Instance() {
	ZUAZO_LOG(*this, Severity::info, "Terminated");
}

const Instance::ApplicationInfo& Instance::getApplicationInfo() const noexcept {
	return m_impl->getApplicationInfo();
}

const Graphics::Vulkan& Instance::getVulkan() const noexcept {
	return m_impl->getVulkan();
}

const Utils::Discrete<ColorFormat>& Instance::getFormatSupport() const noexcept {
	return m_impl->getFormatSupport();
}

const Utils::Discrete<DepthStencilFormat>& Instance::getDepthStencilFormatSupport() const noexcept {
	return m_impl->getDepthStencilFormatSupport();
}

const Utils::Range<Resolution>& Instance::getResolutionSupport() const noexcept {
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
									Duration period )
{
	m_impl->addPeriodicCallback(cbk, prior, period);
}

void Instance::removePeriodicCallback(const ScheduledCallback& cbk) {
	m_impl->removePeriodicCallback(cbk);
}

void Instance::addEvent(size_t emitterId, ScheduledCallback cbk) {
	m_impl->addEvent(emitterId, std::move(cbk));
}

void Instance::removeEvent(size_t emitterId) {
	m_impl->removeEvent(emitterId);
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
		for(auto i = range.getMin(); i <= range.getMax(); i = static_cast<vk::Format>(static_cast<int>(i) + 1)) {
			const auto format = static_cast<vk::Format>(i);
			const auto formatProperties = device.getFormatProperties(format, disp);

			score += Utils::flagCount(formatProperties.linearTilingFeatures);
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
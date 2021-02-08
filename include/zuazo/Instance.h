#pragma once

#include "Macros.h"
#include "Version.h"
#include "Verbosity.h"
#include "Resolution.h"
#include "ColorFormat.h"
#include "Graphics/Vulkan.h"
#include "Chrono.h"
#include "Utils/Pimpl.h"
#include "Utils/Limit.h"

#include <functional>
#include <vector>
#include <limits>
#include <memory>


namespace Zuazo {

class ZuazoBase;

class Instance {
public:
	using DeviceScoreFunc = Graphics::Vulkan::DeviceScoreFunc;
	using ScheduledCallback = std::function<void()>;

	class Module;
	class ApplicationInfo;

	/**
	 * Priority defines in which order Events will be updated, high priority meaning that
	 * it will be updated early, whilst low priority means that it will be updated late.
	 */
	using Priority = int32_t;

	enum {
		LOWEST_PRIORITY = std::numeric_limits<Priority>::min(),
		HIGHEST_PRIORITY = std::numeric_limits<Priority>::max(),

		OUTPUT_PRIORITY = LOWEST_PRIORITY / 2,
		PROCESSOR_PRIORITY = 0,
		INPUT_PRIORITY = HIGHEST_PRIORITY / 2,

		PLAYER_PRIORITY = OUTPUT_PRIORITY - 2048,
		PRESENT_PRIORITY = OUTPUT_PRIORITY - 1024,
		EVENT_HANDLING_PRIORITY = INPUT_PRIORITY + 1024,
	};

	Instance(	ApplicationInfo applicationInfo, 
				const DeviceScoreFunc& deviceScoreFunc = defaultDeviceScoreFunc);
	Instance(const Instance& other) = delete;
	Instance(Instance&& other) = delete;
	~Instance();

	Instance& 							operator=(const Instance& other) = delete;
	Instance& 							operator=(Instance&& other) = delete;

	const ApplicationInfo&				getApplicationInfo() const noexcept;
	const Graphics::Vulkan&				getVulkan() const noexcept;

	const Utils::Discrete<ColorFormat>& getFormatSupport() const noexcept;
	const Utils::Range<Resolution>&		getResolutionSupport() const noexcept;

	void								addRegularCallback(	const ScheduledCallback& cbk, 
															Priority prior );
	void								removeRegularCallback(const ScheduledCallback& cbk);

	void								addPeriodicCallback(	const ScheduledCallback& cbk, 
																Priority prior, 
																Duration period );
	void								removePeriodicCallback(const ScheduledCallback& cbk);

	void								addEvent(size_t emitterId, ScheduledCallback cbk);
	void								removeEvent(size_t emitterId);

	TimePoint							getTime() const noexcept;
	TimePoint							getEpoch() const noexcept;
	Duration							getDeltaT() const noexcept;

	void								lock() noexcept;
	bool								try_lock() noexcept;
	void								unlock() noexcept;

	static uint32_t						defaultDeviceScoreFunc(	const vk::DispatchLoaderDynamic& disp, 
																vk::PhysicalDevice device ) noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>					m_impl;

};



class Instance::Module {
	friend Instance;
public:
	using VulkanExtensions = std::vector<vk::ExtensionProperties>;

	Module(std::string name, Version version) noexcept;
	Module(const Module& other) = default;
	Module(Module&& other) noexcept = default;
	virtual ~Module() = default;

	Module& operator=(const Module& other) = default;
	Module& operator=(Module&& other) noexcept = default;

	const std::string&					getName() const noexcept;
	Version								getVersion() const noexcept;

private:
	std::string 						m_name;
	Version 							m_version;

	virtual void 						initialize(Instance& instance) const;
	virtual void 						terminate(Instance& instance) const;

	//Vulkan related
	virtual VulkanExtensions			getRequiredVulkanInstanceExtensions() const;
	virtual VulkanExtensions			getRequiredVulkanDeviceExtensions() const;
	virtual bool						getPresentationSupport(	vk::Instance  instance, 
																vk::PhysicalDevice device, 
																uint32_t queueIndex ) const;
};



class Instance::ApplicationInfo {
public:
	using Modules = std::vector<std::reference_wrapper<const Module>>;
	using InstanceLogFunc = std::function<void(const Instance&, Severity, std::string_view)>;
	using ElementLogFunc = std::function<void(const ZuazoBase&, Severity, std::string_view)>;

	explicit ApplicationInfo(	std::string name = "Zuazo Application",
								Version version = Version(0, 1, 0),
								Verbosity verbosity = ZUAZO_IS_DEBUG ? Verbosity::GEQ_INFO : Verbosity::SILENT,
								Modules modules = {},
								InstanceLogFunc	instanceLogFunc = defaultInstanceLogFunc,
								ElementLogFunc elementLogFunc = defaultElementLogFunc ) noexcept;
	ApplicationInfo(const ApplicationInfo& other) = default;
	ApplicationInfo(ApplicationInfo&& other) noexcept = default;
	~ApplicationInfo() = default;

	ApplicationInfo& 					operator=(const ApplicationInfo& other) = default;
	ApplicationInfo& 					operator=(ApplicationInfo&& other) noexcept = default;

	const std::string&					getName() const noexcept;
	Version								getVersion() const noexcept;
	Verbosity							getVerbosity() const noexcept;
	const Modules&						getModules() const noexcept;
	const InstanceLogFunc&				getInstanceLogFunc() const noexcept;
	const ElementLogFunc&				getElementLogFunc() const noexcept;


	static void 						defaultInstanceLogFunc(	const Instance& inst, 
																Severity severity, 
																std::string_view msg );

	static void 						defaultElementLogFunc(	const ZuazoBase& base, 
																Severity severity, 
																std::string_view msg );

private:
	std::string							m_name;
	Version								m_version;
	Verbosity							m_verbosity;
	Modules								m_modules;
	InstanceLogFunc						m_instanceLogFunc;
	ElementLogFunc						m_elementLogFunc;
};


#define ZUAZO_LOG(instance, severity, message)													\
	if(	(instance).getApplicationInfo().getInstanceLogFunc() && 								\
		(((instance).getApplicationInfo().getVerbosity()) & (severity)) != Verbosity::SILENT ) 	\
	{																							\
		(instance).getApplicationInfo().getInstanceLogFunc()(instance, severity, message);		\
	}

}
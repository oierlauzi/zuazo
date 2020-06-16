#pragma once

#include "Macros.h"
#include "Version.h"
#include "Verbosity.h"
#include "Video.h"
#include "Graphics/Vulkan.h"
#include "Timing/Chrono.h"
#include "Utils/Pimpl.h"

#include <functional>
#include <vector>
#include <limits>
#include <memory>


namespace Zuazo {

class ZuazoBase;

class Instance {
public:
	using InstanceLogFunc = std::function<void(const Instance&, Severity, std::string_view)>;
	using ElementLogFunc = std::function<void(const ZuazoBase&, Severity, std::string_view)>;
	using DeviceScoreFunc = Graphics::Vulkan::DeviceScoreFunc;
	using ScheduledCallback = std::function<void()>;

	struct ApplicationInfo {
		std::string		name = "Zuazo Application";
		Version			version = Version(0, 1, 0);
		Verbosity		verbosity = ZUAZO_IS_DEBUG ? Verbosity::LEQ_INFO : Verbosity::SILENT;
		VideoMode		defaultVideoMode = Instance::defaultVideoMode;
		InstanceLogFunc	instanceLogFunc = defaultInstanceLogFunc;
		ElementLogFunc	elementLogFunc = defaultElementLogFunc;
	};
	
	struct FormatSupport {
		std::vector<ColorFormat> inputFormats;
		std::vector<ColorFormat> outputFormats;
	};

	struct ResolutionSupport {
		Resolution maxInputResolution;
		Resolution maxOutputResolution;
	};

	/**
	 * Priority defines in which order Events will be updated, high priority meaning that
	 * it will be updated early, whilst low priority means that it will be updated late.
	 */
	using Priority = int32_t;

	enum {
		LOWEST_PRIORITY = std::numeric_limits<Priority>::min(),
		HIGHEST_PRIORITY = std::numeric_limits<Priority>::max(),
		OUTPUT_PRIORITY = LOWEST_PRIORITY / 2,
		PRESENT_PRIORITY = OUTPUT_PRIORITY - 64,
		PROCESSOR_PRIORITY = 0,
		INPUT_PRIORITY = HIGHEST_PRIORITY / 2,
	};

	Instance(	ApplicationInfo&& applicationInfo, 
				const DeviceScoreFunc& deviceScoreFunc = defaultDeviceScoreFunc);
	Instance(const Instance& other) = delete;
	Instance(Instance&& other);
	~Instance();

	Instance& operator=(const Instance& other) = delete;
	Instance& operator=(Instance&& other);

	const ApplicationInfo&		getApplicationInfo() const;
	const Graphics::Vulkan&		getVulkan() const;

	const FormatSupport& 		getFormatSupport() const;
	const ResolutionSupport&	getResolutionSupport() const;

	void						addRegularCallback(const std::shared_ptr<ScheduledCallback>& cbk, Priority prior);
	void						removeRegularCallback(const std::shared_ptr<ScheduledCallback>& cbk);

	void						addPeriodicCallback(const std::shared_ptr<ScheduledCallback>& cbk, Priority prior, Timing::Duration period);
	void						removePeriodicCallback(const std::shared_ptr<ScheduledCallback>& cbk);

	Timing::TimePoint			getTime() const;
	Timing::TimePoint			getEpoch() const;
	Timing::Duration			getDeltaT() const;

	void						lock();
	bool						try_lock();
	void						unlock();

	static void 				defaultInstanceLogFunc(	const Instance& inst, 
														Severity severity, 
														std::string_view msg );

	static void 				defaultElementLogFunc(	const ZuazoBase& base, 
														Severity severity, 
														std::string_view msg );

	static uint32_t				defaultDeviceScoreFunc(	const vk::DispatchLoaderDynamic& disp, 
														vk::PhysicalDevice device );

	static constexpr VideoMode 	defaultVideoMode = VideoMode(	Resolution(1280, 720),
																AspectRatio(1, 1),
																Timing::Rate(30, 1),
																ColorPrimaries::BT709,
																ColorModel::RGB,
																ColorTransferFunction::IEC61966_2_1,
																ColorSubsampling::NONE,
																ColorRange::FULL,
																ColorFormat::B8G8R8A8 );

private:
	struct Impl;
	Utils::Pimpl<Impl>			m_impl;

};


#define ZUAZO_LOG(instance, severity, message)												\
	if(	(instance).getApplicationInfo().instanceLogFunc && 									\
		((severity) & (instance).getApplicationInfo().verbosity) != Verbosity::SILENT ) 	\
	{																						\
		(instance).getApplicationInfo().instanceLogFunc(instance, severity, message);		\
	}

}
#pragma once

#include "Macros.h"
#include "Version.h"
#include "Verbosity.h"
#include "Video.h"
#include "Graphics/Vulkan.h"
#include "Timing/Chrono.h"
#include "Timing/EventBase.h"
#include "Utils/Pimpl.h"

#include <functional>
#include <vector>
#include <limits>


namespace Zuazo {

class Instance {
public:
	using LogFunc = std::function<void(const Instance&, Verbosity, std::string_view)>;
	using DeviceScoreFunc = Graphics::Vulkan::DeviceScoreFunc;

	struct ApplicationInfo {
		std::string		name;
		Version			version;
		Verbosity		verbosity = ZUAZO_IS_DEBUG ? Verbosity::INFO : Verbosity::SILENT;
		VideoMode		defaultVideoMode = {
							Resolution(1280, 720),
							AspectRatio(1, 1),
							Timing::Rate(30, 1),
							ColorPrimaries::BT709,
							ColorModel::RGB,
							ColorTransferFunction::IEC61966_2_1,
							ColorSubsampling::NONE,
							ColorRange::FULL,
							ColorFormat::B8G8R8A8
						};
		LogFunc			logFunc = defaultLogFunc;
		DeviceScoreFunc	deviceScoreFunc = defaultDeviceScoreFunc;
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
	enum class Priority : int {
		LOWEST_PRIORITY = std::numeric_limits<int>::min(),
		HIGHEST_PRIORITY = std::numeric_limits<int>::max(),
		OUTPUT_PRIORITY = LOWEST_PRIORITY / 2,
		PROCESSOR_PRIORITY = 0,
		INPUT_PRIORITY = HIGHEST_PRIORITY / 2,
	};

	Instance(ApplicationInfo&& applicationInfo);
	Instance(const Instance& other) = delete;
	Instance(Instance&& other);
	~Instance();

	Instance& operator=(const Instance& other) = delete;
	Instance& operator=(Instance&& other);

	const ApplicationInfo&		getApplicationInfo() const;
	const Graphics::Vulkan&		getVulkan() const;

	const FormatSupport& 		getFormatSupport() const;
	const ResolutionSupport&	getResolutionSupport() const;

	void						addRegularEvent(const Timing::EventBase& event, Priority prior);
	void						removeRegularEvent(const Timing::EventBase& event);

	void						addPeriodicEvent(const Timing::EventBase& event, Priority prior, Timing::Duration period);
	void						removePeriodicEvent(const Timing::EventBase& event);

	Timing::TimePoint			getTime() const;
	Timing::TimePoint			getEpoch() const;
	Timing::Duration			getDeltaT() const;

	static void 				defaultLogFunc(	const Instance& inst, 
												Verbosity severity, 
												std::string_view msg );

	static uint32_t				defaultDeviceScoreFunc(	const vk::DispatchLoaderDynamic& disp, 
														vk::PhysicalDevice device );

private:
	struct Impl;
	Utils::Pimpl<Impl>			m_impl;

};

#define ZUAZO_LOG(instance, severity, message)									\
	if(																			\
		(instance).getApplicationInfo().logFunc && 								\
		((severity) <= (instance).getApplicationInfo().verbosity) ) 			\
	{																			\
		(instance).getApplicationInfo().logFunc(instance, severity, message);	\
	}

}
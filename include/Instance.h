#pragma once

#include "Macros.h"
#include "Version.h"
#include "Verbosity.h"
#include "ColorFormat.h"
#include "VideoMode.h"
#include "Utils/Pimpl.h"
#include "Graphics/Vulkan.h"
#include "Timing/Scheduler.h"
#include "Timing/MainLoop.h"

#include <functional>
#include <vector>

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

	Instance(ApplicationInfo&& applicationInfo);
	Instance(const Instance& other) = delete;
	Instance(Instance&& other);
	~Instance();

	Instance& operator=(const Instance& other) = delete;
	Instance& operator=(Instance&& other);

	const ApplicationInfo&		getApplicationInfo() const;
	const Graphics::Vulkan&		getVulkan() const;
	const Timing::Scheduler&	getScheduler() const;
	const Timing::MainLoop&		getMainLoop() const;

	const FormatSupport& 		getFormatSupport() const;
	const ResolutionSupport&	getResolutionSupport() const;

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
#pragma once

#include "Macros.h"
#include "Version.h"
#include "Verbosity.h"
#include "ColorFormat.h"
#include "VideoMode.h"
#include "ColorEncoding.h"
#include "Timing/MainLoop.h"
#include "Graphics/GLFW.h"
#include "Graphics/Vulkan.h"

#include <functional>

namespace Zuazo {

class Instance {
public:
	using LogFunc = std::function<void(const Instance&, Verbosity, const std::string_view&)>;
	using DeviceScoreFunc = Graphics::Vulkan::DeviceScoreFunc;

	struct ApplicationInfo {
		std::string		name;
		std::string		description;
		Version			version;
		Verbosity		verbosity = ZUAZO_IS_DEBUG ? Verbosity::INFO : Verbosity::SILENT;
		VideoMode		defaultVideoMode = {
							Resolution(1280, 720),
							AspectRatio(1, 1),
							Timing::Rate(30, 1),
							ColorPrimaries::BT709,
							ColorModel::RGB,
							ColorEncoding::UINT,
							ColorFormat::B8G8R8A8
						};
		LogFunc			logFunc = defaultLogFunc;
		DeviceScoreFunc	deviceScoreFunc = defaultDeviceScoreFunc;
	};

	Instance(ApplicationInfo&& applicationInfo);
	Instance(const Instance& other) = delete;
	Instance(Instance&& other) = delete;
	~Instance() = default;

	Instance& operator=(const Instance& other) = delete;
	Instance& operator=(Instance&& other) = delete;

	const ApplicationInfo&		getApplicationInfo() const;

	void						log(Verbosity severity, const std::string_view& msg) const;

	const Graphics::Vulkan&		getVulkan() const;
	Graphics::Vulkan&			getVulkan();

	const Timing::MainLoop&		getMainLoop() const;
	Timing::MainLoop&			getMainLoop();

	const std::vector<std::tuple<ColorFormat, ColorEncoding>>& getSupportedInputFormats() const;
	const std::vector<std::tuple<ColorFormat, ColorEncoding>>& getSupportedInputYcbcrFormats() const;
	const std::vector<std::tuple<ColorFormat, ColorEncoding>>& getSupportedOutputFormats() const;

	static void 				defaultLogFunc(	const Instance& inst, 
												Verbosity severity, 
												const std::string_view& msg );

	static uint32_t				defaultDeviceScoreFunc(	const vk::DispatchLoaderDynamic& disp, 
														vk::PhysicalDevice device );
private:
	ApplicationInfo				m_applicationInfo;
	Graphics::GLFW				m_glfw;
	Graphics::Vulkan			m_vulkan;
	Timing::MainLoop			m_loop;

	std::vector<std::tuple<ColorFormat, ColorEncoding>> m_inputFormats;
	std::vector<std::tuple<ColorFormat, ColorEncoding>> m_inputYcbcrFormats;
	std::vector<std::tuple<ColorFormat, ColorEncoding>> m_outputFormats;

	void 						setupSupportedFormats();
	std::string					generateInitMessage() const;
};

}
#pragma once

#include "Zuazo.h"
#include "Timing/MainLoop.h"
#include "Graphics/Window.h"
#include "Graphics/Vulkan.h"

namespace Zuazo {

class Instance {
public:
	struct ApplicationInfo {
		std::string							name;
		std::string							description;
		Version								version = {};
		bool								isDebug = true;
		Graphics::Vulkan::DeviceScoreFunc	deviceScoreFunc = Graphics::Vulkan::defaultDeviceScoreFunc;
	};

	Instance(ApplicationInfo&& applicationInfo);
	Instance(const Instance& other) = delete;
	Instance(Instance&& other) = delete;
	~Instance() = default;

	Instance& operator=(const Instance& other) = delete;
	Instance& operator=(Instance&& other) = delete;

	const ApplicationInfo&		getApplicationInfo() const;

	const Graphics::Vulkan&		getVulkan() const;
	Graphics::Vulkan&			getVulkan();

	const Timing::MainLoop&		getMainLoop() const;
	Timing::MainLoop&			getMainLoop();
private:
	ApplicationInfo				m_applicationInfo;
	Graphics::Window::Instance	m_windowInstance;
	Graphics::Vulkan			m_vulkan;
	Timing::MainLoop			m_loop;

	static Graphics::Vulkan::Verbosity	getVulkanVerbosity(bool isDebug);
};

}
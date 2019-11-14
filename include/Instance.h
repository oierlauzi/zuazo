#pragma once

#include "Timing/MainLoop.h"
#include "Graphics/Window.h"
#include "Graphics/Vulkan.h"

namespace Zuazo {

class Instance {
public:
	Instance() = default;
	Instance(const Instance& other) = delete;
	Instance(Instance&& other) = delete;
	~Instance() = default;

	Instance& operator=(const Instance& other) = delete;
	Instance& operator=(Instance&& other) = delete;

	const Graphics::Vulkan&		getVulkan() const;
	Graphics::Vulkan&			getVulkan();

	const Timing::MainLoop&		getMainLoop() const;
	Timing::MainLoop&			getMainLoop();
private:
	Graphics::Window			m_windowInstance;
	Graphics::Vulkan			m_vulkan;
	Timing::MainLoop			m_loop;
};

}
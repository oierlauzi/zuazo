#pragma once

#include "Window.h"
#include "Vulkan/Instance.h"
#include "Vulkan/Messenger.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/Device.h"

#include <atomic>
#include <optional>
#include <functional>

namespace Zuazo::Graphics {

class Instance {
	public:
		Instance();
		Instance(const Instance& other) = delete;
		~Instance() = default;

		Vulkan::Instance&					getVulkanInstance();
		Vulkan::Messenger&					getMessenger();
		Vulkan::PhysicalDevice&				getPhysicalDevice();
		Vulkan::Device&						getDevice();

		static std::function<uint32_t(const Vulkan::PhysicalDevice)> deviceScoreFunc;

	private:
		Window::Instance					m_windowInstance;
		Vulkan::Instance					m_vulkanInstance;
		std::optional<Vulkan::Messenger>	m_messenger;
		Vulkan::PhysicalDevice				m_physicalDevice;
		Vulkan::Device						m_device;

		Vulkan::Queue						m_graphicsQueue;
		Vulkan::Queue						m_computeQueue;
		Vulkan::Queue						m_tranferQueue;
		Vulkan::Queue						m_presentationQueue;

		static std::vector<Vulkan::Extension>		getRequiredExtensions();
		static std::vector<Vulkan::ValidationLayer> getRequieredValidationLayers();
		static std::vector<Vulkan::QueueFamily>		getRequieredQueueFamilies(const Vulkan::PhysicalDevice& dev);

		static Vulkan::PhysicalDevice		getBestPhysicalDevice(const std::vector<Vulkan::PhysicalDevice>& devices);
		static uint32_t						getPhysicalDeviceScore(const Vulkan::PhysicalDevice& dev);
		static bool							getPhysicalDeviceSupport(const Vulkan::PhysicalDevice& dev);
		static int32_t						defaultDeviceScoreFunc(const Vulkan::PhysicalDevice& dev);
};

}
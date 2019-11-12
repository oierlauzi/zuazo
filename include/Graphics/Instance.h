#pragma once

#include "Window.h"
#include "Vulkan/Instance.h"
#include "Vulkan/Messenger.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/Device.h"
#include "Vulkan/Queue.h"

#include <atomic>
#include <optional>
#include <functional>

namespace Zuazo::Graphics {

class Instance {
	public:
		using DeviceScoreFunc = std::function<uint32_t(const Vulkan::PhysicalDevice)>;

		Instance();
		Instance(const Instance& other) = delete;
		~Instance() = default;

		Vulkan::Instance&							getVulkanInstance();
		Vulkan::Messenger&							getMessenger();
		Vulkan::PhysicalDevice&						getPhysicalDevice();
		Vulkan::Device&								getDevice();
		Vulkan::Queue								getGraphicsQueue();
		Vulkan::Queue								getComputeQueue();
		Vulkan::Queue								getTranferQueue();
		Vulkan::Queue								getPresentationQueue();

		static DeviceScoreFunc 						deviceScoreFunc;

	private:
		enum QueueIndices {
			GRAPHICS_QUEUE,
			COMPUTE_QUEUE,
			TRANSFER_QUEUE,
			PRESENTATION_QUEUE,
			QUEUE_COUNT
		};

		Window::Instance							m_windowInstance;
		Vulkan::Instance							m_vulkanInstance;
		std::optional<Vulkan::Messenger>			m_messenger;
		Vulkan::PhysicalDevice						m_physicalDevice;
		Vulkan::Device								m_device;
		std::vector<std::vector<Vulkan::Queue>>		m_queues;

		static std::vector<Vulkan::ValidationLayer> getRequiredValidationLayers();
		static std::vector<Vulkan::Extension>		getRequiredInstanceExtensions();
		static std::vector<Vulkan::QueueFamily>		getRequiredQueueFamilies(Vulkan::Instance& inst, Vulkan::PhysicalDevice& dev);
		static std::vector<Vulkan::Extension>		getRequiredDeviceExtensions();

		static std::optional<Vulkan::Messenger>		getMessenger(Vulkan::Instance& inst);

		static Vulkan::PhysicalDevice				getBestPhysicalDevice(Vulkan::Instance& inst);
		static uint32_t								getPhysicalDeviceScore(const Vulkan::PhysicalDevice& dev);
		static bool									getPhysicalDeviceSupport(Vulkan::Instance& inst, Vulkan::PhysicalDevice& dev);
		static int32_t								defaultDeviceScoreFunc(const Vulkan::PhysicalDevice& dev);
};

}
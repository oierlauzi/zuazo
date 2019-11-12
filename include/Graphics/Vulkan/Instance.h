#pragma once 

#include "Extension.h"
#include "ValidationLayer.h"
#include "PhysicalDevice.h"

#include <vector>
#include <string>

#include <vulkan/vulkan.h>

namespace Zuazo::Graphics::Vulkan {

class PhysicalDevice;

class Instance {
public:
	Instance(const std::vector<Extension>& ext, const std::vector<ValidationLayer>& vl);
	Instance(const Instance& other) = delete;
	~Instance();

	VkInstance							get();

	std::vector<PhysicalDevice>			getPhysicalDevices() const;

	template<typename F>
	F									getFunction(const std::string& name) const;

private:
	VkInstance							m_instance = nullptr;

	static std::vector<Extension> 		getAvailableExtensions();
	static std::vector<ValidationLayer> getAvailableValidationLayers();
	static std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance inst);
};

}

#include "Instance.inl"
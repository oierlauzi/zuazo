#pragma once 

#include <vector>
#include <string>

#include <vulkan/vulkan.h>

namespace Zuazo::Graphics::Vulkan {

class PhysicalDevice;

using Extension = VkExtensionProperties;
using ValidationLayer = VkLayerProperties;

class Instance {
public:
	Instance(const std::vector<Extension>& ext, const std::vector<ValidationLayer>& vl);
	Instance(const Instance& other) = delete;
	~Instance();

	VkInstance							get();

	const std::vector<PhysicalDevice>&	getPhysicalDevices() const;

	template<typename F>
	F									getFunction(const std::string& name) const;
private:
	VkInstance							m_instance = nullptr;
	std::vector<PhysicalDevice>			m_physicalDevices;

	static std::vector<Extension>		getAvailableExtensions();
	static std::vector<Extension>		getExtensions(std::vector<Extension> required);
	static std::vector<const char*>		getNames(const std::vector<VkExtensionProperties>& ext);

	static std::vector<ValidationLayer> getAvailableValidationLayers();
	static std::vector<ValidationLayer> getValidationLayers(std::vector<ValidationLayer> required);
	static std::vector<const char*>		getNames(const std::vector<ValidationLayer>& vl);
};

}

#include "Instance.inl"
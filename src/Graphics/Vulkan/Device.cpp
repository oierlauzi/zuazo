#include <Graphics/Vulkan/Device.h>

#include <Exception.h>

#include <cassert>

namespace Zuazo::Graphics::Vulkan {

Device::Device(PhysicalDevice& dev, const std::vector<QueueFamily>& queueFamilies, const std::vector<Extension>& extensions, const std::vector<ValidationLayer>& validationLayers) :
	m_physicalDevice(dev)
{
	//Fill VkDeviceQueueCreateInfo according to the given families
	const float queuePriorities = 1.0f;

	const auto& availableQueueFamilies = m_physicalDevice.getQueueFamilies();
	const auto usedQueueFamilies = getUsedQueueFamilies(availableQueueFamilies, queueFamilies);
	const auto usedQueueFamilyIndices = getQueueFamilyIndices(availableQueueFamilies, usedQueueFamilies);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for(auto ite = usedQueueFamilyIndices.cbegin(); ite != usedQueueFamilyIndices.cend(); ++ite){
		VkDeviceQueueCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = ite->first;
		createInfo.queueCount = ite->second;
		createInfo.pQueuePriorities = &queuePriorities;

		queueCreateInfos.push_back(createInfo);
	}

	//Get all the extensions
	const auto availableExtensions = dev.getAvailableExtensions();
	const auto usedExtensions = getUsedExtensions(availableExtensions, extensions);
	const auto extensionNames = getNames(usedExtensions);

	//Get all the validation layers
	const auto availableValidationLayers = dev.getAvailableValidationLayers();
	const auto usedValidationLayers = getUsedValidationLayers(availableValidationLayers, validationLayers);
	const auto validationLayerNames = getNames(usedValidationLayers);


	//Fill the VkDeviceCreateInfo structure accordingly
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = queueCreateInfos.size();
	createInfo.ppEnabledExtensionNames = extensionNames.data();
	createInfo.enabledExtensionCount = extensionNames.size();
	createInfo.ppEnabledLayerNames = validationLayerNames.data();
	createInfo.enabledLayerCount = validationLayerNames.size();
	

	//Create the logical device
	VkResult err = vkCreateDevice(m_physicalDevice.get(), &createInfo, nullptr, &m_device);

	switch(err){
		case VK_SUCCESS:
			break; //Everything OK.

		default:
			throw Exception("Error creating Vulkan device");
	}

	//Query the queues
	m_queues.reserve(queueFamilies.size());
	for(const auto& queueFamily : queueFamilies){
		m_queues.emplace_back(std::vector<Queue>(queueFamily.queueCount));

		//Get the indices corresponding to this family
		//It might be distributed among more than one available family
		const auto currentQueueFamilies = getUsedQueueFamilies(
			availableQueueFamilies, 
			std::vector<QueueFamily>(1, queueFamily)
		);

		const auto indices = getQueueFamilyIndices(
			currentQueueFamilies, 
			usedQueueFamilies
		);

		//Iterate though the returned families and assign them to the queue vector
		size_t j = 0; //Queue # counter
		for(const auto& index : indices){
			for(size_t i = 0; i < index.second; i++){
				vkGetDeviceQueue(
					m_device, 				//Device
					index.first, 			//Family index
					i, 						//Queue index
					&(m_queues.back()[j])	//Where to save
				);
				j++;
			}
		}
				
		//Array should be full
		assert(j == m_queues.back().size());
	}
}

Device::~Device(){
	if(m_device){
		vkDestroyDevice(m_device, nullptr);
	}
}



VkDevice Device::get(){
	return m_device;
}

const std::vector<std::vector<Queue>>& Device::getQueues() const{
	return m_queues;
}

}
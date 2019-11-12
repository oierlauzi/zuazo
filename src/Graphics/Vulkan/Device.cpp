#include <Graphics/Vulkan/Device.h>

#include <Exception.h>

#include <cassert>

namespace Zuazo::Graphics::Vulkan {

Device::Device(PhysicalDevice& dev, const std::vector<QueueFamily>& queueFamilies) {
	const float queuePriorities = 1.0f;

	const auto& availableQueueFamilies = dev.getQueueFamilies();
	auto usedQueueFamilies = getQueueFamilies(availableQueueFamilies, queueFamilies);
	auto usedQueueFamilyIndices = getQueueFamilyIndices(availableQueueFamilies, usedQueueFamilies);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for(auto ite = usedQueueFamilyIndices.cbegin(); ite != usedQueueFamilyIndices.cend(); ++ite){
		VkDeviceQueueCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = ite->first;
		createInfo.queueCount = ite->second;
		createInfo.pQueuePriorities = &queuePriorities;

		queueCreateInfos.push_back(createInfo);
	}

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = queueCreateInfos.size();


	//Create the logical device
	VkResult err = vkCreateDevice(dev.get(), &createInfo, nullptr, &m_device);

	switch(err){
		case VK_SUCCESS:
			break; //Everything OK.

		default:
			throw Exception("Error creating Vulkan device");
	}

	//Query the queues
	m_queues.reserve(queueFamilies.size());
	for(size_t i = 0; i < queueFamilies.size(); i++){
		m_queues.emplace_back(std::vector<Queue>(queueFamilies[i].queueCount));

		//Get the indices corresponding to this family
		//It might be distributed among more than one available family
		const auto families = getQueueFamilies(
			availableQueueFamilies, 
			std::vector<QueueFamily>(1, queueFamilies[i])
		);

		const auto indices = getQueueFamilyIndices(
			availableQueueFamilies,
			families
		);

		//Iterate though the returned families and assign them to the queue vector
		size_t j = 0;
		for(const auto f : indices){
			for(size_t h = 0; h < f.second; h++){
				vkGetDeviceQueue(
					m_device, 				//Device
					f.first, 				//Family index
					h, 						//Queue index
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


const std::vector<std::vector<Queue>>& Device::getQueues(){
	return m_queues;
}


std::vector<QueueFamily> Device::getQueueFamilies(const std::vector<QueueFamily>& available, std::vector<QueueFamily> required){
	std::vector<QueueFamily> queueFamilies;

	auto ite = required.begin();
	while(ite != required.end()){
		//Check for requirement
		for(const auto& availableQf : available){
			if(isSuitable(ite->queueFlags, availableQf.queueFlags)){
				if(ite->queueCount > availableQf.queueCount){
					//Not enough of this queue family. Add the exisiting ones
					QueueFamily queue = availableQf;
					queueFamilies.push_back(queue);
					ite->queueCount -= availableQf.queueCount;
				} else {
					//Add only the required amount
					QueueFamily queue = availableQf;
					queue.queueCount = ite->queueCount;
					queueFamilies.push_back(queue);
					ite->queueCount = 0;
					break;
				}
			}
		}

		//Next requirement
		if(ite->queueCount == 0){
			ite = required.erase(ite);
		}else {
			++ite;
		}
	}

	if(required.size()){
		//There are missing queue families
		throw Exception("Missing queue families");
	}

	return queueFamilies;
}

std::map<uint32_t, uint32_t> Device::getQueueFamilyIndices(const std::vector<QueueFamily>& available, std::vector<QueueFamily> requested){
	std::map<uint32_t, uint32_t> result;

	for(const auto& req : requested){
		const size_t idx = getQueueFamilyIndex(available, req.queueFlags); //Should not throw
		result[idx] = std::max(result[idx], req.queueCount);
	}

	return result;
}

size_t Device::getQueueFamilyIndex(const std::vector<QueueFamily>& qf, uint32_t flags){
	for(size_t i = 0; i < qf.size(); i++){
		if(isSuitable(flags, qf[i].queueFlags) && (qf[i].queueCount > 0)) {
			return i;
		}
	}

	throw Exception("Vulkan queue family not found");
}

bool Device::isSuitable(uint32_t required, uint32_t available){
	return (required & available) == required;
}

}
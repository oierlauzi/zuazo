#include <Graphics/VulkanUtils.h>

#include <Exception.h>

namespace Zuazo::Graphics {

std::vector<vk::LayerProperties> getUsedLayers(	const std::vector<vk::LayerProperties>& available, 
												std::vector<vk::LayerProperties>& required )
{
	std::vector<vk::LayerProperties> layers;

	auto ite = required.begin();
	while(ite != required.end()){
		bool found = false;

		for(const auto& availableVl : available){
			if(std::strncmp(ite->layerName, availableVl.layerName, VK_MAX_EXTENSION_NAME_SIZE) == 0){
				if(ite->specVersion <= availableVl.specVersion){
					layers.push_back(availableVl);
					found = true;
					break;
				}
			}
		}

		//Advance to the next one
		if(found){
			ite = required.erase(ite);
		}else {
			++ite;
		}
	}

	return layers;
}

std::vector<const char*> getNames(const std::vector<vk::LayerProperties>& layers){
	std::vector<const char*> names(layers.size());

	for(size_t i = 0; i < names.size(); i++){
		names[i] = layers[i].layerName;
	}

	return names;
}




std::vector<vk::ExtensionProperties> getUsedExtensions(	const std::vector<vk::ExtensionProperties>& available, 
														std::vector<vk::ExtensionProperties>& required )
{
	std::vector<vk::ExtensionProperties> extensions;

	//Check for availability of all required extensions
	auto ite = required.begin();
	while(ite != required.end()){
		bool found = false;

		for(const auto& availableExt : available){
			if(std::strncmp(ite->extensionName, availableExt.extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0){
				if(ite->specVersion <= availableExt.specVersion){
					extensions.push_back(availableExt);
					found = true;
					break;
				}
			}
		}

		//Advance to the next one
		if(found){
			ite = required.erase(ite);
		}else {
			++ite;
		}
	}

	return extensions;
}

std::vector<const char*> getNames(const std::vector<vk::ExtensionProperties>& ext){
	std::vector<const char*> names(ext.size());

	for(size_t i = 0; i < names.size(); i++){
		names[i] = ext[i].extensionName;
	}

	return names;
}




std::vector<vk::QueueFamilyProperties> getUsedQueueFamilies(const std::vector<vk::QueueFamilyProperties>& available, 
															std::vector<vk::QueueFamilyProperties>& required )
{
	std::vector<vk::QueueFamilyProperties> queueFamilies;

	auto ite = required.begin();
	while(ite != required.end()){
		for(const auto& availableQf : available){
			if(getQueueFamilyCompatibility(ite->queueFlags, availableQf.queueFlags)){
				vk::QueueFamilyProperties queue = availableQf;

				if(ite->queueCount > availableQf.queueCount){
					//Not enough of this queue family. Add the exisiting ones
					queueFamilies.push_back(queue);
					ite->queueCount -= queue.queueCount;
				} else {
					//Add only the required amount
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
	
	return queueFamilies;
}

std::map<uint32_t, uint32_t> getQueueFamilyIndices(	const std::vector<vk::QueueFamilyProperties>& available, 
													const std::vector<vk::QueueFamilyProperties>& requested )
{
	std::map<uint32_t, uint32_t> result;

	for(const auto& req : requested){
		const size_t idx = getQueueFamilyIndex(available, req.queueFlags);
		result[idx] = std::max(result[idx], req.queueCount);
	}

	return result;
}

size_t getQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& qf, vk::QueueFlags flags){
	size_t i;

	for(i = 0; i < qf.size(); i++){
		if(getQueueFamilyCompatibility(flags, qf[i].queueFlags) && (qf[i].queueCount > 0)) {
			return i;
		}
	}

	throw Exception("Requested queue family not found!");
}

bool getQueueFamilyCompatibility(vk::QueueFlags required, vk::QueueFlags available){
	return (required & available) == required;
}




std::vector<std::vector<vk::Queue>>	getQueues(	const vk::Device& dev,
												const std::vector<vk::QueueFamilyProperties>& available,
												const std::vector<vk::QueueFamilyProperties>& requested )
{
	std::vector<std::vector<vk::Queue>> result;
	result.reserve(requested.size());

	//Query the queues
	for(const auto& queueFamily : requested){
		result.emplace_back(std::vector<vk::Queue>(queueFamily.queueCount));

		//Get the indices corresponding to this family
		//It might be distributed among more than one available family
		const auto indices = getQueueFamilyIndices(
			available, 
			std::vector<vk::QueueFamilyProperties>(1, queueFamily)
		);

		//Iterate though the returned families and assign them to the queue vector
		size_t j = 0; //Queue # counter
		for(const auto& index : indices){
			for(size_t i = 0; i < index.second; i++){
				result.back()[j] = dev.getQueue(index.first, i);
				j++;
			}
		}
				
		//Array should be full
		assert(j == result.back().size());
	}

	return result;
}

}
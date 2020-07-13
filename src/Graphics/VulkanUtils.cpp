#include "VulkanUtils.h"

#include <zuazo/Exception.h>

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

size_t getQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& qf, vk::QueueFlags flags){
	size_t i;

	for(i = 0; i < qf.size(); i++){
		if(getQueueFamilyCompatibility(flags, qf[i].queueFlags) && (qf[i].queueCount > 0)) {
			return i;
		}
	}

	if(flags == vk::QueueFlagBits::eTransfer){
		//Graphics queue should be also OK
		return getQueueFamilyIndex(qf, vk::QueueFlagBits::eGraphics);
	}

	//Failed to find a suitable queue family
	throw Exception("Requested queue family not found!");
}

bool getQueueFamilyCompatibility(vk::QueueFlags required, vk::QueueFlags available){
	return (required & available) == required;
}


bool hasSamplerSupport(vk::FormatProperties features){
	constexpr auto FLAGS = 	vk::FormatFeatureFlagBits::eTransferDst |
							vk::FormatFeatureFlagBits::eSampledImage |
							vk::FormatFeatureFlagBits::eSampledImageFilterLinear;

	return (features.optimalTilingFeatures & FLAGS) == FLAGS;
}

bool hasFramebufferSupport(vk::FormatProperties features){
	constexpr auto FLAGS = 	vk::FormatFeatureFlagBits::eTransferSrc |
							vk::FormatFeatureFlagBits::eColorAttachmentBlend;

	return (features.optimalTilingFeatures & FLAGS) == FLAGS;
}

}
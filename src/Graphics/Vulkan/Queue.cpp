#include <Graphics/Vulkan/Device.h>

#include <Exception.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace Zuazo::Graphics::Vulkan {

static bool isSuitable(uint32_t required, uint32_t available){
	return (required & available) == required;
}



std::vector<QueueFamily> getUsedQueueFamilies(const std::vector<QueueFamily>& available, std::vector<QueueFamily> required){
	std::vector<QueueFamily> queueFamilies;

	auto ite = required.begin();
	while(ite != required.end()){
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
		std::string msg = "There are missing queue families:\n";

		for(const auto& m : required) {
			char str[256];
			sprintf(str, "Flags: 0x%08x \tCount: %d\n", m.queueFlags, m.queueCount);
			msg += str;
		}

		throw Exception(msg);
	}

	return queueFamilies;
}

std::map<uint32_t, uint32_t> getQueueFamilyIndices(const std::vector<QueueFamily>& available, std::vector<QueueFamily> requested){
	std::map<uint32_t, uint32_t> result;

	for(const auto& req : requested){
		const size_t idx = getQueueFamilyIndex(available, req.queueFlags);

		if(idx < available.size()){
			result[idx] = std::max(result[idx], req.queueCount);
		} else {
			throw Exception("Requested family not found");
		}
	}

	return result;
}

size_t getQueueFamilyIndex(const std::vector<QueueFamily>& qf, uint32_t flags){
	size_t i;

	for(i = 0; i < qf.size(); i++){
		if(isSuitable(flags, qf[i].queueFlags) && (qf[i].queueCount > 0)) {
			break;
		}
	}

	return i;
}

}
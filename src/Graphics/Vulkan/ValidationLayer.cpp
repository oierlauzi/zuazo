#include <Graphics/Vulkan/ValidationLayer.h>

#include <Exception.h>

#include <string>
#include <cstring>

namespace Zuazo::Graphics::Vulkan {

std::vector<ValidationLayer> getUsedValidationLayers(const std::vector<ValidationLayer>& available, std::vector<ValidationLayer> required){
	std::vector<ValidationLayer> layers;

	//Check for availability of all required validation layers
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

	if(required.size()){
		//There are missing validation layers

		std::string missingNames;
		for(const auto& m : required){
			missingNames += m.layerName;
			missingNames += "\n";
		}

		throw Exception("Missing Vulkan validation layers:\n" + missingNames);
	}

	return layers;
}

std::vector<const char*> getNames(const std::vector<ValidationLayer>& ext){
	std::vector<const char*> names;
	const size_t nExtensions = ext.size();

	names.reserve(nExtensions);
	for(size_t i = 0; i < nExtensions; i++){
		names.push_back(ext[i].layerName);
	}

	return names;
}

}
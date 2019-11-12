#include <Graphics/Vulkan/Extension.h>

#include <Exception.h>

#include <string>
#include <cstring>

namespace Zuazo::Graphics::Vulkan {

std::vector<Extension> getUsedExtensions(const std::vector<Extension>& available, std::vector<Extension> required){
	std::vector<Extension> extensions;

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

	if(required.size()){
		//There are missing extensions

		std::string missingNames;
		for(const auto& m : required){
			missingNames += m.extensionName;
			missingNames += "\n";
		}

		throw Exception("Missing Vulkan extensions:\n" + missingNames);
	}

	return extensions;
}

std::vector<const char*> getNames(const std::vector<Extension>& ext){
	std::vector<const char*> names;
	const size_t nExtensions = ext.size();

	names.reserve(nExtensions);
	for(size_t i = 0; i < nExtensions; i++){
		names.push_back(ext[i].extensionName);
	}

	return names;
}

}
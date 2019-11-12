#include <Graphics/Vulkan/Messenger.h>

#include <Exception.h>

namespace Zuazo::Graphics::Vulkan {

Messenger::Messenger(Instance& instance, Callback cbk) :
	m_instance(instance)
{
	//Request the functions to Vulkan
	auto vkCreateDebugUtilsMessengerEXT = 
		m_instance.getFunction<PFN_vkCreateDebugUtilsMessengerEXT>("vkCreateDebugUtilsMessengerEXT");

	//Setup validation layer callback
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = 
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | 
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	createInfo.messageType = 
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = cbk;
	createInfo.pUserData = this;

	//Create the messenger
	VkResult err = vkCreateDebugUtilsMessengerEXT(m_instance.get(), &createInfo, nullptr, &m_messenger);

	switch(err){
		case VK_SUCCESS:
			break; //Everything OK.

		default:
			throw Exception("Error creating Vulkan validation layer messenger");
	}

}

Messenger::~Messenger() {
	if(m_messenger){
		//Request the functions to Vulkan
		auto vkDestroyDebugUtilsMessengerEXT = 
			m_instance.getFunction<PFN_vkDestroyDebugUtilsMessengerEXT>("vkDestroyDebugUtilsMessengerEXT");

		vkDestroyDebugUtilsMessengerEXT(m_instance.get(), m_messenger, nullptr);
	}
}




VkDebugUtilsMessengerEXT Messenger::get() {
	return m_messenger;
}


}
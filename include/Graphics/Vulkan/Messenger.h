#pragma once 

#include "Instance.h"

namespace Zuazo::Graphics::Vulkan {

class Messenger{
public:
	using Callback = PFN_vkDebugUtilsMessengerCallbackEXT;

	Messenger(Instance& instance, Callback cbk = defaultCallback);
	Messenger(const Messenger& other) = delete;
	~Messenger();

	VkDebugUtilsMessengerEXT			get();
private:
	Instance&							m_instance;

	VkDebugUtilsMessengerEXT 			m_messenger = nullptr;

	static const Callback				defaultCallback;
};

}
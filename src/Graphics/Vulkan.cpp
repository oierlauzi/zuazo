#include <Graphics/Vulkan.h>

#include "VulkanUtils.h"

#include <Graphics/GLFW.h>
#include <Graphics/VulkanConversions.h>
#include <Graphics/ColorTransfer.h>
#include <Zuazo.h>
#include <Exception.h>
#include <Macros.h>

#include <cstring>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <sstream>
#include <set>
#include <unordered_map>
#include <optional>

#if (ZUAZO_IS_DEBUG == true)
	#define ZUAZO_ENABLE_VALIDATION_LAYERS
#endif

#if 0
	#define ZUAZO_DISABLE_PRESENTATION_SUPPORT
#endif

namespace Zuazo::Graphics {

struct Vulkan::Impl {
	/*
	 * Vulkan instantiation related stuff
	 */

	enum QueueIndices {
		GRAPHICS_QUEUE,
		COMPUTE_QUEUE,
		TRANSFER_QUEUE,
		PRESENTATION_QUEUE,
		QUEUE_NUM
	};

	using Samplers = std::array<vk::UniqueSampler, VK_FILTER_RANGE_SIZE>;
	using ColorTransferDescriporSetLayouts = std::array<vk::UniqueDescriptorSetLayout, VK_FILTER_RANGE_SIZE>;

	vk::DynamicLoader								loader;
	vk::DispatchLoaderDynamic						dispatcher;
	vk::UniqueInstance								instance;
	vk::UniqueDebugUtilsMessengerEXT				messenger;
	vk::PhysicalDevice								physicalDevice;
	std::array<uint32_t, QUEUE_NUM>					queueIndices;
	vk::UniqueDevice								device;
	std::array<vk::Queue, QUEUE_NUM>				queues;
	vk::UniquePipelineCache							pipelineCache;
	Samplers										samplers;
	FormatSupport									formatSupport;

	/*
	 * Cached Vulkan objects
	 */

	template<typename T>
	using HashMap = std::unordered_map<size_t, T>;

	mutable HashMap<vk::UniqueShaderModule>			shaders;
	mutable HashMap<vk::UniquePipelineLayout>		pipelineLayouts;
	mutable HashMap<vk::UniqueDescriptorSetLayout>	descriptorSetLayouts;

	mutable std::vector<vk::SwapchainKHR>			presentSwapchains;
	mutable std::vector<uint32_t>					presentIndices;
	mutable std::vector<vk::Semaphore>				presentSemaphores;

	/*
	 * Methods
	 */
	Impl(	std::string_view appName, 
			Version appVersion,
			const DeviceScoreFunc& scoreFunc )
		: loader()
		, dispatcher(createDispatcher(loader))
		, instance(createInstance(dispatcher, appName.data(), toVulkan(appVersion)))
		, messenger(createMessenger(dispatcher, *instance))
		, physicalDevice(getBestPhysicalDevice(dispatcher, *instance, scoreFunc))
		, queueIndices(getQueueIndices(dispatcher, *instance, physicalDevice))
		, device(createDevice(dispatcher, physicalDevice, queueIndices))
		, queues(getQueues(dispatcher, *device, queueIndices))
		, samplers(createSamplers(dispatcher, *device))
		, formatSupport(getFormatSupport(dispatcher, physicalDevice))
	{
	}

	~Impl() = default;

	const vk::DynamicLoader& getLoader() const{
		return loader;
	}

	const vk::DispatchLoaderDynamic& getDispatcher() const{
		return dispatcher;
	}

	vk::Instance getInstance() const{
		return *instance;
	}

	vk::PhysicalDevice getPhysicalDevice() const{
		return physicalDevice;
	}

	vk::Device getDevice() const{
		return *device;
	}

	uint32_t getGraphicsQueueIndex() const{
		return queueIndices[GRAPHICS_QUEUE];
	}

	vk::Queue getGraphicsQueue() const{
		return queues[GRAPHICS_QUEUE];
	}

	uint32_t getComputeQueueIndex() const{
		return queueIndices[COMPUTE_QUEUE];
	}

	vk::Queue getComputeQueue() const{
		return queues[COMPUTE_QUEUE];
	}

	uint32_t getTransferQueueIndex() const{
		return queueIndices[TRANSFER_QUEUE];
	}

	vk::Queue getTransferQueue() const{
		return queues[TRANSFER_QUEUE];
	}

	uint32_t getPresentationQueueIndex() const{
		return queueIndices[PRESENTATION_QUEUE];
	}

	vk::Queue getPresentationQueue() const{
		return queues[PRESENTATION_QUEUE];
	}

	vk::PipelineCache getPipelineCache() const {
		return *pipelineCache;
	}

	vk::Sampler getSampler(vk::Filter filter) const {
		return *(samplers[static_cast<size_t>(filter)]);
	}

	const FormatSupport& getFormatSupport() const{
		return formatSupport;
	}

	vk::FormatProperties getFormatFeatures(vk::Format format) const {
		return physicalDevice.getFormatProperties(format, dispatcher);
	}



	vk::UniqueSwapchainKHR createSwapchain(const vk::SwapchainCreateInfoKHR& createInfo) const{
		return device->createSwapchainKHRUnique(createInfo, nullptr, dispatcher);
	}

	vk::UniqueImage createImage(const vk::ImageCreateInfo& createInfo) const{
		return device->createImageUnique(createInfo, nullptr, dispatcher);
	}

	vk::UniqueImageView createImageView(const vk::ImageViewCreateInfo& createInfo) const{
		return device->createImageViewUnique(createInfo, nullptr, dispatcher);
	}

	vk::UniqueRenderPass createRenderPass(const vk::RenderPassCreateInfo& createInfo) const{
		return device->createRenderPassUnique(createInfo, nullptr, dispatcher);
	}

	vk::UniquePipeline createGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& createInfo ) const 
	{
		return device->createGraphicsPipelineUnique(*pipelineCache, createInfo, nullptr, dispatcher);
	}

	vk::UniqueFramebuffer createFramebuffer(const vk::FramebufferCreateInfo& createInfo) const{
		return device->createFramebufferUnique(createInfo, nullptr, dispatcher);
	}

	vk::UniqueCommandPool createCommandPool(const vk::CommandPoolCreateInfo& createInfo) const{
		return device->createCommandPoolUnique(createInfo, nullptr, dispatcher);
	}

	vk::UniqueBuffer createBuffer(const vk::BufferCreateInfo& createInfo) const{
		return device->createBufferUnique(createInfo, nullptr, dispatcher);
	}

	vk::UniqueDescriptorPool createDescriptorPool(const vk::DescriptorPoolCreateInfo& createInfo) const{
		return device->createDescriptorPoolUnique(createInfo, nullptr, dispatcher);
	}

	vk::UniqueSemaphore createSemaphore() const {
		const vk::SemaphoreCreateInfo createInfo;
		return device->createSemaphoreUnique(createInfo, nullptr, dispatcher);
	}

	vk::UniqueFence createFence(bool signaled) const {
		const vk::FenceCreateInfo createInfo(
			signaled ? vk::FenceCreateFlags(vk::FenceCreateFlagBits::eSignaled) : vk::FenceCreateFlags()
		);
		return device->createFenceUnique(createInfo, nullptr, dispatcher);
	}



	vk::UniqueShaderModule createShaderModule(Utils::BufferView<const uint32_t> code) const
	{
		const vk::ShaderModuleCreateInfo createInfo(
			{},												//Flags
			code.size() * sizeof(uint32_t), code.data()		//Code
		);

		return device->createShaderModuleUnique(createInfo, nullptr, dispatcher);
	}

	vk::ShaderModule createShaderModule(size_t id) const {
		auto ite = shaders.find(id);
		if(ite != shaders.cend()) return *(ite->second);
		else return {};	
	}

	vk::ShaderModule createShaderModule(size_t id, 
										Utils::BufferView<const uint32_t> code ) const
	{
		auto [ite, result] = shaders.emplace(
			id,
			createShaderModule(code)
		);

		assert(result);
		assert(ite != shaders.cend());
		return *(ite->second);
	}

	vk::UniquePipelineLayout createPipelineLayout(const vk::PipelineLayoutCreateInfo& createInfo) const {
		return device->createPipelineLayoutUnique(createInfo, nullptr, dispatcher);
	}

	vk::PipelineLayout createPipelineLayout(size_t id) const {
		auto ite = pipelineLayouts.find(id);
		if(ite != pipelineLayouts.cend()) return *(ite->second);
		else return {};	
	}

	vk::PipelineLayout createPipelineLayout(size_t id, 
											const vk::PipelineLayoutCreateInfo& createInfo ) const
	{
		auto [ite, result] = pipelineLayouts.emplace(
			id,
			createPipelineLayout(createInfo)
		);

		assert(result);
		assert(ite != pipelineLayouts.cend());
		return *(ite->second);
	}

	vk::UniqueDescriptorSetLayout createDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& createInfo) const {
		return device->createDescriptorSetLayoutUnique(createInfo, nullptr, dispatcher);
	}

	vk::DescriptorSetLayout createDescriptorSetLayout(size_t id) const {
		auto ite = descriptorSetLayouts.find(id);
		if(ite != descriptorSetLayouts.cend()) return *(ite->second);
		else return {};	
	}

	vk::DescriptorSetLayout createDescriptorSetLayout(	size_t id, 
														const vk::DescriptorSetLayoutCreateInfo& createInfo ) const
	{
		auto [ite, result] = descriptorSetLayouts.emplace(
			id,
			createDescriptorSetLayout(createInfo)
		);

		assert(result);
		assert(ite != descriptorSetLayouts.cend());
		return *(ite->second);
	}



	std::vector<vk::UniqueCommandBuffer> allocateCommnadBuffers(const vk::CommandBufferAllocateInfo& allocInfo) const{
		return device->allocateCommandBuffersUnique(allocInfo, dispatcher);
	}

	vk::UniqueDeviceMemory allocateMemory(const vk::MemoryAllocateInfo& allocInfo) const{
		return device->allocateMemoryUnique(allocInfo, nullptr, dispatcher);
	}

	vk::UniqueDeviceMemory allocateMemory(	const vk::MemoryRequirements& requirements,
											vk::MemoryPropertyFlags properties ) const
	{
		const auto memoryProperties = physicalDevice.getMemoryProperties(dispatcher);

		//Find an apropiate index for the type
		uint32_t i;
		for (i = 0; i < memoryProperties.memoryTypeCount; i++) {
			const uint32_t indexFlag = 1 << i;

			if(	(requirements.memoryTypeBits & indexFlag) && 
				(memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) 
			{
				break; //Found one!
			}
		}
		if(i == memoryProperties.memoryTypeCount){
			throw Exception("Error allocating device memory");
		}

		const vk::MemoryAllocateInfo allocInfo(
			requirements.size,						//Size
			i										//Memory type index
		);

		return allocateMemory(allocInfo);
	}



	std::byte* mapMemory(const vk::MappedMemoryRange& range) const{
		return static_cast<std::byte*>(device->mapMemory(
			range.memory,												//Memory allocation
			range.offset,												//Offset
			range.size,													//Size
			{},															//Flags
			dispatcher												//Dispatcher
		));
	}

	void flushMappedMemory(const vk::MappedMemoryRange& range) const {
		device->flushMappedMemoryRanges(
			range,
			dispatcher
		);
	}



	void present(	vk::SwapchainKHR swapchain,
					uint32_t imageIndex,
					vk::Semaphore waitSemaphore ) const 
	{
		presentSwapchains.emplace_back(swapchain);
		presentIndices.emplace_back(imageIndex);

		if(waitSemaphore) {
			presentSemaphores.emplace_back(waitSemaphore);
		}
	}

	void presentAll() const {
		assert(presentIndices.size() == presentSwapchains.size());

		if(presentSwapchains.size() > 0) {
			const vk::PresentInfoKHR presentInfo(
				presentSemaphores.size(), presentSemaphores.data(),					//Wait semaphores
				presentSwapchains.size(), presentSwapchains.data(), 				//Swapchains
				presentIndices.data(),												//Image index
				nullptr																//Results								
			);

			switch(getPresentationQueue().presentKHR(&presentInfo, dispatcher)){
			case vk::Result::eSuccess: 
			case vk::Result::eErrorOutOfDateKHR: 
				break;
			default:
				throw Exception("Error presentting images");
			}

			presentSemaphores.clear();
			presentSwapchains.clear();
			presentIndices.clear();

			assert(presentSemaphores.size() == 0);
			assert(presentSwapchains.size() == 0);
			assert(presentIndices.size() == 0);
		}
	}



private:
	/*
	 * Instantiation methods
	 */

	static vk::DispatchLoaderDynamic createDispatcher(const vk::DynamicLoader& loader) {
		vk::DispatchLoaderDynamic dispatch;

		auto vkGetInstanceProcAddr = loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		dispatch.init(vkGetInstanceProcAddr);

		return dispatch;
	}

	static vk::UniqueInstance createInstance(	vk::DispatchLoaderDynamic& disp, 
												const char* appName, 
												uint32_t appVersion )
	{
		//Set the application information
		vk::ApplicationInfo appInfo(
			appName,
			appVersion,
			"Zuazo",
			toVulkan(runtimeVersion),
			VK_API_VERSION_1_0
		);

		//Get the validation layers
		auto requiredLayers = getRequiredLayers();
		const auto availableLayers = vk::enumerateInstanceLayerProperties(disp);
		const auto usedLayers = getUsedLayers(availableLayers, requiredLayers);

		if(requiredLayers.size()){
			//There are missing layers
			std::string missingNames;

			for(const auto& m : requiredLayers){
				missingNames += m.layerName;
				missingNames += "\n";
			}

			throw Exception("Missing Vulkan validation layers:\n" + missingNames);
		}

		const auto usedLayerNames = getNames(usedLayers);


		//Get the extensions
		auto requiredExtensions = getRequiredInstanceExtensions();
		const auto availableExtensions = vk::enumerateInstanceExtensionProperties(nullptr, disp);
		const auto usedExtensions = getUsedExtensions(availableExtensions, requiredExtensions);

		if(requiredExtensions.size()){
			//There are missing extensions
			std::string missingNames;

			for(const auto& m : requiredExtensions){
				missingNames += m.extensionName;
				missingNames += "\n";
			}

			throw Exception("Missing Vulkan extensions:\n" + missingNames);
		}


		const auto usedExtensionNames = getNames(usedExtensions);

		//Create the vulkan instance
		const vk::InstanceCreateInfo createInfo(
			{},														//Flags
			&appInfo,												//Application info
			usedLayerNames.size(), usedLayerNames.data(),			//Validation layers
			usedExtensionNames.size(), usedExtensionNames.data()	//Extensions
		);
		auto instance = vk::createInstanceUnique(createInfo, nullptr, disp);

		//Add the available functions to the dispatcher
		disp.init(*instance);

		return instance;
	}

	static vk::UniqueDebugUtilsMessengerEXT	createMessenger(const vk::DispatchLoaderDynamic& disp, 
															vk::Instance instance )
	{
		#ifdef ZUAZO_ENABLE_VALIDATION_LAYERS //TODO move to impl
			constexpr auto msgSeverity =
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;

			constexpr auto msgTypes = 	
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
				vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

			const vk::DebugUtilsMessengerCreateInfoEXT createInfo(
				{},										//Flags
				msgSeverity,							//Message severity flags
				msgTypes,								//Message type flags
				validationLayerCallback,				//Callback ptr
				nullptr									//User ptr
			);

			return instance.createDebugUtilsMessengerEXTUnique(createInfo, nullptr, disp);
		#else
			return {};
		#endif
	}

	static vk::PhysicalDevice getBestPhysicalDevice(const vk::DispatchLoaderDynamic& disp, 
													vk::Instance instance, 
													const DeviceScoreFunc& scoreFunc )
	{
		const auto devices = instance.enumeratePhysicalDevices(disp);

		std::pair<std::optional<vk::PhysicalDevice>, uint32_t> best({}, 0);

		for(const auto& device : devices){
			//Check validation layer support
			auto requiredLayers = getRequiredLayers();
			const auto supportedLayers = device.enumerateDeviceLayerProperties(disp);
			getUsedLayers(supportedLayers, requiredLayers);
			if(requiredLayers.size()){
				continue;
			}

			//Check device extension support
			auto requiredExtensions = getRequiredDeviceExtensions();
			const auto availableExtensions = device.enumerateDeviceExtensionProperties(nullptr, disp);
			getUsedExtensions(availableExtensions, requiredExtensions);
			if(requiredExtensions.size()){
				continue;
			}

			//Check device queue family support
			auto requiredQueueFamilies = getRequiredQueueFamilies();
			const auto availableQueueFamilies = device.getQueueFamilyProperties(disp);
			getUsedQueueFamilies(availableQueueFamilies, requiredQueueFamilies);
			if(requiredQueueFamilies.size()){
				continue;
			}

			#ifndef ZUAZO_DISABLE_PRESENTATION_SUPPORT
				//Window support is required
				const size_t nQueueFamilies = availableQueueFamilies.size();
				if(GLFW::getPresentationQueueFamilies(instance, device, nQueueFamilies).size() == 0){
					continue;
				}
			#endif

			//Evaluate if the device is the best
			//If the score is 0 device is considered not suitable
			//as best score starts at 0
			const uint32_t score = scoreFunc(disp, device);
			if(score > best.second){
				best = {device, score};
			}
		}

		if(best.first.has_value() == false){
			throw Exception("No compatible GPUs were found");
		}

		return best.first.value();
	}

	static std::array<uint32_t, QUEUE_NUM> getQueueIndices(	const vk::DispatchLoaderDynamic& disp, 
															vk::Instance inst, 
															vk::PhysicalDevice dev )
	{
		std::array<uint32_t, QUEUE_NUM>	queues;

		const auto queueFamilies = dev.getQueueFamilyProperties(disp);

		//Add the queue families
		queues[GRAPHICS_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eGraphics);
		queues[COMPUTE_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eCompute);
		queues[TRANSFER_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eTransfer);


		#ifndef ZUAZO_DISABLE_PRESENTATION_SUPPORT
			//Add a queue family compatible with presentation
			const auto presentFamilies = GLFW::getPresentationQueueFamilies(inst, dev, queueFamilies.size());
			assert(presentFamilies.size()); //It should have been checked before

			//Try to assign the graphics queue
			if(std::find(presentFamilies.cbegin(), presentFamilies.cend(), queues[GRAPHICS_QUEUE]) != presentFamilies.cend()){
				queues[PRESENTATION_QUEUE] = queues[GRAPHICS_QUEUE];
			} else {
				queues[PRESENTATION_QUEUE] = presentFamilies[0];
			}
		#endif

		return queues;
	}

	static vk::UniqueDevice	createDevice(	vk::DispatchLoaderDynamic& disp, 
											vk::PhysicalDevice physicalDevice, 
											const std::array<uint32_t, QUEUE_NUM>& queueIndices )
	{
		//Get the validation layers and extensions.
		//They should be available, as we have checked for them when
		//choosing the physical device
		const auto layers = getRequiredLayers();
		const auto layerNames = getNames(layers);
		const auto extensions = getRequiredDeviceExtensions();
		const auto extensionNames = getNames(extensions);

		//Fill the queue create infos
		std::set<uint32_t> uniqueQueueFamilies(queueIndices.cbegin(), queueIndices.cend());
		const float queuePriority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		queueCreateInfos.reserve(uniqueQueueFamilies.size());

		for(auto q : uniqueQueueFamilies){
			queueCreateInfos.emplace_back(
				vk::DeviceQueueCreateFlags(),
				q,
				1, &queuePriority
			);
		}

		//Create it
		const vk::DeviceCreateInfo createInfo(
			{},
			queueCreateInfos.size(), queueCreateInfos.data(),			//Queue families
			layerNames.size(), layerNames.data(),						//Validation layers
			extensionNames.size(), extensionNames.data()				//Extensions
		);

		auto dev = physicalDevice.createDeviceUnique(createInfo, nullptr, disp);

		//Register the device specific functions 
		disp.init(*dev);

		return dev;
	}

	static std::array<vk::Queue, QUEUE_NUM>	getQueues(	const vk::DispatchLoaderDynamic& disp, 
														vk::Device device, 
														const std::array<uint32_t, QUEUE_NUM>& queueIndices ) 
	{
		std::array<vk::Queue, QUEUE_NUM> queues;

		for(size_t i = 0; i < QUEUE_NUM; i++){
			queues[i] = device.getQueue(queueIndices[i], 0, disp);
		}

		return queues;
	}

	static vk::UniquePipelineCache createPipelineCache(	const vk::DispatchLoaderDynamic& disp, 
														vk::Device device )
	{
		const vk::PipelineCacheCreateInfo createInfo(
			{},															//Flags
			0, nullptr													//Data
		);

		return device.createPipelineCacheUnique(createInfo, nullptr, disp);
	}

	static Samplers	createSamplers(	const vk::DispatchLoaderDynamic& disp, 
									vk::Device device )
	{
		Samplers result;

		for(size_t i = 0; i < result.size(); i++){
			const auto filter = static_cast<vk::Filter>(i);

			const vk::SamplerCreateInfo createInfo(
				{},														//Flags
				filter, filter,											//Min/Mag filter
				vk::SamplerMipmapMode::eNearest,						//Mipmap mode
				vk::SamplerAddressMode::eClampToEdge,					//U address mode
				vk::SamplerAddressMode::eClampToEdge,					//V address mode
				vk::SamplerAddressMode::eClampToEdge,					//W address mode
				0.0f,													//Mip LOD bias
				false,													//Enable anisotropy
				0.0f,													//Max anisotropy
				false,													//Compare enable
				vk::CompareOp::eNever,									//Compare operation
				0.0f, 0.0f,												//Min/Max LOD
				vk::BorderColor::eFloatTransparentBlack,				//Boreder color
				false													//Unormalized coords
			);

			result[i] = device.createSamplerUnique(createInfo, nullptr, disp);
		}

		return result;
	}

	static FormatSupport getFormatSupport(	const vk::DispatchLoaderDynamic& disp, 
											vk::PhysicalDevice physicalDevice )
	{
		constexpr std::array FORMAT_RANGES = {
			std::pair{VK_FORMAT_BEGIN_RANGE, VK_FORMAT_END_RANGE }, //Normal formats
			std::pair{VK_FORMAT_G8B8G8R8_422_UNORM, VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM } //YCbCr formats
		};

		FormatSupport result;

		for(const auto& range : FORMAT_RANGES){
			for(size_t i = range.first; i <= range.second; i++){
				const auto format = static_cast<vk::Format>(i);

				const auto properties = physicalDevice.getFormatProperties(format, disp);

				if(hasSamplerSupport(properties)){
					result.sampler.emplace_back(format);
				}

				if(hasFramebufferSupport(properties)){
					result.framebuffer.emplace_back(format);
				}
			}
		}

		return result;
	}
																
	static std::vector<vk::LayerProperties> getRequiredLayers() {
		std::vector<vk::LayerProperties> validationLayers;

		#ifdef ZUAZO_ENABLE_VALIDATION_LAYERS
			//Add debug utils extension requirement
			//Khronos validation layer
			validationLayers.emplace_back();
			std::strncpy(validationLayers.back().layerName, "VK_LAYER_KHRONOS_validation", VK_MAX_EXTENSION_NAME_SIZE);
		#endif

		return validationLayers;
	}

	static std::vector<vk::ExtensionProperties> getRequiredInstanceExtensions() {
		std::vector<vk::ExtensionProperties> extensions;

		#ifndef ZUAZO_DISABLE_PRESENTATION_SUPPORT
			//Add window swap-chain extensions
			const auto windowExtensions = GLFW::getRequiredVulkanExtensions();
			std::copy(
				windowExtensions.cbegin(),
				windowExtensions.cend(),
				std::back_insert_iterator(extensions)
			);
		#endif

		#ifdef ZUAZO_ENABLE_VALIDATION_LAYERS
			//Add debug utils extension requirement
			extensions.emplace_back();
			std::strncpy(extensions.back().extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
		#endif

		return extensions;
	}

	static std::vector<vk::ExtensionProperties> getRequiredDeviceExtensions() {
		std::vector<vk::ExtensionProperties> extensions;

		#ifndef ZUAZO_DISABLE_PRESENTATION_SUPPORT
			//Require swapchain extension
			VkExtensionProperties swapchainExtension = {};
			std::strncpy(swapchainExtension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
			extensions.emplace_back(swapchainExtension);
		#endif

		return extensions;
	}

	static std::vector<vk::QueueFamilyProperties> getRequiredQueueFamilies() {
		return {
			VkQueueFamilyProperties{ VK_QUEUE_GRAPHICS_BIT,	1,	0,	{} },
			VkQueueFamilyProperties{ VK_QUEUE_COMPUTE_BIT,	1,	0,	{} },
			VkQueueFamilyProperties{ VK_QUEUE_TRANSFER_BIT,	1,	0,	{} }
		};
	}

	/*
	 * Validation layer callback
	 */

	static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerCallback(	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
																	VkDebugUtilsMessageTypeFlagsEXT type,
																	const VkDebugUtilsMessengerCallbackDataEXT* data,
																	void* userData ) 
	{
		/*
		* Based on:
		* https://github.com/KhronosGroup/Vulkan-Hpp/blob/master/samples/CreateDebugUtilsMessenger/CreateDebugUtilsMessenger.cpp
		*/

		std::ostringstream message;

		message 
			<< vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)) 
			<< ": " << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(type)) << ":\n";
		message << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(type)) << ":\n";
		message << "\t" << "messageIDName   = <" << data->pMessageIdName << ">\n";
		message << "\t" << "messageIdNumber = " << data->messageIdNumber << "\n";
		message << "\t" << "message         = <" << data->pMessage << ">\n";

		if (0 < data->queueLabelCount) {
			message << "\t" << "Queue Labels:\n";
			for (uint8_t i = 0; i < data->queueLabelCount; i++) {
				message << "\t\t" << "lableName = <" << data->pQueueLabels[i].pLabelName << ">\n";
			}
		}

		if (0 < data->cmdBufLabelCount) {
			message << "\t" << "CommandBuffer Labels:\n";
			for (uint8_t i = 0; i < data->cmdBufLabelCount; i++) {
				message << "\t\t" << "labelName = <" << data->pCmdBufLabels[i].pLabelName << ">\n";
			}
		}

		if (0 < data->objectCount) {
			message << "\t" << "Objects:\n";
			for (uint8_t i = 0; i < data->objectCount; i++) {
				message << "\t\t" << "Object " << i << "\n";
				message 
					<< "\t\t\t" << "objectType   = " 
					<< vk::to_string(static_cast<vk::ObjectType>(data->pObjects[i].objectType)) << "\n";
				message << "\t\t\t" << "objectHandle = " << data->pObjects[i].objectHandle << "\n";
				if (data->pObjects[i].pObjectName) {
					message << "\t\t\t" << "objectName   = <" << data->pObjects[i].pObjectName << ">\n";
				}
			}
		}

		std::cerr << message.str() << std::endl;

		ZUAZO_IGNORE_PARAM(userData);

		return VK_FALSE;
	}

};



/*
 * Interface implementation
 */

Vulkan::Vulkan(	std::string_view appName, 
				Version appVersion,
				const DeviceScoreFunc& scoreFunc )
	: m_impl(appName, appVersion, scoreFunc)
{
}

Vulkan::Vulkan(Vulkan&& other) = default;

Vulkan::~Vulkan() = default;

Vulkan& Vulkan::operator=(Vulkan&& other) = default; 



const vk::DynamicLoader& Vulkan::getLoader() const{
	return m_impl->getLoader();
}

const vk::DispatchLoaderDynamic& Vulkan::getDispatcher() const{
	return m_impl->getDispatcher();
}

vk::Instance Vulkan::getInstance() const{
	return m_impl->getInstance();
}

vk::PhysicalDevice Vulkan::getPhysicalDevice() const{
	return m_impl->getPhysicalDevice();
}

vk::Device Vulkan::getDevice() const{
	return m_impl->getDevice();
}

uint32_t Vulkan::getGraphicsQueueIndex() const{
	return m_impl->getGraphicsQueueIndex();
}

vk::Queue Vulkan::getGraphicsQueue() const{
	return m_impl->getGraphicsQueue();
}

uint32_t Vulkan::getComputeQueueIndex() const{
	return m_impl->getComputeQueueIndex();
}

vk::Queue Vulkan::getComputeQueue() const{
	return m_impl->getComputeQueue();
}

uint32_t Vulkan::getTransferQueueIndex() const{
	return m_impl->getTransferQueueIndex();
}

vk::Queue Vulkan::getTransferQueue() const{
	return m_impl->getTransferQueue();
}

uint32_t Vulkan::getPresentationQueueIndex() const{
	return m_impl->getPresentationQueueIndex();
}

vk::Queue Vulkan::getPresentationQueue() const{
	return m_impl->getPresentationQueue();
}

vk::PipelineCache Vulkan::getPipelineCache() const {
	return m_impl->getPipelineCache();
}

vk::Sampler Vulkan::getSampler(vk::Filter filter) const {
	return m_impl->getSampler(filter);
}

const Vulkan::FormatSupport& Vulkan::getFormatSupport() const {
	return m_impl->getFormatSupport();
}

vk::FormatProperties Vulkan::getFormatFeatures(vk::Format format) const {
	return m_impl->getFormatFeatures(format);
}

vk::UniqueSwapchainKHR Vulkan::createSwapchain(const vk::SwapchainCreateInfoKHR& createInfo) const{
	return m_impl->createSwapchain(createInfo);
}

vk::UniqueImage Vulkan::createImage(const vk::ImageCreateInfo& createInfo) const{
	return m_impl->createImage(createInfo);
}

vk::UniqueImageView Vulkan::createImageView(const vk::ImageViewCreateInfo& createInfo) const{
	return m_impl->createImageView(createInfo);
}

vk::UniqueRenderPass Vulkan::createRenderPass(const vk::RenderPassCreateInfo& createInfo) const{
	return m_impl->createRenderPass(createInfo);
}

vk::UniquePipeline Vulkan::createGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& createInfo ) const 
{
	return m_impl->createGraphicsPipeline(createInfo);
}

vk::UniqueFramebuffer Vulkan::createFramebuffer(const vk::FramebufferCreateInfo& createInfo) const{
	return m_impl->createFramebuffer(createInfo);
}

vk::UniqueCommandPool Vulkan::createCommandPool(const vk::CommandPoolCreateInfo& createInfo) const{
	return m_impl->createCommandPool(createInfo);
}

vk::UniqueBuffer Vulkan::createBuffer(const vk::BufferCreateInfo& createInfo) const{
	return m_impl->createBuffer(createInfo);
}

vk::UniqueDescriptorPool Vulkan::createDescriptorPool(const vk::DescriptorPoolCreateInfo& createInfo) const{
	return m_impl->createDescriptorPool(createInfo);
}

vk::UniqueSemaphore Vulkan::createSemaphore() const {
	return m_impl->createSemaphore();
}

vk::UniqueFence Vulkan::createFence(bool signaled) const {
	return m_impl->createFence(signaled);
}



vk::UniqueShaderModule Vulkan::createShaderModule(Utils::BufferView<const uint32_t> code) const  {
	return m_impl->createShaderModule(code);
}

vk::ShaderModule Vulkan::createShaderModule(size_t id) const  {
	return m_impl->createShaderModule(id);
}

vk::ShaderModule Vulkan::createShaderModule(size_t id, 
											Utils::BufferView<const uint32_t> code ) const 
{
	return m_impl->createShaderModule(id, code);
}

vk::UniquePipelineLayout Vulkan::createPipelineLayout(const vk::PipelineLayoutCreateInfo& createInfo) const {
	return m_impl->createPipelineLayout(createInfo);
}

vk::PipelineLayout Vulkan::createPipelineLayout(size_t id) const {
	return m_impl->createPipelineLayout(id);
}

vk::PipelineLayout Vulkan::createPipelineLayout(size_t id,
												const vk::PipelineLayoutCreateInfo& createInfo ) const
{
	return m_impl->createPipelineLayout(id, createInfo);
}

vk::UniqueDescriptorSetLayout Vulkan::createDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& createInfo) const {
	return m_impl->createDescriptorSetLayout(createInfo);
}

vk::DescriptorSetLayout Vulkan::createDescriptorSetLayout(size_t id) const {
	return m_impl->createDescriptorSetLayout(id);
}

vk::DescriptorSetLayout Vulkan::createDescriptorSetLayout(	size_t id,
															const vk::DescriptorSetLayoutCreateInfo& createInfo ) const
{
	return m_impl->createDescriptorSetLayout(id, createInfo);
}



std::vector<vk::UniqueCommandBuffer> Vulkan::allocateCommnadBuffers(const vk::CommandBufferAllocateInfo& allocInfo) const{
	return m_impl->allocateCommnadBuffers(allocInfo);
}

vk::UniqueDeviceMemory Vulkan::allocateMemory(const vk::MemoryAllocateInfo& allocInfo) const{
	return m_impl->allocateMemory(allocInfo);
}

vk::UniqueDeviceMemory Vulkan::allocateMemory(	const vk::MemoryRequirements& requirements,
												vk::MemoryPropertyFlags properties ) const
{
	return m_impl->allocateMemory(requirements, properties);
}



std::byte* Vulkan::mapMemory(const vk::MappedMemoryRange& range) const{
	return m_impl->mapMemory(range);
}

void Vulkan::flushMappedMemory(const vk::MappedMemoryRange& range) const {
	return m_impl->flushMappedMemory(range);
}



void Vulkan::present(	vk::SwapchainKHR swapchain,
						uint32_t imageIndex,
						vk::Semaphore waitSemaphore ) const
{
	m_impl->present(swapchain, imageIndex, waitSemaphore);
}

void Vulkan::presentAll() const {
	m_impl->presentAll();
}

}
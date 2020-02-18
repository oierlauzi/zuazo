#include <Graphics/Vulkan.h>

#include <Graphics/VulkanConversions.h>

#include <Zuazo.h>
#include <Exception.h>
#include <Macros.h>
#include <Graphics/VulkanUtils.h>
#include <Graphics/GLFW.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <cassert>
#include <set>

#if (ZUAZO_IS_DEBUG == true)
	#define ZUAZO_ENABLE_VALIDATION_LAYERS
#endif

#if 0
	#define ZUAZO_DISABLE_PRESENTATION_SUPPORT
#endif

namespace Zuazo::Graphics {

VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerCallback(	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
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

Vulkan::Vulkan(	const std::string_view& appName, 
				Version appVersion,
				const DeviceScoreFunc& scoreFunc )

	: m_loader()
	, m_dispatcher(createDispatcher(m_loader))
	, m_instance(createInstance(m_dispatcher, appName.data(), toVulkan(appVersion)))
	, m_messenger(createMessenger(m_dispatcher, *m_instance))
	, m_physicalDevice(getBestPhysicalDevice(m_dispatcher, *m_instance, scoreFunc))
	, m_queueIndices(getQueueIndices(m_dispatcher, *m_instance, m_physicalDevice))
	, m_device(createDevice(m_dispatcher, m_physicalDevice, m_queueIndices))
	, m_queues(getQueues(m_dispatcher, *m_device, m_queueIndices))
	, m_formatSupport(getFormatSupport(m_dispatcher, m_physicalDevice))
	, m_rgbSamplers(createRgbSamplers(m_dispatcher, *m_device))
	, m_ycbcrSamplers(createYcbcrSamplers(m_dispatcher, m_physicalDevice, *m_device, m_formatSupport.ycbcrSampler))
{
}

const vk::DynamicLoader& Vulkan::getLoader() const{
	return m_loader;
}

const vk::DispatchLoaderDynamic& Vulkan::getDispatcher() const{
	return m_dispatcher;
}

vk::Instance Vulkan::getInstance() const{
	return *m_instance;
}

vk::PhysicalDevice Vulkan::getPhysicalDevice() const{
	return m_physicalDevice;
}

vk::Device Vulkan::getDevice() const{
	return *m_device;
}

uint32_t Vulkan::getGraphicsQueueIndex() const{
	return m_queueIndices[GRAPHICS_QUEUE];
}

vk::Queue Vulkan::getGraphicsQueue() const{
	return m_queues[GRAPHICS_QUEUE];
}

uint32_t Vulkan::getComputeQueueIndex() const{
	return m_queueIndices[COMPUTE_QUEUE];
}

vk::Queue Vulkan::getComputeQueue() const{
	return m_queues[COMPUTE_QUEUE];
}

uint32_t Vulkan::getTransferQueueIndex() const{
	return m_queueIndices[TRANSFER_QUEUE];
}

vk::Queue Vulkan::getTransferQueue() const{
	return m_queues[TRANSFER_QUEUE];
}

uint32_t Vulkan::getPresentationQueueIndex() const{
	return m_queueIndices[PRESENTATION_QUEUE];
}

vk::Queue Vulkan::getPresentationQueue() const{
	return m_queues[PRESENTATION_QUEUE];
}

const Vulkan::FormatSupport& Vulkan::getFormatSupport() const{
	return m_formatSupport;
}

vk::Sampler Vulkan::getRgbSampler(vk::Filter filter) const{
	return *(m_rgbSamplers[static_cast<size_t>(filter)]);
}

vk::Sampler Vulkan::getYcbcrSampler(vk::Filter filter,
									vk::SamplerYcbcrRange range,
									vk::SamplerYcbcrModelConversion model,
									vk::Format format ) const
{
	return *(m_ycbcrSamplers
			[static_cast<size_t>(format)]
			[static_cast<size_t>(model)]
			[static_cast<size_t>(range)]
			[static_cast<size_t>(filter)] );
}



vk::FormatProperties Vulkan::getFormatFeatures(vk::Format format) const {
	return m_physicalDevice.getFormatProperties(format, m_dispatcher);
}




vk::UniqueSwapchainKHR Vulkan::createSwapchain(const vk::SwapchainCreateInfoKHR& createInfo) const{
	return m_device->createSwapchainKHRUnique(createInfo, nullptr, m_dispatcher);
}
vk::UniqueImageView Vulkan::createImageView(const vk::ImageViewCreateInfo& createInfo) const{
	return m_device->createImageViewUnique(createInfo, nullptr, m_dispatcher);
}

vk::UniqueRenderPass Vulkan::createRenderPass(const vk::RenderPassCreateInfo& createInfo) const{
	return m_device->createRenderPassUnique(createInfo, nullptr, m_dispatcher);
}

vk::UniqueShaderModule Vulkan::createShader(const Utils::BufferView<uint32_t>& code) const {
	const vk::ShaderModuleCreateInfo createInfo(
		{},												//Flags
		code.size() * sizeof(uint32_t), code.data()		//Code
	);

	return m_device->createShaderModuleUnique(createInfo, nullptr, m_dispatcher);
}

vk::UniquePipelineLayout Vulkan::createPipelineLayout(const vk::PipelineLayoutCreateInfo& createInfo) const{
	return m_device->createPipelineLayoutUnique(createInfo, nullptr, m_dispatcher);
}

vk::UniquePipeline Vulkan::createGraphicsPipeline(	vk::PipelineCache cache,
													const vk::GraphicsPipelineCreateInfo& createInfo ) const 
{
	return m_device->createGraphicsPipelineUnique(cache, createInfo, nullptr, m_dispatcher);
}

vk::UniqueFramebuffer Vulkan::createFramebuffer(const vk::FramebufferCreateInfo& createInfo) const{
	return m_device->createFramebufferUnique(createInfo, nullptr, m_dispatcher);
}

vk::UniqueCommandPool Vulkan::createCommandPool(const vk::CommandPoolCreateInfo& createInfo) const{
	return m_device->createCommandPoolUnique(createInfo, nullptr, m_dispatcher);
}

std::vector<vk::UniqueCommandBuffer> Vulkan::allocateCommnadBuffers(const vk::CommandBufferAllocateInfo& allocInfo) const{
	return m_device->allocateCommandBuffersUnique(allocInfo, m_dispatcher);
}

vk::UniqueSemaphore Vulkan::createSemaphore() const {
	const vk::SemaphoreCreateInfo createInfo;
	return m_device->createSemaphoreUnique(createInfo, nullptr, m_dispatcher);
}

vk::UniqueFence Vulkan::createFence(bool signaled) const {
	const vk::FenceCreateInfo createInfo(
		signaled ? vk::FenceCreateFlags(vk::FenceCreateFlagBits::eSignaled) : vk::FenceCreateFlags()
	);
	return m_device->createFenceUnique(createInfo, nullptr, m_dispatcher);
}




vk::DispatchLoaderDynamic Vulkan::createDispatcher(const vk::DynamicLoader& loader){
	vk::DispatchLoaderDynamic dispatch;

	auto vkGetInstanceProcAddr = loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	dispatch.init(vkGetInstanceProcAddr);

	return dispatch;
}

vk::UniqueInstance Vulkan::createInstance(	vk::DispatchLoaderDynamic& disp,
											const char* appName, 
											uint32_t appVersion )
{
	//Set the application information
	vk::ApplicationInfo appInfo(
		appName,
		appVersion,
		"Zuazo",
		toVulkan(runtimeVersion),
		VK_API_VERSION_1_1
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

vk::UniqueDebugUtilsMessengerEXT Vulkan::createMessenger(	const vk::DispatchLoaderDynamic& disp,
															const vk::Instance& instance ) 
{
	#ifdef ZUAZO_ENABLE_VALIDATION_LAYERS
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

vk::PhysicalDevice Vulkan::getBestPhysicalDevice(	const vk::DispatchLoaderDynamic& disp,
													const vk::Instance& instance, 
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

std::array<uint32_t, Vulkan::QUEUE_NUM>	Vulkan::getQueueIndices(const vk::DispatchLoaderDynamic& disp,
																const vk::Instance& inst, 
																const vk::PhysicalDevice& dev )
{
	std::array<uint32_t, Vulkan::QUEUE_NUM>	queues;

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


vk::UniqueDevice Vulkan::createDevice(	vk::DispatchLoaderDynamic& disp,
										const vk::PhysicalDevice& physicalDevice,
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

	//Select enabled features
	auto availableFeatures = physicalDevice.getFeatures2<	vk::PhysicalDeviceFeatures2, 
															vk::PhysicalDeviceSamplerYcbcrConversionFeatures>(disp);
	decltype(availableFeatures) enabledFeatures;

	if(availableFeatures.get<vk::PhysicalDeviceSamplerYcbcrConversionFeatures>().samplerYcbcrConversion){
		//Enable YCbCr conversion
		enabledFeatures.get<vk::PhysicalDeviceSamplerYcbcrConversionFeatures>().samplerYcbcrConversion = true;
	}

	//Create it
	vk::DeviceCreateInfo createInfo(
		{},
		queueCreateInfos.size(), queueCreateInfos.data(),			//Queue families
		layerNames.size(), layerNames.data(),						//Validation layers
		extensionNames.size(), extensionNames.data()				//Extensions
	);
	createInfo.pNext = static_cast<const void*>(&(enabledFeatures.get<vk::PhysicalDeviceFeatures2>()));

	auto dev = physicalDevice.createDeviceUnique(createInfo, nullptr, disp);

	//Register the device specific functions 
	disp.init(*dev);

	return dev;
}

std::array<vk::Queue, Vulkan::QUEUE_NUM> Vulkan::getQueues(	const vk::DispatchLoaderDynamic& disp,
															const vk::Device& device, 
															const std::array<uint32_t, QUEUE_NUM>& queueIndices )
{
	std::array<vk::Queue, Vulkan::QUEUE_NUM> queues;

	for(size_t i = 0; i < QUEUE_NUM; i++){
		queues[i] = device.getQueue(queueIndices[i], 0, disp);
	}

	return queues;
}

Vulkan::FormatSupport Vulkan::getFormatSupport(	const vk::DispatchLoaderDynamic& disp, 
												const vk::PhysicalDevice& physicalDevice )
{
	constexpr std::array FORMAT_RANGES = {
		std::pair{VK_FORMAT_BEGIN_RANGE, VK_FORMAT_END_RANGE }, //Normal formats
		std::pair{VK_FORMAT_G8B8G8R8_422_UNORM, VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM } //YCbCr formats
	};

	Vulkan::FormatSupport result;

	for(const auto& range : FORMAT_RANGES){
		for(size_t i = range.first; i <= range.second; i++){
			const auto format = static_cast<vk::Format>(i);

			const auto properties = physicalDevice.getFormatProperties(format, disp);

			if(hasSamplerSupport(properties)){
				result.sampler.emplace_back(format);
				
				if(hasYCbCrSupport(properties)){
					result.ycbcrSampler.emplace_back(format);
				}
			}

			if(hasFramebufferSupport(properties)){
				result.framebuffer.emplace_back(format);
			}
		}
	}

	return result;
}

Vulkan::Samplers Vulkan::createRgbSamplers(	const vk::DispatchLoaderDynamic& disp, 
											const vk::Device& device )
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

std::vector<Vulkan::YcbcrSamplers> Vulkan::createYcbcrSamplers(	const vk::DispatchLoaderDynamic& disp, 
																const vk::PhysicalDevice& physicalDevice,
																const vk::Device& device,
																const std::vector<vk::Format>& formats )
{
	std::vector<Vulkan::YcbcrSamplers> result;
	result.reserve(formats.size());

	for(const auto format : formats){
		const auto properties = physicalDevice.getFormatProperties(format, disp);

		const auto chromaLocation = 
			(properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eMidpointChromaSamples) 
			? vk::ChromaLocation::eMidpoint
			: vk::ChromaLocation::eCositedEven;

		YcbcrSamplers samplers;

		//For each combination, create a sampler (20 samplers)
		for(size_t i = 0; i < samplers.size(); i++){ //All color models (5)
			const auto colorModel = static_cast<vk::SamplerYcbcrModelConversion>(i);

			for(size_t j = 0; j < samplers[i].size(); j++){ //All color ranges (2)
				const auto colorRange = static_cast<vk::SamplerYcbcrRange>(j);
				
				for(size_t h = 0; h < samplers[i][j].size(); h++){ //All filters (2)
					const auto filter = static_cast<vk::Filter>(h);

					const vk::SamplerYcbcrConversionCreateInfo ycbcrCreateInfo(
						format,													//Pixel format
						colorModel,												//Color model
						colorRange,												//Color range
						{},														//Swizzle
						chromaLocation,											//X chroma location
						chromaLocation,											//Y chroma location
						filter,													//Reconstruction filter
						false													//Force explicit reconstruction
					);

					const auto ycbcrConversion = device.createSamplerYcbcrConversionUnique(ycbcrCreateInfo, nullptr, disp);				
					const vk::SamplerYcbcrConversionInfo ycbcrInfo(*ycbcrConversion);


					vk::SamplerCreateInfo createInfo(
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
					createInfo.pNext = &ycbcrInfo;

					samplers[i][j][h] = device.createSamplerUnique(createInfo, nullptr, disp);
				}
			}
		}

		result.emplace_back(std::move(samplers));
	}

	assert(result.size() == formats.size());
	return result;
}


std::vector<vk::LayerProperties> Vulkan::getRequiredLayers(){
	std::vector<vk::LayerProperties> validationLayers;

	#ifdef ZUAZO_ENABLE_VALIDATION_LAYERS
		//Add debug utils extension requirement
		//Khronos validation layer
		validationLayers.emplace_back();
		std::strncpy(validationLayers.back().layerName, "VK_LAYER_KHRONOS_validation", VK_MAX_EXTENSION_NAME_SIZE);
	#endif

	return validationLayers;
}

std::vector<vk::ExtensionProperties> Vulkan::getRequiredInstanceExtensions(){
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

std::vector<vk::ExtensionProperties> Vulkan::getRequiredDeviceExtensions(){
	std::vector<vk::ExtensionProperties> extensions;

	#ifndef ZUAZO_DISABLE_PRESENTATION_SUPPORT
		//Require swapchain extension
		VkExtensionProperties swapchainExtension = {};
		std::strncpy(swapchainExtension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
		extensions.emplace_back(swapchainExtension);
	#endif

	return extensions;
}

std::vector<vk::QueueFamilyProperties> Vulkan::getRequiredQueueFamilies(){
	return {
		VkQueueFamilyProperties{ VK_QUEUE_GRAPHICS_BIT,	1,	0,	{} },
		VkQueueFamilyProperties{ VK_QUEUE_COMPUTE_BIT,	1,	0,	{} },
		VkQueueFamilyProperties{ VK_QUEUE_TRANSFER_BIT,	1,	0,	{} }
	};
}

}
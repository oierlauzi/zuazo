#include <zuazo/Graphics/Vulkan.h>

#include "VulkanUtils.h"

#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Utils/Functions.h>
#include <zuazo/Zuazo.h>
#include <zuazo/Exception.h>
#include <zuazo/Macros.h>

#include <cstring>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <set>
#include <unordered_map>
#include <optional>

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

	LogCallback 									logCallback;

	vk::DynamicLoader								loader;
	vk::DispatchLoaderDynamic						dispatcher;
	vk::UniqueInstance								instance;
	vk::UniqueDebugUtilsMessengerEXT				messenger;
	vk::PhysicalDevice								physicalDevice;
	vk::PhysicalDeviceProperties					physicalDeviceProperties;
	std::array<uint32_t, QUEUE_NUM>					queueIndices;
	vk::UniqueDevice								device;
	std::array<vk::Queue, QUEUE_NUM>				queues;
	vk::UniquePipelineCache							pipelineCache;
	FormatSupport									formatSupport;

	/*
	 * Cached Vulkan objects
	 */

	template<typename T>
	using HashMap = std::unordered_map<size_t, T>;

	mutable HashMap<vk::UniqueRenderPass>			renderPasses;
	mutable HashMap<vk::UniqueShaderModule>			shaders;
	mutable HashMap<vk::UniquePipelineLayout>		pipelineLayouts;
	mutable HashMap<vk::UniqueDescriptorSetLayout>	descriptorSetLayouts;
	mutable HashMap<vk::UniqueSampler>				samplers;

	/*
	 * Deferred present
	 */

	mutable std::vector<vk::SwapchainKHR>			presentSwapchains;
	mutable std::vector<uint32_t>					presentIndices;
	mutable std::vector<vk::Semaphore>				presentSemaphores;


	/*
	 * Methods
	 */
	Impl(	const std::string&  appName, 
			Version appVersion,
			Verbosity verbosity,
			std::vector<vk::ExtensionProperties> requiredInstanceExtensions,	
			std::vector<vk::ExtensionProperties> requiredDeviceExtensions,
			LogCallback logCallback,
			const PresentationSupportCallback& presentationSupportCbk,
			const DeviceScoreFunc& scoreFunc )
		: logCallback(std::move(logCallback))
		, loader()
		, dispatcher(createDispatcher(loader))
		, instance(createInstance(dispatcher, appName.c_str(), toVulkan(appVersion), std::move(requiredInstanceExtensions)))
		, messenger(createMessenger(dispatcher, *instance, verbosity, this))
		, physicalDevice(getBestPhysicalDevice(dispatcher, *instance, requiredDeviceExtensions, presentationSupportCbk, scoreFunc))
		, physicalDeviceProperties(getPhysicalDeviceProperties(dispatcher, physicalDevice))
		, queueIndices(getQueueIndices(dispatcher, *instance, physicalDevice, presentationSupportCbk))
		, device(createDevice(dispatcher, physicalDevice, queueIndices, std::move(requiredDeviceExtensions)))
		, queues(getQueues(dispatcher, *device, queueIndices))
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

	const vk::PhysicalDeviceProperties&	getPhysicalDeviceProperties() const {
		return physicalDeviceProperties;
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

	vk::UniquePipeline createGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& createInfo ) const 
	{
		auto result = device->createGraphicsPipelineUnique(*pipelineCache, createInfo, nullptr, dispatcher);
		return std::move(result.value);
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


	vk::UniqueRenderPass createRenderPass(const vk::RenderPassCreateInfo& createInfo) const{
		return device->createRenderPassUnique(createInfo, nullptr, dispatcher);
	}

	vk::RenderPass createRenderPass(size_t id) const {
		auto ite = renderPasses.find(id);
		return ite != renderPasses.cend() ? *(ite->second) : vk::RenderPass();
	}

	vk::RenderPass createRenderPass(size_t id,
									const vk::RenderPassCreateInfo& createInfo ) const
	{
		auto [ite, result] = renderPasses.emplace(
			id,
			createRenderPass(createInfo)
		);

		assert(result);
		assert(ite != renderPasses.cend());
		return *(ite->second);
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
		return ite != shaders.cend() ? *(ite->second) : vk::ShaderModule();
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
		return ite != pipelineLayouts.cend() ? *(ite->second) : vk::PipelineLayout();
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
		return ite != descriptorSetLayouts.cend() ? *(ite->second) : vk::DescriptorSetLayout();
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

	vk::UniqueSampler createSampler(const vk::SamplerCreateInfo& createInfo) const {
		return device->createSamplerUnique(createInfo, nullptr, dispatcher);
	}

	vk::Sampler createSampler(size_t id) const {
		auto ite = samplers.find(id);
		return ite != samplers.cend() ? *(ite->second) : vk::Sampler();
	}

	vk::Sampler createSampler(	size_t id,
								const vk::SamplerCreateInfo& createInfo ) const 
	{
		auto [ite, result] = samplers.emplace(
			id,
			createSampler(createInfo)
		);

		assert(result);
		assert(ite != samplers.cend());
		return *(ite->second);
	}



	std::vector<vk::UniqueCommandBuffer> allocateCommnadBuffers(const vk::CommandBufferAllocateInfo& allocInfo) const{
		return device->allocateCommandBuffersUnique(allocInfo, dispatcher);
	}

	vk::UniqueCommandBuffer allocateCommnadBuffer(const vk::CommandBufferAllocateInfo& allocInfo) const {
		assert(allocInfo.commandBufferCount == 1);
		auto vec = device->allocateCommandBuffersUnique(allocInfo, dispatcher);
		return std::move(vec[0]);
	}

	void resetCommandPool(	vk::CommandPool cmdPool, 
							vk::CommandPoolResetFlags flags ) const
	{
		device->resetCommandPool(cmdPool, flags, dispatcher);
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

	AggregatedAllocation allocateMemory(Utils::BufferView<const vk::MemoryRequirements> requirements,
										vk::MemoryPropertyFlags properties ) const
	{
		AggregatedAllocation result;
		vk::MemoryRequirements combinedRequirements(0, 1, ~(0U)); //Size, aligment, flags

		result.areas.reserve(requirements.size());

		//Combine all fields and evaluate offsets
		for(size_t i = 0; i < requirements.size(); i++){
			//Align the storage
			combinedRequirements.size = Utils::align(combinedRequirements.size, requirements[i].alignment);

			//Previous size will be the new offset
			result.areas.emplace_back(combinedRequirements.size, requirements[i].size);

			combinedRequirements.size += requirements[i].size; //Increment the size
			combinedRequirements.memoryTypeBits &= requirements[i].memoryTypeBits; //Restrict the flags
			combinedRequirements.alignment = std::max(combinedRequirements.alignment, requirements[i].alignment); //Restrict the alignment
		}

		result.memory = allocateMemory(combinedRequirements, properties);
		return result;
	}



	vk::MemoryRequirements getMemoryRequirements(vk::Buffer buf) const {
		return device->getBufferMemoryRequirements(buf, dispatcher);
	}

	vk::MemoryRequirements getMemoryRequirements(vk::Image img) const {
		return device->getImageMemoryRequirements(img, dispatcher);
	}



	void bindMemory(vk::Buffer buf, vk::DeviceMemory mem, size_t off) const {
		device->bindBufferMemory(buf, mem, off, dispatcher);
	}

	void bindMemory(vk::Image img, vk::DeviceMemory mem, size_t off) const {
		device->bindImageMemory(img, mem, off, dispatcher);
	}



	std::byte* mapMemory(const vk::MappedMemoryRange& range) const{
		return static_cast<std::byte*>(device->mapMemory(
			range.memory,												//Memory allocation
			range.offset,												//Offset
			range.size,													//Size
			{},															//Flags
			dispatcher													//Dispatcher
		));
	}

	std::byte* mapMemory(	vk::DeviceMemory memory, 
							size_t off, 
							size_t size ) const
	{
		return mapMemory(vk::MappedMemoryRange(memory, off, size));
	}

	void flushMappedMemory(const vk::MappedMemoryRange& range) const {
		device->flushMappedMemoryRanges(
			range,
			dispatcher
		);
	}

	void flushMappedMemory(	vk::DeviceMemory memory, 
							size_t off, 
							size_t size ) const
	{
		flushMappedMemory(vk::MappedMemoryRange(memory, off, size));
	}



	void updateDescriptorSets(	Utils::BufferView<const vk::WriteDescriptorSet> write,
								Utils::BufferView<const vk::CopyDescriptorSet> copy ) const
	{
		device->updateDescriptorSets(
			toVulkan(write),
			toVulkan(copy),
			dispatcher
		);
	}

	void updateDescriptorSets(Utils::BufferView<const vk::WriteDescriptorSet> write) const {
		updateDescriptorSets(write, {});
	}

	void updateDescriptorSets(Utils::BufferView<const vk::CopyDescriptorSet> copy) const {
		updateDescriptorSets({}, copy);
	}



	void waitIdle() const {
		device->waitIdle(dispatcher);
	}

	bool waitForFences(	Utils::BufferView<const vk::Fence> fences,
						bool waitAll,
						uint64_t timeout ) const
	{
		using FenceArray = vk::ArrayProxy<const vk::Fence>;

		return vk::Result::eSuccess == device->waitForFences(
			FenceArray(fences.size(), fences.data()),		//Fences
			waitAll,										//Wait all
			timeout,										//Timeout
			dispatcher										//Dispatcher
		);
	}

	void resetFences(Utils::BufferView<const vk::Fence> fences) const { 
		using FenceArray = vk::ArrayProxy<const vk::Fence>;

		device->resetFences(
			FenceArray(fences.size(), fences.data()),		//Fences
			dispatcher										//Dispatcher
		);
	}


	void submit(vk::Queue queue,
				Utils::BufferView<const vk::SubmitInfo> subInfo,
				vk::Fence fence ) const
	{
		using SubInfoArray = vk::ArrayProxy<const vk::SubmitInfo>;

		queue.submit(
			SubInfoArray(subInfo.size(), subInfo.data()),	//Submit infos
			fence,											//Fence
			dispatcher										//Dispatcher
		);
	}


	void begin(	vk::CommandBuffer cmd,
				const vk::CommandBufferBeginInfo& beginInfo ) const
	{
		cmd.begin(beginInfo, dispatcher);
	}

	void end(vk::CommandBuffer cmd) const {
		cmd.end(dispatcher);
	}

	void beginRenderPass(	vk::CommandBuffer cmd, 
							const vk::RenderPassBeginInfo& beginInfo, 
							vk::SubpassContents contents ) const
	{
		cmd.beginRenderPass(beginInfo, contents, dispatcher);
	}

	void endRenderPass(vk::CommandBuffer cmd) const {
		cmd.endRenderPass(dispatcher);
	}

	void bindPipeline(	vk::CommandBuffer cmd, 
						vk::PipelineBindPoint bindPoint, 
						vk::Pipeline pipeline ) const
	{
		cmd.bindPipeline(bindPoint, pipeline, dispatcher);
	}

	void bindVertexBuffers(	vk::CommandBuffer cmd, 
							uint32_t firstBinding, 
							Utils::BufferView<const vk::Buffer> buffers, 
							Utils::BufferView<const vk::DeviceSize> offsets ) const
	{
		cmd.bindVertexBuffers(
			firstBinding, 
			toVulkan(buffers), 
			toVulkan(offsets), 
			dispatcher
		);
	}

	void bindDescriptorSets(vk::CommandBuffer cmd, 
							vk::PipelineBindPoint pipelineBindPoint, 
							vk::PipelineLayout layout, 
							uint32_t firstSet, 
							Utils::BufferView<const vk::DescriptorSet> descriptorSets, 
							Utils::BufferView<const uint32_t> dynamicOffsets) const
	{
		cmd.bindDescriptorSets(
			pipelineBindPoint, 
			layout, firstSet, 
			toVulkan(descriptorSets), 
			toVulkan(dynamicOffsets), 
			dispatcher
		);
	}

	void draw(	vk::CommandBuffer cmd, 
				uint32_t vertexCount, 
				uint32_t instanceCount, 
				uint32_t firstVertex, 
				uint32_t firstInstance ) const
	{
		cmd.draw(vertexCount, instanceCount, firstVertex, firstInstance, dispatcher);
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
												uint32_t appVersion,
												std::vector<vk::ExtensionProperties> ext )
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
				missingNames += m.layerName.data();
				missingNames += "\n";
			}

			throw Exception("Missing Vulkan validation layers:\n" + missingNames);
		}

		const auto usedLayerNames = getNames(usedLayers);


		//Get the extensions
		auto requiredExtensions = getRequiredInstanceExtensions(std::move(ext));
		const auto availableExtensions = vk::enumerateInstanceExtensionProperties(nullptr, disp);
		const auto usedExtensions = getUsedExtensions(availableExtensions, requiredExtensions);

		if(requiredExtensions.size()){
			//There are missing extensions
			std::string missingNames;

			for(const auto& m : requiredExtensions){
				missingNames += m.extensionName.data();
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
															vk::Instance instance,
															Verbosity verbosity,
															Impl* usrPtr )
	{
		#ifndef ZUAZO_DISABLE_VALIDATION_LAYERS
			const vk::DebugUtilsMessageSeverityFlagsEXT msgSeverity = toVulkan(verbosity);

			constexpr vk::DebugUtilsMessageTypeFlagsEXT msgTypes = 	
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
				vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

			const vk::DebugUtilsMessengerCreateInfoEXT createInfo(
				{},										//Flags
				msgSeverity,							//Message severity flags
				msgTypes,								//Message type flags
				validationLayerCallback,				//Callback ptr
				usrPtr									//User ptr
			);

			return instance.createDebugUtilsMessengerEXTUnique(createInfo, nullptr, disp);
		#else
			ZUAZO_IGNORE_PARAM(disp);
			ZUAZO_IGNORE_PARAM(instance);
			ZUAZO_IGNORE_PARAM(verbosity);
			ZUAZO_IGNORE_PARAM(usrPtr);
			return {};
		#endif
	}

	static vk::PhysicalDevice getBestPhysicalDevice(const vk::DispatchLoaderDynamic& disp, 
													vk::Instance instance,
													std::vector<vk::ExtensionProperties> ext,
													const PresentationSupportCallback& presentationSupportCbk,
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
			auto requiredExtensions = getRequiredDeviceExtensions(std::move(ext));
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

			if(getPresentationQueueFamilies(instance, device, availableQueueFamilies.size(), presentationSupportCbk).size() == 0){
				continue;
			}

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

	static vk::PhysicalDeviceProperties getPhysicalDeviceProperties(const vk::DispatchLoaderDynamic& disp, 
																	vk::PhysicalDevice dev)
	{
		return dev.getProperties(disp);
	}

	static std::array<uint32_t, QUEUE_NUM> getQueueIndices(	const vk::DispatchLoaderDynamic& disp, 
															vk::Instance inst, 
															vk::PhysicalDevice dev,
															const PresentationSupportCallback& presentationSupportCbk )
	{
		std::array<uint32_t, QUEUE_NUM>	queues;

		const auto queueFamilies = dev.getQueueFamilyProperties(disp);

		//Add the queue families
		queues[GRAPHICS_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eGraphics);
		queues[COMPUTE_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eCompute);
		queues[TRANSFER_QUEUE] = getQueueFamilyIndex(queueFamilies, vk::QueueFlagBits::eTransfer);

		//Find a queue family compatible with presentation
		const auto presentFamilies = getPresentationQueueFamilies(inst, dev, queueFamilies.size(), presentationSupportCbk);
		assert(presentFamilies.size()); //It should have been checked before

		//Try to assign the graphics queue
		if(std::find(presentFamilies.cbegin(), presentFamilies.cend(), queues[GRAPHICS_QUEUE]) != presentFamilies.cend()){
			queues[PRESENTATION_QUEUE] = queues[GRAPHICS_QUEUE];
		} else {
			queues[PRESENTATION_QUEUE] = presentFamilies[0];
		}

		return queues;
	}

	static vk::UniqueDevice	createDevice(	vk::DispatchLoaderDynamic& disp, 
											vk::PhysicalDevice physicalDevice, 
											const std::array<uint32_t, QUEUE_NUM>& queueIndices,
											std::vector<vk::ExtensionProperties> ext )
	{
		//Get the validation layers and extensions.
		//They should be available, as we have checked for them when
		//choosing the physical device
		const auto& layers = getRequiredLayers();
		const auto layerNames = getNames(layers);
		const auto extensions = getRequiredDeviceExtensions(std::move(ext));
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

	static FormatSupport getFormatSupport(	const vk::DispatchLoaderDynamic& disp, 
											vk::PhysicalDevice physicalDevice )
	{
		FormatSupport result;

		for(const auto& range : FORMAT_RANGES){
			for(auto format = range.getMin(); format <= range.getMax(); reinterpret_cast<int&>(format)++){
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

		#ifndef ZUAZO_DISABLE_VALIDATION_LAYERS
			//Add debug utils extension requirement
			//Khronos validation layer
			validationLayers.emplace_back();
			std::strncpy(validationLayers.back().layerName, "VK_LAYER_KHRONOS_validation", VK_MAX_EXTENSION_NAME_SIZE);
		#endif

		return validationLayers;
	}

	static std::vector<vk::ExtensionProperties> getRequiredInstanceExtensions(std::vector<vk::ExtensionProperties> ext) {
		#ifndef ZUAZO_DISABLE_VALIDATION_LAYERS
			//Add debug utils extension requirement
			ext.push_back(vk::ExtensionProperties(std::array<char, VK_MAX_EXTENSION_NAME_SIZE>{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}));
		#endif

		removeDuplicated(ext);
		return ext;
	}

	static std::vector<vk::ExtensionProperties> getRequiredDeviceExtensions(std::vector<vk::ExtensionProperties> ext) {
		removeDuplicated(ext);
		return ext;
	}

	static std::vector<vk::QueueFamilyProperties> getRequiredQueueFamilies() {
		return {
			VkQueueFamilyProperties{ VK_QUEUE_GRAPHICS_BIT,	1,	0,	{} },
			VkQueueFamilyProperties{ VK_QUEUE_COMPUTE_BIT,	1,	0,	{} },
			VkQueueFamilyProperties{ VK_QUEUE_TRANSFER_BIT,	1,	0,	{} }
		};
	}

	static std::vector<uint32_t> getPresentationQueueFamilies(	vk::Instance instance, 
																vk::PhysicalDevice device, 
																uint32_t count,
																const PresentationSupportCallback& presentationSupportCbk ) {
		std::vector<uint32_t> result;

		for(uint32_t i = 0; i < count; i++) {
			//This queue family can be used for presenting
			if(presentationSupportCbk(instance, device, i)) {
				result.emplace_back(i);
			}
		}

		return result;
	}

	static void removeDuplicated(std::vector<vk::ExtensionProperties>& ext) {
		for(auto ite1 = ext.begin(); ite1 != ext.end(); ++ite1) {
			auto ite2 = ite1 + 1;
			while(ite2 != ext.end()) {
				const auto comp = std::strncmp(
					ite1->extensionName.data(), 
					ite2->extensionName.data(), 
					Math::min(ite1->extensionName.size(), ite2->extensionName.size())
				);

				if(comp == 0) {
					//Extension is duplicate
					ite1->specVersion = Math::max(ite1->specVersion, ite2->specVersion); //Select the newest version
					ite2 = ext.erase(ite2);
				} else {
					//Extension is unique
					++ite2;
				}
			}
		}
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

		Impl* impl = static_cast<Impl*>(userData);
		assert(impl);

		if(impl->logCallback) {
			std::ostringstream message;

			message << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(type)) << ":\n";
			message << "\t" << "messageIDName   = <" << data->pMessageIdName << ">\n";
			message << "\t" << "messageIdNumber = " << data->messageIdNumber << "\n";
			message << "\t" << "message         = <" << data->pMessage << ">\n";

			if(data->queueLabelCount > 0) {
				message << "\t" << "Queue Labels:\n";
				for (uint32_t i = 0; i < data->queueLabelCount; i++) {
					message << "\t\t" << "lableName = <" << data->pQueueLabels[i].pLabelName << ">\n";
				}
			}

			if(data->cmdBufLabelCount > 0) {
				message << "\t" << "CommandBuffer Labels:\n";
				for (uint32_t i = 0; i < data->cmdBufLabelCount; i++) {
					message << "\t\t" << "labelName = <" << data->pCmdBufLabels[i].pLabelName << ">\n";
				}
			}

			if(data->objectCount > 0) {
				message << "\t" << "Objects:\n";
				for (uint32_t i = 0; i < data->objectCount; i++) {
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

			//Invoke the callback
			impl->logCallback(fromVulkan(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)), message.str());
		}

		return VK_FALSE;
	}

};



/*
 * Interface implementation
 */

Vulkan::Vulkan(	const std::string& appName, 
				Version appVersion,
				Verbosity verbosity,
				std::vector<vk::ExtensionProperties> requiredInstanceExtensions,	
				std::vector<vk::ExtensionProperties> requiredDeviceExtensions,
				LogCallback logCallback,
				const PresentationSupportCallback& presentationSupportCbk,
				const DeviceScoreFunc& scoreFunc )
	: m_impl(
		{}, 
		appName, 
		appVersion,
		verbosity, 
		std::move(requiredInstanceExtensions),
		std::move(requiredDeviceExtensions),
		std::move(logCallback), 
		presentationSupportCbk,
		scoreFunc )
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

const vk::PhysicalDeviceProperties&	Vulkan::getPhysicalDeviceProperties() const { 
	return m_impl->getPhysicalDeviceProperties();
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


vk::UniqueRenderPass Vulkan::createRenderPass(const vk::RenderPassCreateInfo& createInfo) const {
	return m_impl->createRenderPass(createInfo);
}

vk::RenderPass Vulkan::createRenderPass(size_t id) const {
	return m_impl->createRenderPass(id);
}

vk::RenderPass Vulkan::createRenderPass(size_t id,
										const vk::RenderPassCreateInfo& createInfo ) const
{
	return m_impl->createRenderPass(id, createInfo);
}

vk::UniqueShaderModule Vulkan::createShaderModule(Utils::BufferView<const uint32_t> code) const {
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

vk::UniqueSampler Vulkan::createSampler(const vk::SamplerCreateInfo& createInfo) const {
	return m_impl->createSampler(createInfo);
}

vk::Sampler Vulkan::createSampler(size_t id) const {
	return m_impl->createSampler(id);
}

vk::Sampler Vulkan::createSampler(	size_t id,
									const vk::SamplerCreateInfo& createInfo ) const 
{
	return m_impl->createSampler(id, createInfo);
}



std::vector<vk::UniqueCommandBuffer> Vulkan::allocateCommnadBuffers(const vk::CommandBufferAllocateInfo& allocInfo) const{
	return m_impl->allocateCommnadBuffers(allocInfo);
}

vk::UniqueCommandBuffer Vulkan::allocateCommnadBuffer(const vk::CommandBufferAllocateInfo& allocInfo) const{
	return m_impl->allocateCommnadBuffer(allocInfo);
}

void Vulkan::resetCommandPool(	vk::CommandPool cmdPool, 
								vk::CommandPoolResetFlags flags ) const
{
	m_impl->resetCommandPool(cmdPool, flags);
}


vk::UniqueDeviceMemory Vulkan::allocateMemory(const vk::MemoryAllocateInfo& allocInfo) const{
	return m_impl->allocateMemory(allocInfo);
}

vk::UniqueDeviceMemory Vulkan::allocateMemory(	const vk::MemoryRequirements& requirements,
												vk::MemoryPropertyFlags properties ) const
{
	return m_impl->allocateMemory(requirements, properties);
}

Vulkan::AggregatedAllocation Vulkan::allocateMemory(Utils::BufferView<const vk::MemoryRequirements> requirements,
													vk::MemoryPropertyFlags properties ) const
{
	return m_impl->allocateMemory(requirements, properties);
}



vk::MemoryRequirements Vulkan::getMemoryRequirements(vk::Buffer buf) const {
	return m_impl->getMemoryRequirements(buf);
}

vk::MemoryRequirements Vulkan::getMemoryRequirements(vk::Image img) const {
	return m_impl->getMemoryRequirements(img);
}


void Vulkan::bindMemory(vk::Buffer buf, vk::DeviceMemory mem, size_t off) const {
	m_impl->bindMemory(buf, mem, off);
}

void Vulkan::bindMemory(vk::Image img, vk::DeviceMemory mem, size_t off) const {
	m_impl->bindMemory(img, mem, off);
}




std::byte* Vulkan::mapMemory(const vk::MappedMemoryRange& range) const{
	return m_impl->mapMemory(range);
}

std::byte* Vulkan::mapMemory(	vk::DeviceMemory memory, 
								size_t off, 
								size_t size ) const
{
	return m_impl->mapMemory(memory, off, size);
}

void Vulkan::flushMappedMemory(const vk::MappedMemoryRange& range) const {
	m_impl->flushMappedMemory(range);
}

void Vulkan::flushMappedMemory(	vk::DeviceMemory memory, 
								size_t off, 
								size_t size ) const
{
	m_impl->flushMappedMemory(memory, off, size);
}



void Vulkan::updateDescriptorSets(	Utils::BufferView<const vk::WriteDescriptorSet> write,
									Utils::BufferView<const vk::CopyDescriptorSet> copy ) const
{
	m_impl->updateDescriptorSets(write, copy);
}

void Vulkan::updateDescriptorSets(Utils::BufferView<const vk::WriteDescriptorSet> write) const {
	m_impl->updateDescriptorSets(write);
}

void Vulkan::updateDescriptorSets(Utils::BufferView<const vk::CopyDescriptorSet> copy) const {
	m_impl->updateDescriptorSets(copy);
}



void Vulkan::waitIdle() const {
	m_impl->waitIdle();
}

bool Vulkan::waitForFences(	Utils::BufferView<const vk::Fence> fences,
							bool waitAll,
							uint64_t timeout ) const
{
	return m_impl->waitForFences(fences, waitAll, timeout);
}

void Vulkan::resetFences(Utils::BufferView<const vk::Fence> fences) const {
	m_impl->resetFences(fences);
}



void Vulkan::submit(vk::Queue queue,
					Utils::BufferView<const vk::SubmitInfo> subInfo,
					vk::Fence fence ) const
{
	m_impl->submit(queue, subInfo, fence);
}


void Vulkan::begin(	vk::CommandBuffer cmd,
					const vk::CommandBufferBeginInfo& beginInfo ) const
{
	m_impl->begin(cmd, beginInfo);
}

void Vulkan::end(vk::CommandBuffer cmd) const {
	m_impl->end(cmd);
}

void Vulkan::beginRenderPass(vk::CommandBuffer cmd, 
							const vk::RenderPassBeginInfo& beginInfo, 
							vk::SubpassContents contents ) const
{
	m_impl->beginRenderPass(cmd, beginInfo, contents);
}

void Vulkan::endRenderPass(vk::CommandBuffer cmd) const {
	m_impl->endRenderPass(cmd);
}

void Vulkan::bindPipeline(	vk::CommandBuffer cmd, 
							vk::PipelineBindPoint bindPoint, 
							vk::Pipeline pipeline ) const
{
	m_impl->bindPipeline(cmd, bindPoint, pipeline);
}

void Vulkan::bindVertexBuffers(	vk::CommandBuffer cmd, 
								uint32_t firstBinding, 
								Utils::BufferView<const vk::Buffer> buffers, 
								Utils::BufferView<const vk::DeviceSize> offsets ) const
{
	m_impl->bindVertexBuffers(cmd, firstBinding, buffers, offsets);
}

void Vulkan::bindDescriptorSets(vk::CommandBuffer cmd, 
								vk::PipelineBindPoint pipelineBindPoint, 
								vk::PipelineLayout layout, 
								uint32_t firstSet, 
								Utils::BufferView<const vk::DescriptorSet> descriptorSets, 
								Utils::BufferView<const uint32_t> dynamicOffsets) const
{
	m_impl->bindDescriptorSets(cmd, pipelineBindPoint, layout, firstSet, descriptorSets, dynamicOffsets);
}

void Vulkan::draw(	vk::CommandBuffer cmd, 
					uint32_t vertexCount, 
					uint32_t instanceCount, 
					uint32_t firstVertex, 
					uint32_t firstInstance ) const
{
	m_impl->draw(cmd, vertexCount, instanceCount, firstVertex, firstInstance);
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
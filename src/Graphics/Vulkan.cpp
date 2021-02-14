#include <zuazo/Graphics/Vulkan.h>

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
	std::array<uint32_t, QUEUE_NUM>					queueIndices;
	DeviceFeatures									deviceFeatures;
	vk::UniqueDevice								device;
	std::array<vk::Queue, QUEUE_NUM>				queues;
	vk::UniquePipelineCache							pipelineCache;
	
	vk::PhysicalDeviceProperties					physicalDeviceProperties;
	FormatSupport									formatSupport;

	/*
	 * Cached Vulkan objects
	 */

	template<typename T>
	using HashMap = std::unordered_map<size_t, T>;

	mutable HashMap<std::vector<vk::Format>>		optimalFormatProperties;
	mutable HashMap<std::vector<vk::Format>>		linearFormatProperties;

	mutable HashMap<vk::UniqueRenderPass>			renderPasses;
	mutable HashMap<vk::UniqueShaderModule>			shaders;
	mutable HashMap<vk::UniquePipelineLayout>		pipelineLayouts;
	mutable HashMap<vk::UniquePipeline>				graphicsPipelines;
	mutable HashMap<vk::UniqueDescriptorSetLayout>	descriptorSetLayouts;
	mutable HashMap<vk::UniqueSamplerYcbcrConversion>	samplerYCbCrConversions;
	mutable HashMap<vk::UniqueSampler>				samplers;

	/*
	 * Deferred present
	 */

	mutable std::vector<vk::SwapchainKHR>			presentSwapchains;
	mutable std::vector<uint32_t>					presentIndices;
	mutable std::vector<vk::Semaphore>				presentSemaphores;

	/*
	 * User pointers
	 */
	HashMap<std::shared_ptr<void>>					userPointers;

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
		, queueIndices(getQueueIndices(dispatcher, *instance, physicalDevice, presentationSupportCbk))
		, deviceFeatures(createDeviceFeatures(dispatcher, physicalDevice))
		, device(createDevice(dispatcher, physicalDevice, queueIndices, deviceFeatures, std::move(requiredDeviceExtensions)))
		, queues(getQueues(dispatcher, *device, queueIndices))
		, physicalDeviceProperties(getPhysicalDeviceProperties(dispatcher, physicalDevice))
		, formatSupport(getFormatSupport(dispatcher, physicalDevice))
	{
	}

	~Impl() = default;

	const vk::DynamicLoader& getLoader() const noexcept {
		return loader;
	}

	const vk::DispatchLoaderDynamic& getDispatcher() const noexcept {
		return dispatcher;
	}

	vk::Instance getInstance() const noexcept {
		return *instance;
	}

	vk::PhysicalDevice getPhysicalDevice() const noexcept {
		return physicalDevice;
	}

	vk::Device getDevice() const noexcept {
		return *device;
	}

	uint32_t getGraphicsQueueIndex() const noexcept {
		return queueIndices[GRAPHICS_QUEUE];
	}

	vk::Queue getGraphicsQueue() const noexcept {
		return queues[GRAPHICS_QUEUE];
	}

	uint32_t getComputeQueueIndex() const noexcept {
		return queueIndices[COMPUTE_QUEUE];
	}

	vk::Queue getComputeQueue() const noexcept {
		return queues[COMPUTE_QUEUE];
	}

	uint32_t getTransferQueueIndex() const noexcept {
		return queueIndices[TRANSFER_QUEUE];
	}

	vk::Queue getTransferQueue() const noexcept {
		return queues[TRANSFER_QUEUE];
	}

	uint32_t getPresentationQueueIndex() const noexcept {
		return queueIndices[PRESENTATION_QUEUE];
	}

	vk::Queue getPresentationQueue() const noexcept {
		return queues[PRESENTATION_QUEUE];
	}

	vk::PipelineCache getPipelineCache() const noexcept {
		return *pipelineCache;
	}



	const vk::PhysicalDeviceProperties&	getPhysicalDeviceProperties() const noexcept {
		return physicalDeviceProperties;
	}

	const FormatSupport& getFormatSupport() const noexcept {
		return formatSupport;
	}

	const DeviceFeatures& getDeviceFeatures() const noexcept {
		return deviceFeatures;
	}

	const std::vector<vk::Format>& listSupportedFormatsOptimal(vk::FormatFeatureFlags flags) const {
		const auto id = static_cast<vk::FormatFeatureFlags::MaskType>(flags);
		auto ite = optimalFormatProperties.find(id);

		if(ite == optimalFormatProperties.cend()) {
			//This flags where not tested before
			std::vector<vk::Format> result;

			for(const auto& feature : getFormatSupport()) {
				//Test if all the flags are set to '1'
				if((feature.second.optimalTilingFeatures & flags) == flags) {
					result.push_back(feature.first);
				}
			}

			//Sort in order to use binary search if needed
			std::sort(result.begin(), result.end());
			std::tie(ite, std::ignore) = optimalFormatProperties.emplace(
				id,
				std::move(result)
			);
		}

		assert(ite != optimalFormatProperties.cend());
		assert(std::is_sorted(ite->second.cbegin(), ite->second.cend()));
		return ite->second;
	}

	const std::vector<vk::Format>& listSupportedFormatsLinear(vk::FormatFeatureFlags flags) const {
		const auto id = static_cast<vk::FormatFeatureFlags::MaskType>(flags);
		auto ite = linearFormatProperties.find(id);

		if(ite == linearFormatProperties.cend()) {
			//This flags where not tested before
			std::vector<vk::Format> result;

			for(const auto& feature : getFormatSupport()) {
				//Test if all the flags are set to '1'
				if((feature.second.linearTilingFeatures & flags) == flags) {
					result.push_back(feature.first);
				}
			}

			//Sort in order to use binary search if needed
			std::sort(result.begin(), result.end());
			std::tie(ite, std::ignore) = linearFormatProperties.emplace(
				id,
				std::move(result)
			);
		}

		assert(ite != linearFormatProperties.cend());
		assert(std::is_sorted(ite->second.cbegin(), ite->second.cend()));
		return ite->second;
	}



	vk::FormatProperties getFormatFeatures(vk::Format format) const{
		return physicalDevice.getFormatProperties(format, dispatcher);
	}


	vk::UniqueSwapchainKHR createSwapchain(const vk::SwapchainCreateInfoKHR& createInfo) const {
		//It is a extension so support should have been checked before calling
		if(dispatcher.vkCreateSwapchainKHR) {
			return device->createSwapchainKHRUnique(createInfo, nullptr, dispatcher);
		} else {
			throw Exception("Unsupported Vulkan function call: vkCreateSwapchainKHR");
		}
	}

	vk::UniqueImage createImage(const vk::ImageCreateInfo& createInfo) const{
		return device->createImageUnique(createInfo, nullptr, dispatcher);
	}

	vk::UniqueImageView createImageView(const vk::ImageViewCreateInfo& createInfo) const{
		return device->createImageViewUnique(createInfo, nullptr, dispatcher);
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

	vk::UniquePipeline createGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& createInfo) const 
	{
		auto result = device->createGraphicsPipelineUnique(*pipelineCache, createInfo, nullptr, dispatcher);
		return std::move(result.value);
	}

	vk::Pipeline createGraphicsPipeline(size_t id) const {
		auto ite = graphicsPipelines.find(id);
		return ite != graphicsPipelines.cend() ? *(ite->second) : vk::Pipeline();
	}

	vk::Pipeline createGraphicsPipeline(size_t id,
										const vk::GraphicsPipelineCreateInfo& createInfo ) const
	{
		auto [ite, result] = graphicsPipelines.emplace(
			id,
			createGraphicsPipeline(createInfo)
		);

		assert(result);
		assert(ite != graphicsPipelines.cend());
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

	vk::UniqueSamplerYcbcrConversion createSamplerYcbcrConversion(const vk::SamplerYcbcrConversionCreateInfo& createInfo) const {
		//It is a extension so support should have been checked before calling
		if(dispatcher.vkCreateSamplerYcbcrConversion) {
			return device->createSamplerYcbcrConversionUnique(createInfo, nullptr, dispatcher);
		} else if(dispatcher.vkCreateSamplerYcbcrConversionKHR) {
			return device->createSamplerYcbcrConversionKHRUnique(createInfo, nullptr, dispatcher);
		} else {
			throw Exception("Unsupported Vulkan function call: vkCreateSamplerYcbcrConversion");
		}		
	}

	vk::SamplerYcbcrConversion createSamplerYcbcrConversion(size_t id) const {
		auto ite = samplerYCbCrConversions.find(id);
		return ite != samplerYCbCrConversions.cend() ? *(ite->second) : vk::SamplerYcbcrConversion();
	}

	vk::SamplerYcbcrConversion createSamplerYcbcrConversion(	size_t id,
																const vk::SamplerYcbcrConversionCreateInfo& createInfo ) const 
	{
		auto [ite, result] = samplerYCbCrConversions.emplace(
			id,
			createSamplerYcbcrConversion(createInfo)
		);

		assert(result);
		assert(ite != samplerYCbCrConversions.cend());
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

	vk::UniqueCommandBuffer allocateCommnadBuffer(	vk::CommandPool commandPool, 
													vk::CommandBufferLevel level) const 
	{
		const vk::CommandBufferAllocateInfo allocInfo(
			commandPool, 
			level, 
			1
		);
		const vk::PoolFree<vk::Device, vk::CommandPool, vk::DispatchLoaderDynamic> deleter(
			*device, 
			allocInfo.commandPool, 
			dispatcher
		);
		assert(allocInfo.commandBufferCount == 1);

		vk::CommandBuffer commandBuffer;
		const auto result = device->allocateCommandBuffers(&allocInfo, &commandBuffer, dispatcher);
		vk::UniqueCommandBuffer uniqueCommandBuffer(commandBuffer, deleter);

		return vk::createResultValue(
			result, 
			uniqueCommandBuffer, 
			VULKAN_HPP_NAMESPACE_STRING"::Device::allocateCommandBuffers"
		);
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
		vk::UniqueDeviceMemory result;
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

		if(i < memoryProperties.memoryTypeCount){
			//Found a suitable memory type
			const vk::MemoryAllocateInfo allocInfo(
				requirements.size,						//Size
				i										//Memory type index
			);

			result = allocateMemory(allocInfo);
		} else {
			//Did not find any suitable memory
			if(properties & vk::MemoryPropertyFlagBits::eLazilyAllocated) {
				//Try to allocate without the lazy allocation bit
				constexpr vk::MemoryPropertyFlags MASK = ~vk::MemoryPropertyFlagBits::eLazilyAllocated;
				result = allocateMemory(requirements, properties & MASK);
			} else {
				//No solution :-<
				throw Exception("Error allocating device memory");
			}
		}

		return result;
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



	std::vector<vk::UniqueDescriptorSet> allocateDescriptorSets(const vk::DescriptorSetAllocateInfo& allocInfo) const {
		return device->allocateDescriptorSetsUnique(allocInfo, dispatcher);
	}

	vk::UniqueDescriptorSet allocateDescriptorSet(	vk::DescriptorPool pool, 
													vk::DescriptorSetLayout layout) const
	{
		const vk::DescriptorSetAllocateInfo allocInfo(
			pool,													//Pool
			1, &layout												//Layouts
		);
		const vk::PoolFree<vk::Device, vk::DescriptorPool, vk::DispatchLoaderDynamic> deleter(
			*device, 
			allocInfo.descriptorPool, 
			dispatcher
		);
		assert(allocInfo.descriptorSetCount == 1);

		vk::DescriptorSet descriptorSet;
		const auto result = device->allocateDescriptorSets(&allocInfo, &descriptorSet, dispatcher);
		vk::UniqueDescriptorSet uniqueDescriptorSet(descriptorSet, deleter);

		return vk::createResultValue(
			result, 
			uniqueDescriptorSet, 
			VULKAN_HPP_NAMESPACE_STRING"::Device::allocateDescriptorSets"
		);
	}



	vk::MemoryRequirements getMemoryRequirements(vk::Buffer buf) const {
		return device->getBufferMemoryRequirements(buf, dispatcher);
	}

	vk::MemoryRequirements getMemoryRequirements(vk::Image img) const {
		return device->getImageMemoryRequirements(img, dispatcher);
	}

	vk::MemoryRequirements2 getMemoryRequirements(const vk::BufferMemoryRequirementsInfo2& reqInfo) const {
		if(dispatcher.vkGetBufferMemoryRequirements2) {
			return device->getBufferMemoryRequirements2(reqInfo, dispatcher);
		} else if(dispatcher.vkGetBufferMemoryRequirements2KHR) {
			return device->getBufferMemoryRequirements2KHR(reqInfo, dispatcher);
		} else {
			//Extension not supported, manually implement if possible
			if(reqInfo.pNext) throw Exception("Unsupported Vulkan function call: getBufferMemoryRequirements2");
			return vk::MemoryRequirements2(getMemoryRequirements(reqInfo.buffer));
		}
	}

	vk::MemoryRequirements2 getMemoryRequirements(const vk::ImageMemoryRequirementsInfo2& reqInfo) const {
		if(dispatcher.vkGetImageMemoryRequirements2) {
			return device->getImageMemoryRequirements2(reqInfo, dispatcher);
		} else if(dispatcher.vkGetImageMemoryRequirements2KHR) {
			return device->getImageMemoryRequirements2KHR(reqInfo, dispatcher);
		} else {
			//Extension not supported, manually implement if possible
			if(reqInfo.pNext) throw Exception("Unsupported Vulkan function call: vkGetImageMemoryRequirements2");
			return vk::MemoryRequirements2(getMemoryRequirements(reqInfo.image));
		}
	}



	void bindMemory(vk::Buffer buf, vk::DeviceMemory mem, size_t off) const {
		device->bindBufferMemory(buf, mem, off, dispatcher);
	}

	void bindMemory(vk::Image img, vk::DeviceMemory mem, size_t off) const {
		device->bindImageMemory(img, mem, off, dispatcher);
	}

	void bindMemory(Utils::BufferView<const vk::BindBufferMemoryInfo> bindInfos) const {
		if(dispatcher.vkBindBufferMemory2) {
			device->bindBufferMemory2(toVulkan(bindInfos), dispatcher);
		} else if(dispatcher.vkBindBufferMemory2KHR) {
			device->bindBufferMemory2KHR(toVulkan(bindInfos), dispatcher);
		} else {
			//Extension not supported, manually implement if possible
			for(const auto& bindInfo : bindInfos) {
				if(bindInfo.pNext) throw Exception("Unsupported Vulkan function call: vkBindBufferMemory2");
				bindMemory(bindInfo.buffer, bindInfo.memory, bindInfo.memoryOffset);
			}
		}
	}

	void bindMemory(Utils::BufferView<const vk::BindImageMemoryInfo> bindInfos) const {
		if(dispatcher.vkBindImageMemory2) {
			device->bindImageMemory2(toVulkan(bindInfos), dispatcher);
		} else if(dispatcher.vkBindImageMemory2KHR) {
			device->bindImageMemory2KHR(toVulkan(bindInfos), dispatcher);
		} else {
			//Extension not supported, manually implement if possible
			for(const auto& bindInfo : bindInfos) {
				if(bindInfo.pNext) throw Exception("Unsupported Vulkan function call: vkBindImageMemory2");
				bindMemory(bindInfo.image, bindInfo.memory, bindInfo.memoryOffset);
			}
		}
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


	void execute(	vk::CommandBuffer cmd,
					Utils::BufferView<const vk::CommandBuffer> buf ) const noexcept
	{
		cmd.executeCommands(toVulkan(buf), dispatcher);
	}


	void pipelineBarrier(	vk::CommandBuffer cmd,
							vk::PipelineStageFlags srcStageMask,
							vk::PipelineStageFlags dstStageMask,
							vk::DependencyFlags dependencyFlags,
							Utils::BufferView<const vk::MemoryBarrier> memoryBarriers,
							Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers,
							Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers ) const noexcept
	{
		cmd.pipelineBarrier(
			srcStageMask, 
			dstStageMask, 
			dependencyFlags, 
			toVulkan(memoryBarriers), 
			toVulkan(bufferMemoryBarriers), 
			toVulkan(imageMemoryBarriers), 
			dispatcher
		);
	}

	void pipelineBarrier(	vk::CommandBuffer cmd,
							vk::PipelineStageFlags srcStageMask,
							vk::PipelineStageFlags dstStageMask,
							vk::DependencyFlags dependencyFlags,
							Utils::BufferView<const vk::MemoryBarrier> memoryBarriers ) const noexcept
	{
		pipelineBarrier(cmd, srcStageMask, dstStageMask, dependencyFlags, memoryBarriers, {}, {});
	}

	void pipelineBarrier(	vk::CommandBuffer cmd,
							vk::PipelineStageFlags srcStageMask,
							vk::PipelineStageFlags dstStageMask,
							vk::DependencyFlags dependencyFlags,
							Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers ) const noexcept
	{
		pipelineBarrier(cmd, srcStageMask, dstStageMask, dependencyFlags, {}, bufferMemoryBarriers, {});
	}

	void pipelineBarrier(	vk::CommandBuffer cmd,
							vk::PipelineStageFlags srcStageMask,
							vk::PipelineStageFlags dstStageMask,
							vk::DependencyFlags dependencyFlags,
							Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers ) const noexcept
	{
		pipelineBarrier(cmd, srcStageMask, dstStageMask, dependencyFlags, {}, {}, imageMemoryBarriers);
	}


	void clear(	vk::CommandBuffer cmd,
				vk::Image image,
				vk::ImageLayout imageLayout,
				const vk::ClearColorValue& value,
				Utils::BufferView<const vk::ImageSubresourceRange> ranges ) const noexcept
	{
		cmd.clearColorImage(image, imageLayout, value, toVulkan(ranges), dispatcher);
	}

	void clear(	vk::CommandBuffer cmd,
				vk::Image image,
				vk::ImageLayout imageLayout,
				const vk::ClearDepthStencilValue& value,
				Utils::BufferView<const vk::ImageSubresourceRange> ranges ) const noexcept
	{
		cmd.clearDepthStencilImage(image, imageLayout, value, toVulkan(ranges), dispatcher);
	}

	void clear(	vk::CommandBuffer cmd,
				Utils::BufferView<const vk::ClearAttachment> attachments,
				Utils::BufferView<const vk::ClearRect> rects ) const noexcept
	{
		cmd.clearAttachments(toVulkan(attachments), toVulkan(rects), dispatcher);
	}

	void clear(	vk::CommandBuffer cmd,
				vk::Buffer buffer,
				const Utils::Area& area,
				uint32_t data ) const noexcept
	{
		cmd.fillBuffer(buffer, area.offset(), area.size(), data, dispatcher);
	}

	void clear(	vk::CommandBuffer cmd,
				vk::Buffer buffer,
				const Utils::Area& area,
				const std::byte* data ) const noexcept
	{
		cmd.updateBuffer(buffer, area.offset(), area.size(), static_cast<const void*>(data), dispatcher);
	}


	void copy(	vk::CommandBuffer cmd,
				vk::Buffer srcBuffer,
				vk::Buffer dstBuffer,
				Utils::BufferView<const vk::BufferCopy> regions ) const noexcept
	{
		cmd.copyBuffer(srcBuffer, dstBuffer, toVulkan(regions), dispatcher);
	}

	void copy(	vk::CommandBuffer cmd,
				vk::Image srcImage,
    			vk::ImageLayout srcImageLayout,
    			vk::Image dstImage,
    			vk::ImageLayout dstImageLayout,
				Utils::BufferView<const vk::ImageCopy> regions ) const noexcept
	{
		cmd.copyImage(srcImage, srcImageLayout, dstImage, dstImageLayout, toVulkan(regions), dispatcher);
	}

	void copy(	vk::CommandBuffer cmd,
				vk::Buffer srcBuffer,
    			vk::Image dstImage,
    			vk::ImageLayout dstImageLayout,
				Utils::BufferView<const vk::BufferImageCopy> regions ) const noexcept
	{
		cmd.copyBufferToImage(srcBuffer, dstImage, dstImageLayout, toVulkan(regions), dispatcher);
	}

	void copy(	vk::CommandBuffer cmd,
				vk::Image srcImage,
    			vk::ImageLayout srcImageLayout,
    			vk::Buffer dstBuffer,
				Utils::BufferView<const vk::BufferImageCopy> regions ) const noexcept
	{
		cmd.copyImageToBuffer(srcImage, srcImageLayout, dstBuffer, toVulkan(regions), dispatcher);
	}

	void blit(	vk::CommandBuffer cmd,
				vk::Image srcImage,
    			vk::ImageLayout srcImageLayout,
    			vk::Image dstImage,
    			vk::ImageLayout dstImageLayout,
				Utils::BufferView<const vk::ImageBlit> regions,
    			vk::Filter filter ) const noexcept
	{
		cmd.blitImage(srcImage, srcImageLayout, dstImage, dstImageLayout, toVulkan(regions), filter, dispatcher);
	}

	void resolve(	vk::CommandBuffer cmd,
					vk::Image srcImage,
					vk::ImageLayout srcImageLayout,
					vk::Image dstImage,
					vk::ImageLayout dstImageLayout,
					Utils::BufferView<const vk::ImageResolve> regions ) const noexcept
	{
		cmd.resolveImage(srcImage, srcImageLayout, dstImage, dstImageLayout, toVulkan(regions), dispatcher);
	}



	void beginRenderPass(	vk::CommandBuffer cmd, 
							const vk::RenderPassBeginInfo& beginInfo, 
							vk::SubpassContents contents ) const noexcept
	{
		cmd.beginRenderPass(beginInfo, contents, dispatcher);
	}

	void endRenderPass(vk::CommandBuffer cmd) const noexcept {
		cmd.endRenderPass(dispatcher);
	}

	void nextSubpass(	vk::CommandBuffer cmd,
						vk::SubpassContents contents ) const noexcept
	{
		cmd.nextSubpass(contents, dispatcher);
	}

	void bindPipeline(	vk::CommandBuffer cmd, 
						vk::PipelineBindPoint bindPoint, 
						vk::Pipeline pipeline ) const noexcept
	{
		cmd.bindPipeline(bindPoint, pipeline, dispatcher);
	}

	void bindVertexBuffers(	vk::CommandBuffer cmd, 
							uint32_t firstBinding, 
							Utils::BufferView<const vk::Buffer> buffers, 
							Utils::BufferView<const vk::DeviceSize> offsets ) const noexcept
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
							Utils::BufferView<const uint32_t> dynamicOffsets) const noexcept
	{
		cmd.bindDescriptorSets(
			pipelineBindPoint, 
			layout, firstSet, 
			toVulkan(descriptorSets), 
			toVulkan(dynamicOffsets), 
			dispatcher
		);
	}

	void setViewport(	vk::CommandBuffer cmd,
						uint32_t first,
						Utils::BufferView<const vk::Viewport> viewports ) const noexcept
	{
		cmd.setViewport(first, viewports.size(), viewports.data(), dispatcher);
	}

	void setScissor(vk::CommandBuffer cmd,
					uint32_t first,
					Utils::BufferView<const vk::Rect2D> scissors ) const noexcept
	{
		cmd.setScissor(first, scissors.size(), scissors.data(), dispatcher);
	}

	void draw(	vk::CommandBuffer cmd, 
				uint32_t vertexCount, 
				uint32_t instanceCount, 
				uint32_t firstVertex, 
				uint32_t firstInstance ) const noexcept
	{
		cmd.draw(vertexCount, instanceCount, firstVertex, firstInstance, dispatcher);
	}

	void draw(	vk::CommandBuffer cmd, 
				vk::Buffer buffer,
				size_t offset,
				uint32_t drawCount,
				uint32_t stride ) const noexcept
	{
		cmd.drawIndirect(buffer, offset, drawCount, stride, dispatcher);
	}

	void drawIndexed(	vk::CommandBuffer cmd, 
						uint32_t indexCount,
						uint32_t instanceCount,
						uint32_t firstIndex,
						int32_t vertexOffset,
						uint32_t firstInstance ) const noexcept
	{
		cmd.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance, dispatcher);
	}
	
	void drawIndexed(	vk::CommandBuffer cmd,
						vk::Buffer buffer,
    					size_t offset,
    					uint32_t drawCount,
    					uint32_t stride ) const noexcept
	{
		cmd.drawIndexedIndirect(buffer, offset, drawCount, stride, dispatcher);
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

			if(dispatcher.vkQueuePresentKHR) {
				switch(getPresentationQueue().presentKHR(&presentInfo, dispatcher)){
				case vk::Result::eSuccess: 
				case vk::Result::eErrorOutOfDateKHR: 
					break;
				default:
					throw Exception("Error presentting images");
				}
			} else {
				throw Exception("Unsupported Vulkan function call: vkQueuePresentKHR");
			}

			presentSemaphores.clear();
			presentSwapchains.clear();
			presentIndices.clear();

			assert(presentSemaphores.size() == 0);
			assert(presentSwapchains.size() == 0);
			assert(presentIndices.size() == 0);
		}
	}



	void setUserPointer(size_t id, std::shared_ptr<void> usrPtr) {
		userPointers[id] = std::move(usrPtr);
	}

	const std::shared_ptr<void>& getUserPointer(size_t id) const noexcept {
		static const std::shared_ptr<void> NOT_FOUND;
		const auto ite = userPointers.find(id);
		return (ite != userPointers.cend()) ? ite->second : NOT_FOUND;
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
			VK_API_VERSION_1_1
		);

		//Get the validation layers
		const auto availableLayers = vk::enumerateInstanceLayerProperties(disp);
		auto requiredLayers = getRequiredLayers();
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
		const auto availableExtensions = vk::enumerateInstanceExtensionProperties(nullptr, disp);
		auto requiredExtensions = getRequiredInstanceExtensions(availableExtensions, std::move(ext));
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
			const auto supportedLayers = device.enumerateDeviceLayerProperties(disp);
			auto requiredLayers = getRequiredLayers();
			getUsedLayers(supportedLayers, requiredLayers);
			if(requiredLayers.size()){
				continue;
			}

			//Check device extension support
			const auto availableExtensions = device.enumerateDeviceExtensionProperties(nullptr, disp);
			auto requiredExtensions = getRequiredDeviceExtensions(availableExtensions, std::move(ext));
			getUsedExtensions(availableExtensions, requiredExtensions);
			if(requiredExtensions.size()){
				continue;
			}

			//Check device queue family support
			const auto availableQueueFamilies = device.getQueueFamilyProperties(disp);
			auto requiredQueueFamilies = getRequiredQueueFamilies();
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

	static DeviceFeatures createDeviceFeatures(	const vk::DispatchLoaderDynamic& disp,
												vk::PhysicalDevice physicalDevice )
	{
		DeviceFeatures result;

		//List the supported features
		DeviceFeatures supported;
		if(disp.vkGetPhysicalDeviceFeatures2) {
			physicalDevice.getFeatures2(&supported.get(), disp);
		} else if(disp.vkGetPhysicalDeviceFeatures2KHR) {
			physicalDevice.getFeatures2KHR(&supported.get(), disp);
		} else {
			physicalDevice.getFeatures(&supported.get().features, disp);
		}

		//Add the YCbCr sampler feature if possible
		if(supported.get<vk::PhysicalDeviceSamplerYcbcrConversionFeatures>().samplerYcbcrConversion) {
			result.get<vk::PhysicalDeviceSamplerYcbcrConversionFeatures>().samplerYcbcrConversion = true;
		}

		return result;
	}

	static vk::UniqueDevice	createDevice(	vk::DispatchLoaderDynamic& disp, 
											vk::PhysicalDevice physicalDevice, 
											const std::array<uint32_t, QUEUE_NUM>& queueIndices,
											const DeviceFeatures& features,
											std::vector<vk::ExtensionProperties> ext )
	{
		//Get the validation layers and extensions.
		//They should be available, as we have checked for them when
		//choosing the physical device
		const auto& layers = getRequiredLayers();
		const auto layerNames = getNames(layers);
		const auto availableExtensions = physicalDevice.enumerateDeviceExtensionProperties(nullptr, disp);
		const auto extensions = getRequiredDeviceExtensions(availableExtensions, std::move(ext));
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
		const auto createInfo = vk::DeviceCreateInfo(
			{},
			queueCreateInfos.size(), queueCreateInfos.data(),			//Queue families
			layerNames.size(), layerNames.data(),						//Validation layers
			extensionNames.size(), extensionNames.data(),				//Extensions
			nullptr														//Features will be specified with pNext
		).setPNext(&features.get());

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
			for(auto format = range.getMin(); format <= range.getMax(); format = static_cast<vk::Format>(static_cast<int>(format) + 1)){
				result.emplace(format, physicalDevice.getFormatProperties(format, disp));
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

	static std::vector<vk::ExtensionProperties> getRequiredInstanceExtensions(	const std::vector<vk::ExtensionProperties>& supported,
																				std::vector<vk::ExtensionProperties> extensions ) 
	{
		#ifndef ZUAZO_DISABLE_VALIDATION_LAYERS
			//Add debug utils extension requirement
			extensions.push_back(vk::ExtensionProperties(std::array<char, VK_MAX_EXTENSION_NAME_SIZE>{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}));
		#endif

		//Add get_physical_device_properties2 if supported
		const auto physicalDeviceProperties2Supported = std::find_if(
			supported.cbegin(), supported.cend(),
			[] (const vk::ExtensionProperties& ext) -> bool {
				return !std::strncmp(
					VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 
					ext.extensionName.data(), 
					ext.extensionName.size()
				);
			}
		);
		if(physicalDeviceProperties2Supported != supported.cend()) {
			extensions.push_back(*physicalDeviceProperties2Supported);
		}

		removeDuplicated(extensions);
		return extensions;
	}

	static std::vector<vk::ExtensionProperties> getRequiredDeviceExtensions(const std::vector<vk::ExtensionProperties>& supported,
																			std::vector<vk::ExtensionProperties> extensions) 
	{
		//Add YCbCr sampler extension if possible
		const auto ycbcrSamplerSupported = std::find_if(
			supported.cbegin(), supported.cend(),
			[] (const vk::ExtensionProperties& ext) -> bool {
				return !std::strncmp(
					VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME, 
					ext.extensionName.data(), 
					ext.extensionName.size()
				);
			}
		);
		if(ycbcrSamplerSupported != supported.cend()) {
			//YCbCr sampler has some dependencies. Assert they are supported and add them to the list
			extensions.emplace_back(std::array<char, VK_MAX_EXTENSION_NAME_SIZE>{VK_KHR_MAINTENANCE1_EXTENSION_NAME});
			extensions.emplace_back(std::array<char, VK_MAX_EXTENSION_NAME_SIZE>{VK_KHR_BIND_MEMORY_2_EXTENSION_NAME});
			extensions.emplace_back(std::array<char, VK_MAX_EXTENSION_NAME_SIZE>{VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME});
			extensions.push_back(*ycbcrSamplerSupported);
		}

		//Add cubic filter extension if possible
		const auto cubicFilterSupported = std::find_if(
			supported.cbegin(), supported.cend(),
			[] (const vk::ExtensionProperties& ext) -> bool {
				return !std::strncmp(
					VK_IMG_FILTER_CUBIC_EXTENSION_NAME, 
					ext.extensionName.data(), 
					ext.extensionName.size()
				);
			}
		);
		if(cubicFilterSupported != supported.cend()) {
			extensions.push_back(*cubicFilterSupported);
		}

		removeDuplicated(extensions);
		return extensions;
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
			auto ite2 = std::next(ite1);
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

	static std::vector<vk::LayerProperties> getUsedLayers(	const std::vector<vk::LayerProperties>& available, 
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

	static std::vector<const char*> getNames(const std::vector<vk::LayerProperties>& layers) {
		std::vector<const char*> names(layers.size());

		for(size_t i = 0; i < names.size(); i++){
			names[i] = layers[i].layerName;
		}

		return names;
	}




	static std::vector<vk::ExtensionProperties> getUsedExtensions(	const std::vector<vk::ExtensionProperties>& available, 
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

	static std::vector<const char*> getNames(const std::vector<vk::ExtensionProperties>& ext) {
		std::vector<const char*> names(ext.size());

		for(size_t i = 0; i < names.size(); i++){
			names[i] = ext[i].extensionName;
		}

		return names;
	}

	static std::vector<vk::QueueFamilyProperties> getUsedQueueFamilies(	const std::vector<vk::QueueFamilyProperties>& available, 
																		std::vector<vk::QueueFamilyProperties>& required )
	{
		std::vector<vk::QueueFamilyProperties> queueFamilies;

		auto ite = required.begin();
		while(ite != required.end()){
			for(const auto& availableQf : available){
				if((ite->queueFlags & availableQf.queueFlags) == ite->queueFlags){
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

	static size_t getQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& qf, vk::QueueFlags flags) {
		size_t i;

		for(i = 0; i < qf.size(); i++){
			if((qf[i].queueFlags & flags) == flags && qf[i].queueCount > 0) {
				return i;
			}
		}

		if((flags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer) {
			//Graphics queue should be also OK
			return getQueueFamilyIndex(qf, (flags ^ vk::QueueFlagBits::eTransfer) | vk::QueueFlagBits::eGraphics);
		}

		//Failed to find a suitable queue family
		throw Exception("Requested queue family not found!");
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

Vulkan::Vulkan(Vulkan&& other) noexcept  = default;

Vulkan::~Vulkan() = default;

Vulkan& Vulkan::operator=(Vulkan&& other) noexcept  = default; 



const vk::DynamicLoader& Vulkan::getLoader() const noexcept {
	return m_impl->getLoader();
}

const vk::DispatchLoaderDynamic& Vulkan::getDispatcher() const noexcept {
	return m_impl->getDispatcher();
}

vk::Instance Vulkan::getInstance() const noexcept {
	return m_impl->getInstance();
}

vk::PhysicalDevice Vulkan::getPhysicalDevice() const noexcept {
	return m_impl->getPhysicalDevice();
}

vk::Device Vulkan::getDevice() const noexcept {
	return m_impl->getDevice();
}

uint32_t Vulkan::getGraphicsQueueIndex() const noexcept {
	return m_impl->getGraphicsQueueIndex();
}

vk::Queue Vulkan::getGraphicsQueue() const noexcept {
	return m_impl->getGraphicsQueue();
}

uint32_t Vulkan::getComputeQueueIndex() const noexcept {
	return m_impl->getComputeQueueIndex();
}

vk::Queue Vulkan::getComputeQueue() const noexcept {
	return m_impl->getComputeQueue();
}

uint32_t Vulkan::getTransferQueueIndex() const noexcept {
	return m_impl->getTransferQueueIndex();
}

vk::Queue Vulkan::getTransferQueue() const noexcept {
	return m_impl->getTransferQueue();
}

uint32_t Vulkan::getPresentationQueueIndex() const noexcept {
	return m_impl->getPresentationQueueIndex();
}

vk::Queue Vulkan::getPresentationQueue() const noexcept {
	return m_impl->getPresentationQueue();
}

vk::PipelineCache Vulkan::getPipelineCache() const noexcept {
	return m_impl->getPipelineCache();
}



const vk::PhysicalDeviceProperties&	Vulkan::getPhysicalDeviceProperties() const noexcept { 
	return m_impl->getPhysicalDeviceProperties();
}

const Vulkan::FormatSupport& Vulkan::getFormatSupport() const noexcept {
	return m_impl->getFormatSupport();
}

const Vulkan::DeviceFeatures& Vulkan::getDeviceFeatures() const noexcept {
	return m_impl->getDeviceFeatures();
}

const std::vector<vk::Format>& Vulkan::listSupportedFormatsOptimal(vk::FormatFeatureFlags flags) const {
	return m_impl->listSupportedFormatsOptimal(flags);
}

const std::vector<vk::Format>& Vulkan::listSupportedFormatsLinear(vk::FormatFeatureFlags flags) const {
	return m_impl->listSupportedFormatsLinear(flags);
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

vk::UniquePipeline Vulkan::createGraphicsPipeline(const vk::GraphicsPipelineCreateInfo& createInfo) const {
	return m_impl->createGraphicsPipeline(createInfo);
}

vk::Pipeline Vulkan::createGraphicsPipeline(size_t id) const {
	return m_impl->createGraphicsPipeline(id);
}

vk::Pipeline Vulkan::createGraphicsPipeline(size_t id,
											const vk::GraphicsPipelineCreateInfo& createInfo ) const
{
	return m_impl->createGraphicsPipeline(id, createInfo);
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

vk::UniqueSamplerYcbcrConversion Vulkan::createSamplerYcbcrConversion(const vk::SamplerYcbcrConversionCreateInfo& createInfo) const {
	return m_impl->createSamplerYcbcrConversion(createInfo);
}

vk::SamplerYcbcrConversion Vulkan::createSamplerYcbcrConversion(size_t id) const {
	return m_impl->createSamplerYcbcrConversion(id);
}

vk::SamplerYcbcrConversion Vulkan::createSamplerYcbcrConversion(size_t id,
																const vk::SamplerYcbcrConversionCreateInfo& createInfo ) const
{
	return m_impl->createSamplerYcbcrConversion(id, createInfo);
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

vk::UniqueCommandBuffer Vulkan::allocateCommnadBuffer(	vk::CommandPool commandPool, 
														vk::CommandBufferLevel level ) const
{
	return m_impl->allocateCommnadBuffer(commandPool, level);
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


std::vector<vk::UniqueDescriptorSet> Vulkan::allocateDescriptorSets(const vk::DescriptorSetAllocateInfo& allocInfo) const {
	return m_impl->allocateDescriptorSets(allocInfo);
}

vk::UniqueDescriptorSet Vulkan::allocateDescriptorSet(	vk::DescriptorPool pool, 
														vk::DescriptorSetLayout layout) const
{
	return m_impl->allocateDescriptorSet(pool, layout);
}


vk::MemoryRequirements Vulkan::getMemoryRequirements(vk::Buffer buf) const {
	return m_impl->getMemoryRequirements(buf);
}

vk::MemoryRequirements Vulkan::getMemoryRequirements(vk::Image img) const {
	return m_impl->getMemoryRequirements(img);
}

vk::MemoryRequirements2	Vulkan::getMemoryRequirements(const vk::BufferMemoryRequirementsInfo2& reqInfo) const {
	return m_impl->getMemoryRequirements(reqInfo);
}

vk::MemoryRequirements2	Vulkan::getMemoryRequirements(const vk::ImageMemoryRequirementsInfo2& reqInfo) const {
	return m_impl->getMemoryRequirements(reqInfo);
}



void Vulkan::bindMemory(vk::Buffer buf, vk::DeviceMemory mem, size_t off) const {
	m_impl->bindMemory(buf, mem, off);
}

void Vulkan::bindMemory(vk::Image img, vk::DeviceMemory mem, size_t off) const {
	m_impl->bindMemory(img, mem, off);
}

void Vulkan::bindMemory(Utils::BufferView<const vk::BindBufferMemoryInfo> bindInfos) const {
	m_impl->bindMemory(bindInfos);
}

void Vulkan::bindMemory(Utils::BufferView<const vk::BindImageMemoryInfo> bindInfos) const {
	m_impl->bindMemory(bindInfos);
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


void Vulkan::execute(	vk::CommandBuffer cmd,
						Utils::BufferView<const vk::CommandBuffer> buf ) const noexcept
{
	m_impl->execute(cmd, buf);
}


void Vulkan::pipelineBarrier(	vk::CommandBuffer cmd,
								vk::PipelineStageFlags srcStageMask,
								vk::PipelineStageFlags dstStageMask,
								vk::DependencyFlags dependencyFlags,
								Utils::BufferView<const vk::MemoryBarrier> memoryBarriers,
								Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers,
								Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers ) const noexcept
{
	m_impl->pipelineBarrier(
		cmd,
		srcStageMask, 
		dstStageMask, 
		dependencyFlags, 
		memoryBarriers,
		bufferMemoryBarriers,
		imageMemoryBarriers
	);
}

void Vulkan::pipelineBarrier(	vk::CommandBuffer cmd,
								vk::PipelineStageFlags srcStageMask,
								vk::PipelineStageFlags dstStageMask,
								vk::DependencyFlags dependencyFlags,
								Utils::BufferView<const vk::MemoryBarrier> memoryBarriers ) const noexcept
{
	m_impl->pipelineBarrier(cmd, srcStageMask, dstStageMask, dependencyFlags, memoryBarriers);
}

void Vulkan::pipelineBarrier(	vk::CommandBuffer cmd,
								vk::PipelineStageFlags srcStageMask,
								vk::PipelineStageFlags dstStageMask,
								vk::DependencyFlags dependencyFlags,
								Utils::BufferView<const vk::BufferMemoryBarrier> bufferMemoryBarriers ) const noexcept
{
	m_impl->pipelineBarrier(cmd, srcStageMask, dstStageMask, dependencyFlags, bufferMemoryBarriers);
}

void Vulkan::pipelineBarrier(	vk::CommandBuffer cmd,
								vk::PipelineStageFlags srcStageMask,
								vk::PipelineStageFlags dstStageMask,
								vk::DependencyFlags dependencyFlags,
								Utils::BufferView<const vk::ImageMemoryBarrier> imageMemoryBarriers ) const noexcept
{
	m_impl->pipelineBarrier(cmd, srcStageMask, dstStageMask, dependencyFlags, imageMemoryBarriers);
}


void Vulkan::clear(	vk::CommandBuffer cmd,
					vk::Image image,
					vk::ImageLayout imageLayout,
					const vk::ClearColorValue& value,
					Utils::BufferView<const vk::ImageSubresourceRange> ranges ) const noexcept
{
	m_impl->clear(cmd, image, imageLayout, value, ranges);
}

void Vulkan::clear(	vk::CommandBuffer cmd,
					vk::Image image,
					vk::ImageLayout imageLayout,
					const vk::ClearDepthStencilValue& value,
					Utils::BufferView<const vk::ImageSubresourceRange> ranges ) const noexcept
{
	m_impl->clear(cmd, image, imageLayout, value, ranges);
}

void Vulkan::clear(	vk::CommandBuffer cmd,
					Utils::BufferView<const vk::ClearAttachment> attachments,
					Utils::BufferView<const vk::ClearRect> rects ) const noexcept
{
	m_impl->clear(cmd, attachments, rects);
}

void Vulkan::clear(	vk::CommandBuffer cmd,
					vk::Buffer buffer,
					const Utils::Area& area,
					uint32_t data ) const noexcept
{
	m_impl->clear(cmd, buffer, area, data);
}

void Vulkan::clear(	vk::CommandBuffer cmd,
					vk::Buffer buffer,
					const Utils::Area& area,
					const std::byte* data ) const noexcept
{
	m_impl->clear(cmd, buffer, area, data);
}


void Vulkan::copy(	vk::CommandBuffer cmd,
					vk::Buffer srcBuffer,
					vk::Buffer dstBuffer,
					Utils::BufferView<const vk::BufferCopy> regions ) const noexcept
{
	m_impl->copy(cmd, srcBuffer, dstBuffer, regions);
}

void Vulkan::copy(	vk::CommandBuffer cmd,
					vk::Image srcImage,
					vk::ImageLayout srcImageLayout,
					vk::Image dstImage,
					vk::ImageLayout dstImageLayout,
					Utils::BufferView<const vk::ImageCopy> regions ) const noexcept
{
	m_impl->copy(cmd, srcImage, srcImageLayout, dstImage, dstImageLayout, regions);
}

void Vulkan::copy(	vk::CommandBuffer cmd,
					vk::Buffer srcBuffer,
					vk::Image dstImage,
					vk::ImageLayout dstImageLayout,
					Utils::BufferView<const vk::BufferImageCopy> regions ) const noexcept
{
	m_impl->copy(cmd, srcBuffer, dstImage, dstImageLayout, regions);
}

void Vulkan::copy(	vk::CommandBuffer cmd,
					vk::Image srcImage,
					vk::ImageLayout srcImageLayout,
					vk::Buffer dstBuffer,
					Utils::BufferView<const vk::BufferImageCopy> regions ) const noexcept
{
	m_impl->copy(cmd, srcImage, srcImageLayout, dstBuffer, regions);
}

void Vulkan::blit(	vk::CommandBuffer cmd,
					vk::Image srcImage,
					vk::ImageLayout srcImageLayout,
					vk::Image dstImage,
					vk::ImageLayout dstImageLayout,
					Utils::BufferView<const vk::ImageBlit> regions,
					vk::Filter filter ) const noexcept
{
	m_impl->blit(cmd, srcImage, srcImageLayout, dstImage, dstImageLayout, regions, filter);
}

void Vulkan::resolve(	vk::CommandBuffer cmd,
						vk::Image srcImage,
						vk::ImageLayout srcImageLayout,
						vk::Image dstImage,
						vk::ImageLayout dstImageLayout,
						Utils::BufferView<const vk::ImageResolve> regions ) const noexcept
{
	m_impl->resolve(cmd, srcImage, srcImageLayout, dstImage, dstImageLayout, regions);
}


void Vulkan::beginRenderPass(vk::CommandBuffer cmd, 
							const vk::RenderPassBeginInfo& beginInfo, 
							vk::SubpassContents contents ) const noexcept
{
	m_impl->beginRenderPass(cmd, beginInfo, contents);
}

void Vulkan::endRenderPass(vk::CommandBuffer cmd) const noexcept {
	m_impl->endRenderPass(cmd);
}

void Vulkan::nextSubpass(	vk::CommandBuffer cmd,
							vk::SubpassContents contents ) const noexcept
{
	m_impl->nextSubpass(cmd, contents);
}

void Vulkan::bindPipeline(	vk::CommandBuffer cmd, 
							vk::PipelineBindPoint bindPoint, 
							vk::Pipeline pipeline ) const noexcept
{
	m_impl->bindPipeline(cmd, bindPoint, pipeline);
}

void Vulkan::bindVertexBuffers(	vk::CommandBuffer cmd, 
								uint32_t firstBinding, 
								Utils::BufferView<const vk::Buffer> buffers, 
								Utils::BufferView<const vk::DeviceSize> offsets ) const noexcept
{
	m_impl->bindVertexBuffers(cmd, firstBinding, buffers, offsets);
}

void Vulkan::bindDescriptorSets(vk::CommandBuffer cmd, 
								vk::PipelineBindPoint pipelineBindPoint, 
								vk::PipelineLayout layout, 
								uint32_t firstSet, 
								Utils::BufferView<const vk::DescriptorSet> descriptorSets, 
								Utils::BufferView<const uint32_t> dynamicOffsets) const noexcept
{
	m_impl->bindDescriptorSets(cmd, pipelineBindPoint, layout, firstSet, descriptorSets, dynamicOffsets);
}

void Vulkan::setViewport(	vk::CommandBuffer cmd,
							uint32_t first,
							Utils::BufferView<const vk::Viewport> viewports ) const noexcept
{
	m_impl->setViewport(cmd, first, viewports);
}

void Vulkan::setScissor(vk::CommandBuffer cmd,
						uint32_t first,
						Utils::BufferView<const vk::Rect2D> scissors ) const noexcept
{
	m_impl->setScissor(cmd, first, scissors);
}

void Vulkan::draw(	vk::CommandBuffer cmd, 
					uint32_t vertexCount, 
					uint32_t instanceCount, 
					uint32_t firstVertex, 
					uint32_t firstInstance ) const noexcept
{
	m_impl->draw(cmd, vertexCount, instanceCount, firstVertex, firstInstance);
}

void Vulkan::draw(	vk::CommandBuffer cmd, 
					vk::Buffer buffer,
					size_t offset,
					uint32_t drawCount,
					uint32_t stride ) const noexcept
{
	m_impl->draw(cmd, buffer, offset, drawCount, stride);
}

void Vulkan::drawIndexed(	vk::CommandBuffer cmd, 
							uint32_t indexCount,
							uint32_t instanceCount,
							uint32_t firstIndex,
							int32_t vertexOffset,
							uint32_t firstInstance ) const noexcept
{
	m_impl->drawIndexed(cmd, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void Vulkan::drawIndexed(	vk::CommandBuffer cmd,
							vk::Buffer buffer,
							size_t offset,
							uint32_t drawCount,
							uint32_t stride ) const noexcept
{
	m_impl->drawIndexed(cmd, buffer, offset, drawCount, stride);
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



void Vulkan::setUserPointer(size_t id, std::shared_ptr<void> usrPtr) {
	m_impl->setUserPointer(id, std::move(usrPtr));
}

const std::shared_ptr<void>& Vulkan::getUserPointer(size_t id) const noexcept {
	return m_impl->getUserPointer(id);
}

}
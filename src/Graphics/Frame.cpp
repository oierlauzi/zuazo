#include <Graphics/Frame.h>

#include <Exception.h>
#include <Utils/StaticId.h>
#include <Graphics/ColorTransfer.h>

namespace Zuazo::Graphics {

Frame::Frame(	const Vulkan& vulkan,
				Math::Vec2f size,
				Utils::BufferView<const Image::PlaneDescriptor> planes,
				const std::shared_ptr<const Buffer>& colorTransfer )
	: m_vulkan(vulkan)
	, m_size(size)
	, m_image(createImage(m_vulkan, planes))
	, m_colorTransfer(colorTransfer)
	, m_descriptorPool(createDescriptorPool(m_vulkan))
	, m_descriptorSets(allocateDescriptorSets(m_vulkan, *m_descriptorPool))
	, m_readyFence(m_vulkan.createFence(true))
{
	//Update the descriptors
	for(size_t i = 0; i < m_descriptorSets.size(); i++){
		//Update all images. As nullptr images can't be passed, repeat available images		
		const auto& imageViews = m_image.getImageViews();
		std::array<vk::DescriptorImageInfo, MAX_PLANE_COUNT> images;
		for(size_t j = 0; j < images.size(); j++){
			images[j] = vk::DescriptorImageInfo(
				nullptr,												//Sampler
				*(imageViews[j % imageViews.size()]),					//Image views
				vk::ImageLayout::eShaderReadOnlyOptimal					//Layout
			);
		}

		const std::array buffers = {
			vk::DescriptorBufferInfo(
				m_colorTransfer->getBuffer(),							//Buffer
				0,														//Offset
				sizeof(ColorTransfer)									//Size
			)
		};

		const std::array writeDescriptorSets ={
			vk::WriteDescriptorSet( //Image descriptor
				m_descriptorSets[i],									//Descriptor set
				ColorTransfer::getSamplerBinding(),						//Binding
				0, 														//Index
				images.size(), 											//Descriptor count
				vk::DescriptorType::eCombinedImageSampler,				//Descriptor type
				images.data(), 											//Images
				nullptr, 												//Buffers
				nullptr													//Texel buffers
			),
			vk::WriteDescriptorSet( //Ubo descriptor set
				m_descriptorSets[i],									//Descriptor set
				ColorTransfer::getDataBinding(),						//Binding
				0, 														//Index
				buffers.size(),											//Descriptor count		
				vk::DescriptorType::eUniformBuffer,						//Descriptor type
				nullptr, 												//Images 
				buffers.data(), 										//Buffers
				nullptr													//Texel buffers
			)
		};

		m_vulkan.getDevice().updateDescriptorSets(writeDescriptorSets, {}, m_vulkan.getDispatcher());
	}

}


void Frame::bind( 	vk::CommandBuffer cmd,
					vk::PipelineLayout layout,
					uint32_t index,
					vk::Filter filter ) 
{
	const std::array descriptorSets = {
		m_descriptorSets[static_cast<size_t>(filter)]
	};

	cmd.bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics,	//Pipeline bind point
		layout,								//Pipeline layout
		index,								//First index
		descriptorSets,						//Descriptor sets
		{},									//Dynamic offsets
		m_vulkan.getDispatcher()
	);
}


const Math::Vec2f& Frame::getSize() const {
	return m_size;
}

const Image& Frame::getImage() const {
	return m_image;
}

const Buffer& Frame::getColorTransfer() const {
	return *m_colorTransfer;
}

const vk::Fence& Frame::getReadyFence() const {
	return *m_readyFence;
}



Math::Vec2f Frame::getFrameSize(Resolution resolution, AspectRatio par) {
	return Math::Vec2f(
		static_cast<float>(par) * resolution.x,
		static_cast<float>(resolution.y)
	);
}

std::shared_ptr<Buffer> Frame::createColorTransferBuffer( 	const Vulkan& vulkan,
															const ColorTransfer& colorTransfer )
{
	//Create the buffer
	constexpr vk::BufferUsageFlags usageFlags =
		vk::BufferUsageFlagBits::eUniformBuffer |
		vk::BufferUsageFlagBits::eTransferDst;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eDeviceLocal;

	Buffer result(
		vulkan,
		usageFlags,
		memoryFlags,
		ColorTransfer::size()
	);
 
	//Create a staging buffer. Using this technique due to preferably having
	//device memory allocated for the UBO as it will be only written once and
	//read frequently
	constexpr vk::BufferUsageFlags stagingUsageFlags =
		vk::BufferUsageFlagBits::eTransferSrc;

	constexpr vk::MemoryPropertyFlags stagingMemoryFlags =
		vk::MemoryPropertyFlagBits::eHostVisible;

	Buffer stagingBuffer(
		vulkan,
		stagingUsageFlags,
		stagingMemoryFlags,
		ColorTransfer::size()
	);

	//Upload the contents to the staging buffer
	const vk::MappedMemoryRange range(
		stagingBuffer.getDeviceMemory(),
		0, VK_WHOLE_SIZE
	);

	auto* stagingBufferData = vulkan.mapMemory(range);
	std::memcpy(
		stagingBufferData,
		colorTransfer.data(),
		ColorTransfer::size()
	);

	//Create a command pool and a command buffer for uploading it
	const vk::CommandPoolCreateInfo cpCreateInfo(
		{},													//Flags
		vulkan.getTransferQueueIndex()						//Queue index
	);

	const auto commandPool = vulkan.createCommandPool(cpCreateInfo);

	const vk::CommandBufferAllocateInfo cbAllocInfo(
		*commandPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);

	auto cmdBuffers = vulkan.allocateCommnadBuffers(cbAllocInfo);
	auto& cmdBuffer = *(cmdBuffers[0]);

	//Record the command buffer
	constexpr vk::CommandBufferUsageFlags cbUsage =
		vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	const vk::CommandBufferBeginInfo cbBeginInfo(
		cbUsage,
		nullptr
	);

	cmdBuffer.begin(cbBeginInfo, vulkan.getDispatcher()); 
	{
		cmdBuffer.copyBuffer(
			stagingBuffer.getBuffer(),
			result.getBuffer(),
			vk::BufferCopy(0, 0, ColorTransfer::size()),
			vulkan.getDispatcher()
		);

		//Insert a memory barrier
		const bool queueOwnershipTransfer = vulkan.getTransferQueueIndex() != vulkan.getGraphicsQueueIndex();
		const auto srcFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
		const auto dstFamily = queueOwnershipTransfer ? vulkan.getGraphicsQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
		constexpr vk::AccessFlags srcAccess = {};
		constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eShaderRead;

		const vk::BufferMemoryBarrier memoryBarrier(
			srcAccess,						//Old access mask
			dstAccess,						//New access mask
			srcFamily,						//Old queue family
			dstFamily, 						//New queue family
			result.getBuffer(),				//Buffer
			0, VK_WHOLE_SIZE				//Range
		);

		constexpr vk::PipelineStageFlags sourceStages = 
			vk::PipelineStageFlagBits::eTransfer;

		constexpr vk::PipelineStageFlags destinationStages = 
			vk::PipelineStageFlagBits::eAllGraphics;
		
		cmdBuffer.pipelineBarrier(
			sourceStages,					//Generating stages
			destinationStages,				//Consuming stages
			{},								//dependency flags
			{},								//Memory barriers
			memoryBarrier,					//Buffer memory barriers
			{},								//Image memory barriers
			vulkan.getDispatcher()
		);
	}
	cmdBuffer.end(vulkan.getDispatcher());

	//Submit the command buffer
	const auto uploadFence = vulkan.createFence();
	const vk::SubmitInfo submitInfo(
		0, nullptr,							//Wait semaphores
		nullptr,							//Pipeline stages
		1, &cmdBuffer,						//Command buffers
		0, nullptr							//Signal semaphores
	);

	vulkan.getTransferQueue().submit(submitInfo, *uploadFence, vulkan.getDispatcher());

	//Wait for completion
	vulkan.getDevice().waitForFences(
		*uploadFence, 
		true, 
		std::numeric_limits<uint64_t>::max(),
		vulkan.getDispatcher()
	);

	return std::make_shared<Buffer>(std::move(result));
}

vk::DescriptorSetLayout	Frame::getDescriptorSetLayout(	const Vulkan& vulkan,
														vk::Filter filter) {
	static const std::array<Utils::StaticId, VK_FILTER_RANGE_SIZE> ids;
	const size_t id = ids[static_cast<size_t>(filter)];

	auto result = vulkan.createDescriptorSetLayout(id);

	if(!result) {
		//Descriptor set was not previously created. Create it
		auto sampler = vulkan.createSampler(id);
		if(!sampler) {
			//Sampler does not exist, create it
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

			sampler = vulkan.createSampler(id, createInfo);
		}

		//Create the sampler array
		std::vector<vk::Sampler> inmutableSamplers(ColorTransfer::getSamplerCount(), sampler);

		//Create the bindings
		const std::array bindings = {
			vk::DescriptorSetLayoutBinding( //Sampled image binding
				ColorTransfer::getSamplerBinding(),				//Binding
				vk::DescriptorType::eCombinedImageSampler,		//Type
				inmutableSamplers.size(),						//Count
				vk::ShaderStageFlagBits::eAllGraphics,			//Shader stage
				inmutableSamplers.data()						//Inmutable samplers
			), 
			vk::DescriptorSetLayoutBinding(	//Color transfer binding
				ColorTransfer::getDataBinding(),				//Binding
				vk::DescriptorType::eUniformBuffer,				//Type
				1,												//Count
				vk::ShaderStageFlagBits::eAllGraphics,			//Shader stage
				nullptr											//Inmutable samplers
			), 
		};

		const vk::DescriptorSetLayoutCreateInfo createInfo(
			{},
			bindings.size(), bindings.data()
		);

		result = vulkan.createDescriptorSetLayout(id, createInfo);
	}

	return result;
}



Image Frame::createImage(	const Vulkan& vulkan,
							Utils::BufferView<const Image::PlaneDescriptor> planes )
{
	constexpr vk::ImageUsageFlags usageFlags =
		vk::ImageUsageFlagBits::eSampled |
		vk::ImageUsageFlagBits::eTransferDst;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eDeviceLocal;

	return Image(
		vulkan,
		usageFlags,
		memoryFlags,
		planes
	);
}

vk::UniqueDescriptorPool Frame::createDescriptorPool(const Vulkan& vulkan){
	//A descriptor pool is created, from which 2 descriptor sets, each one with 
	//one combined image sampler and one uniform buffer. Threrefore, 2 descriptors
	//of each type will be required.
	const std::array poolSizes = {
		vk::DescriptorPoolSize(
			vk::DescriptorType::eCombinedImageSampler,			//Descriptor type
			DESCRIPTOR_COUNT * ColorTransfer::getSamplerCount()	//Descriptor count
		),
		vk::DescriptorPoolSize(
			vk::DescriptorType::eUniformBuffer,					//Descriptor type
			DESCRIPTOR_COUNT									//Descriptor count
		)
	};

	const vk::DescriptorPoolCreateInfo createInfo(
		{},														//Flags
		DESCRIPTOR_COUNT,										//Descriptor set count
		poolSizes.size(), poolSizes.data()						//Pool sizes
	);

	return vulkan.createDescriptorPool(createInfo);
}

Frame::DescriptorSets Frame::allocateDescriptorSets(const Vulkan& vulkan,
													vk::DescriptorPool pool )
{
	std::array<vk::DescriptorSetLayout, DESCRIPTOR_COUNT> layouts;
	for(size_t i = 0; i < layouts.size(); i++){
		const auto filter = static_cast<vk::Filter>(i);
		layouts[i] = getDescriptorSetLayout(vulkan, filter);
	}

	const vk::DescriptorSetAllocateInfo allocInfo(
		pool,													//Pool
		layouts.size(), layouts.data()							//Layouts
	);

	DescriptorSets descriptorSets;
	static_assert(descriptorSets.size() == layouts.size());
	const auto result = vulkan.getDevice().allocateDescriptorSets(&allocInfo, descriptorSets.data(), vulkan.getDispatcher());

	if(result != vk::Result::eSuccess){
		throw Exception("Error allocating descriptor sets");
	}

	return descriptorSets;
}



/*
 * Frame::Geometry
 */

Frame::Geometry::Geometry(	const Vulkan& vulkan, 
							uint32_t binding, 
							Math::Vec2f  targetSize )
	: m_vulkan(vulkan)
	, m_binding(binding)
	, m_targetSize(targetSize)
	, m_vertexBuffer(createVertexBuffer(m_vulkan))
	, m_stagingBuffer(createStagingBuffer(m_vulkan))
	, m_vertices(mapStagingBuffer(m_vulkan, m_stagingBuffer))
	, m_commandPool(createCommandPool(m_vulkan))
	, m_uploadCommand(createCommandBuffer(
		m_vulkan,
		*m_commandPool,
		m_stagingBuffer,
		m_vertexBuffer ))
	, m_uploadCommandSubmit(createSubmitInfo(m_uploadCommand))
{
}


void Frame::Geometry::setTargetSize(Math::Vec2f size) {
	if(size != m_targetSize) {
		m_targetSize = size;
		m_sourceSize = Math::Vec2f(0.0f); //So that it gets recalculated
	}
}
const Math::Vec2f& Frame::Geometry::getTargetSize() const {
	return m_targetSize;
}


void Frame::Geometry::setSourceSize(Math::Vec2f size) {
	if(size != m_sourceSize) {
		m_sourceSize = size;
		updateVertexBuffer(); //Recalculate the vertices
	}
}

const Math::Vec2f& Frame::Geometry::getSourceSize() const {
	return m_sourceSize;
}


vk::VertexInputBindingDescription Frame::Geometry::getBindingDescription() const{
	return vk::VertexInputBindingDescription(
		m_binding,
		sizeof(Vertex),
		vk::VertexInputRate::eVertex
	);
}

std::array<vk::VertexInputAttributeDescription, Frame::Geometry::ATTRIBUTE_COUNT> Frame::Geometry::getAttributeDescriptions(uint32_t posLocation, 
																															uint32_t texLocation ) const
{
	return{
		vk::VertexInputAttributeDescription(
			posLocation,
			m_binding,
			vk::Format::eR32G32Sfloat,
			offsetof(Vertex, position)
		),
		vk::VertexInputAttributeDescription(
			texLocation,
			m_binding,
			vk::Format::eR32G32Sfloat,
			offsetof(Vertex, texCoord)
		)
	};
}


void Frame::Geometry::useFrame(const Frame& frame) {
	setSourceSize(frame.getSize());
}

void Frame::Geometry::bind(vk::CommandBuffer cmd) const {
	cmd.bindVertexBuffers(
		m_binding,									//Binding
		m_vertexBuffer.getBuffer(),					//Vertex buffers
		0UL,										//Offsets
		m_vulkan.getDispatcher()					//Dispathcer
	);
}



void Frame::Geometry::updateVertexBuffer() {
	//auto scale = static_cast<Math::Vec2f>(m_dstResolution) / static_cast<Math::Vec2f>(m_srcResolution); 

	//TODO modify scale to fit

	/*auto resolution = static_cast<Math::Vec2f>(m_srcResolution) * scale;
	auto texSize = static_cast<Math::Vec2f>(m_dstResolution) / resolution;
	resolution = glm::max(resolution, static_cast<Math::Vec2f>(m_dstResolution));*/
	auto resolution = Math::Vec2f(1.0);
	auto texSize = Math::Vec2f(1.0);

	constexpr std::array<glm::vec2, VERTEX_COUNT> vertexPositions = {
		glm::vec2(-0.5f, -0.5f),
		glm::vec2(-0.5f, +0.5f),
		glm::vec2(+0.5f, -0.5f),
		glm::vec2(+0.5f, +0.5f),
	};

	for(size_t i = 0; i < vertexPositions.size(); i++){
		m_vertices[i] = Vertex{
			resolution * vertexPositions[i],
			texSize * vertexPositions[i] + glm::vec2(0.5f)
		};
	}

	//Flush it
	const vk::MappedMemoryRange range(
		m_stagingBuffer.getDeviceMemory(),
		0, VK_WHOLE_SIZE
	);
	m_vulkan.flushMappedMemory(range);
	m_vulkan.getTransferQueue().submit(
		m_uploadCommandSubmit,
		nullptr,
		m_vulkan.getDispatcher()
	);
}



Buffer Frame::Geometry::createVertexBuffer(const Vulkan& vulkan) {
	constexpr vk::BufferUsageFlags usageFlags =
		vk::BufferUsageFlagBits::eTransferDst |
		vk::BufferUsageFlagBits::eVertexBuffer;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eDeviceLocal;

	return Buffer(
		vulkan,
		usageFlags,
		memoryFlags,
		BUFFER_SIZE
	);
}

Buffer Frame::Geometry::createStagingBuffer(const Vulkan& vulkan) {
	constexpr vk::BufferUsageFlags usageFlags =
		vk::BufferUsageFlagBits::eTransferSrc;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eHostVisible;

	return Buffer(
		vulkan,
		usageFlags,
		memoryFlags,
		BUFFER_SIZE
	);
}

Utils::BufferView<Frame::Geometry::Vertex> Frame::Geometry::mapStagingBuffer(	const Vulkan& vulkan, 
																				const Buffer& stagingBuffer )
{
	//Map its memory
	const vk::MappedMemoryRange range(
		stagingBuffer.getDeviceMemory(),
		0, VK_WHOLE_SIZE
	);
	auto* data = vulkan.mapMemory(range);

	return Utils::BufferView<Vertex>( 
		reinterpret_cast<Vertex*>(data), 
		VERTEX_COUNT 
	);
}

vk::UniqueCommandPool Frame::Geometry::createCommandPool(const Vulkan& vulkan) {
	const vk::CommandPoolCreateInfo createInfo(
		{},													//Flags
		vulkan.getTransferQueueIndex()						//Queue index
	);

	return vulkan.createCommandPool(createInfo);
}

vk::CommandBuffer Frame::Geometry::createCommandBuffer(	const Vulkan& vulkan,
														vk::CommandPool cmdPool,
														const Buffer& stagingBuffer,
														const Buffer& vertexBuffer )
{
	const vk::CommandBufferAllocateInfo cbAllocInfo(
		cmdPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);

	auto cmdBuffers = vulkan.allocateCommnadBuffers(cbAllocInfo);
	auto& cmdBuffer = *(cmdBuffers[0]);

	//Record the command buffer
	const vk::CommandBufferBeginInfo cbBeginInfo(
		{},
		nullptr
	);

	cmdBuffer.begin(cbBeginInfo, vulkan.getDispatcher()); 
	{
		const bool queueOwnershipTransfer = vulkan.getTransferQueueIndex() != vulkan.getGraphicsQueueIndex();

		{
			//Insert a memory barrier to acquire buffer's ownership
			const auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
			const auto dstFamily = queueOwnershipTransfer ? vulkan.getGraphicsQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
			constexpr vk::AccessFlags srcAccess = {};
			constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferWrite;

			const vk::BufferMemoryBarrier memoryBarrier(
				srcAccess,						//Old access mask
				dstAccess,						//New access mask
				srcFamily,						//Old queue family
				dstFamily, 						//New queue family
				vertexBuffer.getBuffer(),		//Buffer
				0, VK_WHOLE_SIZE				//Range
			);

			constexpr vk::PipelineStageFlags sourceStages = 
				vk::PipelineStageFlagBits::eTopOfPipe;

			constexpr vk::PipelineStageFlags destinationStages = 
				vk::PipelineStageFlagBits::eTransfer;
			
			cmdBuffer.pipelineBarrier(
				sourceStages,					//Generating stages
				destinationStages,				//Consuming stages
				{},								//dependency flags
				{},								//Memory barriers
				memoryBarrier,					//Buffer memory barriers
				{},								//Image memory barriers
				vulkan.getDispatcher()
			);
		}

		//Copy the data into the vertex buffer
		cmdBuffer.copyBuffer(
			stagingBuffer.getBuffer(),
			vertexBuffer.getBuffer(),
			vk::BufferCopy(0, 0, BUFFER_SIZE),
			vulkan.getDispatcher()
		);

		//Insert a memory barrier, so that changes are visible
		{
			const auto srcFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
			const auto dstFamily = queueOwnershipTransfer ? vulkan.getGraphicsQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
			constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferWrite;
			constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eVertexAttributeRead;

			const vk::BufferMemoryBarrier memoryBarrier(
				srcAccess,						//Old access mask
				dstAccess,						//New access mask
				srcFamily,						//Old queue family
				dstFamily, 						//New queue family
				vertexBuffer.getBuffer(),		//Buffer
				0, VK_WHOLE_SIZE				//Range
			);

			constexpr vk::PipelineStageFlags sourceStages = 
				vk::PipelineStageFlagBits::eTransfer;

			constexpr vk::PipelineStageFlags destinationStages = 
				vk::PipelineStageFlagBits::eAllGraphics;
			
			cmdBuffer.pipelineBarrier(
				sourceStages,					//Generating stages
				destinationStages,				//Consuming stages
				{},								//dependency flags
				{},								//Memory barriers
				memoryBarrier,					//Buffer memory barriers
				{},								//Image memory barriers
				vulkan.getDispatcher()
			);
		}
	}
	cmdBuffer.end(vulkan.getDispatcher());

	return cmdBuffers[0].release();
}

vk::SubmitInfo Frame::Geometry::createSubmitInfo(const vk::CommandBuffer& cmdBuffer){
	return vk::SubmitInfo(
		0, nullptr,							//Wait semaphores
		nullptr,							//Pipeline stages
		1, &cmdBuffer,						//Command buffers
		0, nullptr							//Signal semaphores
	);
}

}
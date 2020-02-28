#include <Graphics/Uploader.h>

#include <Graphics/VulkanConversions.h>
#include <Exception.h>

namespace Zuazo::Graphics {

Uploader::Uploader(	const Vulkan& vulkan,
					const Descriptor& descriptor )
	: m_vulkan(vulkan)
	, m_descriptor(descriptor)
	, m_commandPool(createCommandPool(m_vulkan))
	, m_colorTransferBuffer(createColorTransferBuffer(m_vulkan, **m_commandPool, m_descriptor.colorTransfer))
{
}

std::tuple<const std::shared_ptr<Frame>&, const Uploader::PixelData&> Uploader::getFrame() const {
	const auto& frame = acquireFrame();
	const auto& data = static_cast<FrameData&>(frame->getData());

	//Wait for completion of previous uplaods
	m_vulkan.getDevice().waitForFences(
		frame->getReadyFence(),							//Fence
		true,											//Wait all
		std::numeric_limits<uint64_t>::max(),			//Timeout
		m_vulkan.getDispatcher()						//Dispatcher
	);

	return { frame, data.pixelData };
}


void Uploader::flush(const std::shared_ptr<Frame>& frame) {
	auto& data = static_cast<FrameData&>(frame->getData());
	data.stagingBufferMemory.flush();
}


const std::shared_ptr<Frame>& Uploader::acquireFrame() const {
	//FIXME delete vacant frames

	//Try to find a unused frame.
	//If it is only held by me it is considered to be unused
	for(const auto& frame : m_frames){
		if(frame.unique()){
			return frame;
		}
	}

	//No unused frames were found. Create a new one and return it
	m_frames.emplace_back(createFrame());
	return m_frames.back();
}



Uploader::Descriptor Uploader::getDescriptor(	const Vulkan& vulkan,
												Resolution resolution,
												ColorSubsampling subsampling,
												ColorFormat format,
												ColorRange range,
												ColorTransferFunction transferFunction,
												ColorModel model,
												ColorPrimaries primaries )
{
	Descriptor result;

	//Get the plane count
	const auto planeCount = getPlaneCount(format);

	//Obtain the resolutions of each plane
	result.extents[0] = toVulkan(resolution);
	if(planeCount > 1 && planeCount <= 3) {
		const auto chromaSize = toVulkan(getChromaResolution(subsampling, resolution));

		for(size_t i = 1; i < planeCount; i++){
			result.extents[i] = chromaSize;
		}
	} //else 4 planar formats to be implemented

	//Obtain the format
	result.colorFormat = toVulkan(format);
	const auto& supportedFormats = vulkan.getFormatSupport();
	for(size_t i = 0; i < planeCount; i++){
		if(	std::find(
				supportedFormats.sampler.cbegin(), 
				supportedFormats.sampler.cend(), 
				std::get<vk::Format>(result.colorFormat[i]) )
			== supportedFormats.sampler.cend())
		{
			throw Exception("Unsupported format!");
		}
	}
	switch(planeCount){
	case 1:
		result.colorTransfer.planeFormat = PLANE_FORMAT_RGBA;	
		break;
	case 2:
		result.colorTransfer.planeFormat = PLANE_FORMAT_G_BR;
		break;
	case 3:
		result.colorTransfer.planeFormat = PLANE_FORMAT_G_B_R;
		break;
	default:
		throw Exception("Unsupported plane count!");
	}

	//Decide the range
	result.colorTransfer.colorRange = toVulkan(range, model);
	if(result.colorTransfer.colorRange < 0){
		throw Exception("Unsupported color range!");
	}

	//Decide the transfer function
	result.colorTransfer.colorTransferFunction = toVulkan(transferFunction);
	if(result.colorTransfer.colorTransferFunction < 0){
		throw Exception("Unsupported color transfer function!");
	}

	//Obtain the color model
	result.colorTransfer.colorModel = getConversionMatrix(model);
	if(result.colorTransfer.colorModel == Math::Mat4x4f()){
		throw Exception("Unsupported color model!");
	}

	//Obtain the color primaries
	result.colorTransfer.colorPrimaries = getConversionMatrix(primaries);
	if(result.colorTransfer.colorPrimaries == Math::Mat4x4f()){
		throw Exception("Unsupported color primaries!");
	}

	//Try to optimize the format
	for(size_t i = 0; i < planeCount; i++){
		const auto optimizedFormat = optimizeFormat(result.colorFormat[i]);
		if(	std::find(
				supportedFormats.sampler.cbegin(), 
				supportedFormats.sampler.cend(), 
				std::get<vk::Format>(optimizedFormat) )
			!= supportedFormats.sampler.cend())
		{
			result.colorFormat[i] = optimizedFormat;
		}
	}

	//Try to optimize the transfer function
	if(planeCount == 1){
		const auto optimizedFormat = optimizeFormat(
			std::get<vk::Format>(result.colorFormat[0]), 
			result.colorTransfer.colorTransferFunction,
			result.colorTransfer.colorRange
		);

		if(	std::find(
				supportedFormats.sampler.cbegin(), 
				supportedFormats.sampler.cend(), 
				std::get<vk::Format>(optimizedFormat) )
			!= supportedFormats.sampler.cend())
		{
			std::get<vk::Format>(result.colorFormat[0]) = std::get<vk::Format>(optimizedFormat);
			result.colorTransfer.colorTransferFunction = std::get<int32_t>(optimizedFormat);
		}
	}

	return result;
}



std::shared_ptr<Frame> Uploader::createFrame() const {
	return std::make_shared<Frame>(
		m_vulkan,
		createImage(),
		std::shared_ptr(m_colorTransferBuffer),
		createFrameData()
	);
	return {};
}

Image Uploader::createImage() const {
	constexpr vk::ImageUsageFlags usageFlags =
		vk::ImageUsageFlagBits::eSampled |
		vk::ImageUsageFlagBits::eTransferDst;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eDeviceLocal;

	//Get the plane descriptor array
	std::vector<std::tuple<vk::Extent2D, vk::Format, vk::ComponentMapping>> planeDescriptors;
	for(size_t i = 0; i < m_descriptor.colorFormat.size(); i++){
		if(std::get<vk::Format>(m_descriptor.colorFormat[i]) != vk::Format::eUndefined){
			planeDescriptors.emplace_back(
				m_descriptor.extents[i],
				std::get<vk::Format>(m_descriptor.colorFormat[i]),
				std::get<vk::ComponentMapping>(m_descriptor.colorFormat[i])
			);
		} else {
			break;
		}
	}

	return Image(
		m_vulkan,
		usageFlags,
		memoryFlags,
		planeDescriptors
	);
	return {};
}

std::unique_ptr<Frame::Data> Uploader::createFrameData() const {
	/*const vk::SubmitInfo submitInfo(
		0, nullptr,							//Wait semaphores
		nullptr,							//Pipeline stages
		1, &(*data.commandBuffer),			//Command buffers
		1, &(frame->getReadySemaphore())	//Signal semaphores
	);*/
	//TODO
	return {};
}




std::shared_ptr<vk::UniqueCommandPool> Uploader::createCommandPool(const Vulkan& vulkan){
	const vk::CommandPoolCreateInfo createInfo(
		{},													//Flags
		vulkan.getTransferQueueIndex()						//Queue index
	);

	return std::make_shared<vk::UniqueCommandPool>(vulkan.createCommandPool(createInfo));
}

std::shared_ptr<const Buffer> Uploader::createColorTransferBuffer(	const Vulkan& vulkan,
																	vk::CommandPool& commandPool,
																	const ColorTransfer& colorTransfer )
{
	//Create the buffer
	constexpr vk::BufferUsageFlags usageFlags =
		vk::BufferUsageFlagBits::eUniformBuffer |
		vk::BufferUsageFlagBits::eTransferDst;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eDeviceLocal;

	constexpr size_t size = sizeof(ColorTransfer);

	Buffer result(
		vulkan,
		usageFlags,
		memoryFlags,
		size
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
		size
	);

	//Upload the contents to the staging buffer
	{
		MappedMemory mapping(
			vulkan,
			vk::MappedMemoryRange(
				stagingBuffer.getDeviceMemory(),
				0, VK_WHOLE_SIZE
			)
		);

		std::memcpy(
			mapping.data(),
			&colorTransfer,
			size
		);

		mapping.flush();
	}

	//Allocate a command buffer for uploading it
	const vk::CommandBufferAllocateInfo cbAllocInfo(
		commandPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);

	vk::CommandBuffer cmdBuffer;
	vulkan.getDevice().allocateCommandBuffers(
		&cbAllocInfo, 
		&cmdBuffer, 
		vulkan.getDispatcher()
	);

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
			vk::BufferCopy(0, 0, size),
			vulkan.getDispatcher()
		);

		if(vulkan.getTransferQueueIndex() != vulkan.getGraphicsQueueIndex()) {
			//Change the ownership to the graphics queue
			const vk::BufferMemoryBarrier memoryBarrier(
				{},								//Old access mask
				{},								//New access mask
				vulkan.getTransferQueueIndex(),	//Old queue family
				vulkan.getGraphicsQueueIndex(), //New queue family
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

	//Wait for completion and reset the command pool, as we've allocated a command buffer
	vulkan.getDevice().waitForFences(
		*uploadFence, 
		true, 
		std::numeric_limits<uint64_t>::max(),
		vulkan.getDispatcher()
	);
	vulkan.getDevice().resetCommandPool(commandPool, {}, vulkan.getDispatcher());

	return std::make_shared<const Buffer>(std::move(result));
}

}
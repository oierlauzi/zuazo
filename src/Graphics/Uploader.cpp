#include <Graphics/Uploader.h>

#include <Graphics/VulkanConversions.h>
#include <Exception.h>

namespace Zuazo::Graphics {

Uploader::Uploader(	const Vulkan& vulkan,
					const Descriptor& descriptor )
	: m_vulkan(vulkan)
	, m_descriptor(descriptor)
	, m_commandPool(createCommandPool(m_vulkan))
	, m_colorTransferBuffer(Graphics::Frame::createColorTransferBuffer(m_vulkan, m_descriptor.colorTransfer))
{
}


const std::shared_ptr<Uploader::Frame>& Uploader::acquireFrame() const {
	const auto& frame = getUniqueFrame();

	//Wait for completion of previous uplaods
	m_vulkan.getDevice().waitForFences(
		frame->getReadyFence(),							//Fence
		true,											//Wait all
		std::numeric_limits<uint64_t>::max(),			//Timeout
		m_vulkan.getDispatcher()						//Dispatcher
	);

	return frame;
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

const std::shared_ptr<Uploader::Frame>& Uploader::getUniqueFrame() const {
	//Try to find a unused frame.
	//If it is only held by me it is considered to be unused
	for(const auto& frame : m_frames){
		if(frame.unique()){
			return frame;
		}
	}

	//No unused frames were found. Create a new one and return it
	m_frames.emplace_back(std::make_shared<Frame>(
		m_vulkan,
		m_descriptor,
		std::static_pointer_cast<const Buffer>(m_colorTransferBuffer),
		std::static_pointer_cast<const vk::UniqueCommandPool>(m_commandPool)
	));

	return m_frames.back();
}



std::shared_ptr<vk::UniqueCommandPool> Uploader::createCommandPool(const Vulkan& vulkan){
	const vk::CommandPoolCreateInfo createInfo(
		{},													//Flags
		vulkan.getTransferQueueIndex()						//Queue index
	);

	return std::make_shared<vk::UniqueCommandPool>(vulkan.createCommandPool(createInfo));
}





Uploader::Frame::Frame(	const Vulkan& vulkan,
						const Descriptor& desc,
						std::shared_ptr<const Buffer>&& colorTransfer,
						std::shared_ptr<const vk::UniqueCommandPool>&& cmdPool )
	: Graphics::Frame(
		vulkan,
		createImage(vulkan, desc),
		std::move(colorTransfer) )
	, m_stagingBuffer(createStagingBuffer(vulkan, getImage()))
	, m_stagingBufferMemory(mapStagingBuffer(vulkan, m_stagingBuffer))
	, m_pixelData(createPixelData(getImage(), m_stagingBufferMemory))
	, m_commandPool(std::move(cmdPool))
	, m_commandBuffer(createCommandBuffer(m_vulkan, desc, **m_commandPool, getImage(), m_stagingBuffer))
	, m_commandBufferSubmit(createSubmitInfo(*m_commandBuffer))
{
}

const Frame::PixelData& Uploader::Frame::getPixelData() {
	return m_pixelData;
}

void Uploader::Frame::flush() {
	m_stagingBufferMemory.flush();

	//Send it to the queue
	m_vulkan.getDevice().resetFences(getReadyFence(), m_vulkan.getDispatcher());
	m_vulkan.getTransferQueue().submit(
		m_commandBufferSubmit,
		getReadyFence(),
		m_vulkan.getDispatcher()
	);
}



Image Uploader::Frame::createImage(	const Vulkan& vulkan,
									const Descriptor& desc )
{
	constexpr vk::ImageUsageFlags usageFlags =
		vk::ImageUsageFlagBits::eSampled |
		vk::ImageUsageFlagBits::eTransferDst;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eDeviceLocal;

	//Get the plane descriptor array
	std::vector<std::tuple<vk::Extent2D, vk::Format, vk::ComponentMapping>> planeDescriptors;
	for(size_t i = 0; i < desc.colorFormat.size(); i++){
		if(std::get<vk::Format>(desc.colorFormat[i]) != vk::Format::eUndefined){
			planeDescriptors.emplace_back(
				desc.extents[i],
				std::get<vk::Format>(desc.colorFormat[i]),
				std::get<vk::ComponentMapping>(desc.colorFormat[i])
			);
		} else {
			break;
		}
	}

	return Image(
		vulkan,
		usageFlags,
		memoryFlags,
		planeDescriptors
	);
}

Buffer Uploader::Frame::createStagingBuffer(const Vulkan& vulkan,
											const Image& image )
{
	constexpr vk::BufferUsageFlags usageFlags =
		vk::BufferUsageFlagBits::eTransferSrc;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eHostVisible;

	const auto& planes = image.getPlanes();

	return Buffer(
		vulkan,
		usageFlags,
		memoryFlags,
		planes.back().first + planes.back().second 
	);
}

MappedMemory Uploader::Frame::mapStagingBuffer(	const Vulkan& vulkan, 
												const Buffer& buffer)
{
	return MappedMemory(
		vulkan,
		vk::MappedMemoryRange(
			buffer.getDeviceMemory(),
			0, VK_WHOLE_SIZE
		)
	);
}

Frame::PixelData Uploader::Frame::createPixelData(	const Image& image,
													MappedMemory& memory )
{
	Frame::PixelData result;
	const auto planes = image.getPlanes();
	auto* data = memory.data();

	for(size_t i = 0; i < planes.size(); i++){
		result[i] = std::span<std::byte>(data + planes[i].first, planes[i].second);
	}

	return result;
}

vk::UniqueCommandBuffer Uploader::Frame::createCommandBuffer(	const Vulkan& vulkan,
																const Descriptor& desc,
																vk::CommandPool cmdPool,
																const Image& image,
																const Buffer& stagingBuffer )
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
		const auto& images = image.getImages();
		std::vector<vk::ImageMemoryBarrier> memoryBarriers(images.size());
		constexpr vk::ImageSubresourceRange imageSubresourceRange(
			vk::ImageAspectFlagBits::eColor,				//Aspect mask
			0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
		);

		//Transition the layout of the images
		for(size_t i = 0; i < images.size(); i++){
			constexpr vk::AccessFlags srcAccess = {};
			constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferWrite;

			const auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
			const auto dstFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;

			memoryBarriers[i] = vk::ImageMemoryBarrier(
				srcAccess,								//Old access mask
				dstAccess,								//New access mask
				vk::ImageLayout::eUndefined,			//Old layout
				vk::ImageLayout::eTransferDstOptimal,	//New layout
				srcFamily,								//Old queue family
				dstFamily,								//New queue family
				*(images[i]),							//Image
				imageSubresourceRange					//Image subresource
			);
		}

		cmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTopOfPipe,		//Generating stages
			vk::PipelineStageFlagBits::eTransfer,		//Consuming stages
			{},											//dependency flags
			{},											//Memory barriers
			{},											//Buffer memory barriers
			memoryBarriers,								//Image memory barriers
			vulkan.getDispatcher()
		);

		//Copy the buffer to the image
		for(size_t i = 0; i < images.size(); i++){
			const vk::BufferImageCopy region(
				0, 										//Buffer offset
				0,										//Buffer line stride
				0,										//Buffer image lines
				vk::ImageSubresourceLayers(				//Image subresource
					imageSubresourceRange.aspectMask,		//Aspect mask
					imageSubresourceRange.baseMipLevel,			//Mip level
					imageSubresourceRange.baseArrayLayer,	//Array layer offset
					imageSubresourceRange.layerCount 		//Array layers
				),	
				vk::Offset3D(),							//Image offset
				vk::Extent3D(desc.extents[i], 1)		//Image extent
			);

			cmdBuffer.copyBufferToImage(
				stagingBuffer.getBuffer(),
				*(images[i]),
				vk::ImageLayout::eTransferDstOptimal,
				region,
				vulkan.getDispatcher()
			);
		}

		//Transition the layout of the images (again)
		for(size_t i = 0; i < images.size(); i++){
			constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferWrite;
			constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eShaderRead;

			const auto srcFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
			const auto dstFamily = queueOwnershipTransfer ? vulkan.getGraphicsQueueIndex() : VK_QUEUE_FAMILY_IGNORED;

			memoryBarriers[i] = vk::ImageMemoryBarrier(
				srcAccess,								//Old access mask
				dstAccess,								//New access mask
				vk::ImageLayout::eTransferDstOptimal,	//Old layout
				vk::ImageLayout::eShaderReadOnlyOptimal,//New layout
				srcFamily,								//Old queue family
				dstFamily,								//New queue family
				*(images[i]),							//Image
				imageSubresourceRange					//Image subresource
			);
		}

		cmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,		//Generating stages
			vk::PipelineStageFlagBits::eAllGraphics,	//Consuming stages
			{},											//Dependency flags
			{},											//Memory barriers
			{},											//Buffer memory barriers
			memoryBarriers,								//Image memory barriers
			vulkan.getDispatcher()
		);
	}
	cmdBuffer.end(vulkan.getDispatcher());

	return std::move(cmdBuffers[0]);
}

vk::SubmitInfo Uploader::Frame::createSubmitInfo(const vk::CommandBuffer& cmdBuffer)
{
	return vk::SubmitInfo(
		0, nullptr,							//Wait semaphores
		nullptr,							//Pipeline stages
		1, &cmdBuffer,						//Command buffers
		0, nullptr							//Signal semaphores
	);
}



}
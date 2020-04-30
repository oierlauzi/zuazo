#include <Graphics/Uploader.h>

#include <Graphics/VulkanConversions.h>
#include <Exception.h>

namespace Zuazo::Graphics {

Uploader::Uploader(	const Vulkan& vulkan, 
					Resolution resolution,
					AspectRatio par,
					ColorSubsampling subsampling,
					ColorFormat format,
					ColorRange range,
					ColorTransferFunction transferFunction,
					ColorModel model,
					ColorPrimaries primaries )
	: m_vulkan(vulkan)
	, m_frameSize(Frame::getFrameSize(resolution, par))
	, m_colorTransfer(format, range, transferFunction, model, primaries)
	, m_planeDescriptors(createPlaneDescriptors(vulkan, resolution, subsampling, format, m_colorTransfer))
	, m_commandPool(createCommandPool(m_vulkan))
	, m_colorTransferBuffer(std::make_shared<Buffer>(m_colorTransfer.createBuffer(m_vulkan)))
{
}


const std::shared_ptr<Uploader::Frame>& Uploader::acquireFrame() const {
	const auto& frame = getUniqueFrame();

	//Wait for completion of previous uplaods
	m_vulkan.waitForFences(frame->getReadyFence());

	return frame;
}

void Uploader::clear() {
	//Remove all unused frames
	auto ite = m_frames.begin();
	while(ite != m_frames.end()){
		if(ite->unique()){
			m_frames.erase(ite);
		} else {
			++ite;
		}
	} 
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
		m_frameSize,
		m_planeDescriptors,
		m_colorTransferBuffer,
		m_commandPool
	));

	return m_frames.back();
}


std::vector<Image::PlaneDescriptor>	Uploader::createPlaneDescriptors(	const Vulkan& vulkan, 
																		Resolution resolution,
																		ColorSubsampling subsampling,
																		ColorFormat format, 
																		ColorTransfer& colorTransfer)
{
	std::vector<Image::PlaneDescriptor> result = Image::getPlaneDescriptors(resolution, subsampling, format);

	//Try to optimize it
	const auto& supportedFormats = vulkan.getFormatSupport().sampler;
	colorTransfer.optimize(result, supportedFormats);
	for(auto& plane : result) {
		std::tie(plane.format, plane.swizzle) = optimizeFormat(std::make_tuple(plane.format, plane.swizzle));
	}

	return result;
}

std::shared_ptr<vk::UniqueCommandPool> Uploader::createCommandPool(const Vulkan& vulkan){
	const vk::CommandPoolCreateInfo createInfo(
		{},													//Flags
		vulkan.getTransferQueueIndex()						//Queue index
	);

	return std::make_shared<vk::UniqueCommandPool>(vulkan.createCommandPool(createInfo));
}





Uploader::Frame::Frame(	const Vulkan& vulkan,
							Math::Vec2f size,
							Utils::BufferView<const Image::PlaneDescriptor> planes,
							const std::shared_ptr<const Buffer>& colorTransfer,
							const std::shared_ptr<const vk::UniqueCommandPool>& cmdPool )
	: Graphics::Frame(
		vulkan,
		size,
		planes,
		colorTransfer )
	, m_stagingBuffer(createStagingBuffer(vulkan, getImage()))
	, m_pixelData(getPixelData(vulkan, getImage(), m_stagingBuffer))
	, m_commandPool(std::move(cmdPool))
	, m_commandBuffer(createCommandBuffer(m_vulkan, planes, **m_commandPool, getImage(), m_stagingBuffer))
	, m_commandBufferSubmit(createSubmitInfo(*m_commandBuffer))
{
}

Uploader::Frame::~Frame() {
	m_vulkan.waitForFences(getReadyFence());
}

const Image::PixelData& Uploader::Frame::getPixelData() {
	return m_pixelData;
}

void Uploader::Frame::flush() {
	//Flush the mapped memory
	const vk::MappedMemoryRange range(
		m_stagingBuffer.getDeviceMemory(),
		0, VK_WHOLE_SIZE
	);
	m_vulkan.flushMappedMemory(range);

	//Send it to the queue
	m_vulkan.resetFences(getReadyFence());
	m_vulkan.submit(
		m_vulkan.getTransferQueue(),
		m_commandBufferSubmit,
		getReadyFence()
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

Image::PixelData Uploader::Frame::getPixelData(	const Vulkan& vulkan,
												const Image& image,
												const Buffer& buffer )
{
	const vk::MappedMemoryRange range(
		buffer.getDeviceMemory(),
		0, VK_WHOLE_SIZE
	);

	const auto planes = image.getPlanes();
	auto* data = vulkan.mapMemory(range);

	return Image::slice(reinterpret_cast<std::byte*>(data), planes);
}

vk::UniqueCommandBuffer Uploader::Frame::createCommandBuffer(	const Vulkan& vulkan,
																Utils::BufferView<const Image::PlaneDescriptor> planes,
																vk::CommandPool cmdPool,
																const Image& image,
																const Buffer& stagingBuffer )
{
	const vk::CommandBufferAllocateInfo allocInfo(
		cmdPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);

	auto cmdBuffer = vulkan.allocateCommnadBuffer(allocInfo);

	//Record the command buffer
	const vk::CommandBufferBeginInfo cbBeginInfo(
		{},
		nullptr
	);

	vulkan.begin(*cmdBuffer, cbBeginInfo);
	{
		const bool queueOwnershipTransfer = vulkan.getTransferQueueIndex() != vulkan.getGraphicsQueueIndex();
		const auto& images = image.getImages();
		const auto& offsets = image.getPlanes();
		assert(images.size() == offsets.size());
		assert(images.size() == planes.size());

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

		cmdBuffer->pipelineBarrier(
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
				offsets[i].first, 						//Buffer offset
				0,										//Buffer line stride
				0,										//Buffer image lines
				vk::ImageSubresourceLayers(				//Image subresource
					imageSubresourceRange.aspectMask,		//Aspect mask
					imageSubresourceRange.baseMipLevel,			//Mip level
					imageSubresourceRange.baseArrayLayer,	//Array layer offset
					imageSubresourceRange.layerCount 		//Array layers
				),	
				vk::Offset3D(),							//Image offset
				vk::Extent3D(planes[i].extent, 1)		//Image extent
			);

			cmdBuffer->copyBufferToImage(
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

		cmdBuffer->pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,		//Generating stages
			vk::PipelineStageFlagBits::eAllGraphics,	//Consuming stages
			{},											//Dependency flags
			{},											//Memory barriers
			{},											//Buffer memory barriers
			memoryBarriers,								//Image memory barriers
			vulkan.getDispatcher()
		);
	}
	vulkan.end(*cmdBuffer);

	return cmdBuffer;
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
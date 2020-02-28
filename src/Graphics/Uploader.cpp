#include <Graphics/Uploader.h>

#include <Graphics/VulkanConversions.h>
#include <Exception.h>

namespace Zuazo::Graphics {

Uploader::Uploader(	const Vulkan& vulkan,
					const Descriptor& descriptor )
	: m_vulkan(vulkan)
	, m_descriptor(descriptor)
	, m_commandPool(createCommandPool(m_vulkan))
{
}

/*std::shared_ptr<const Frame> Uploader::operator()(const Utils::BufferView<Utils::BufferView<std::byte>>& pixData) const{
	const auto& frame = acquireFrame();
	const auto& data = static_cast<FrameData&>(frame->getData());
	const auto& memory = data.stagingBuffer.getDeviceMemory();
	const auto& offsets = frame->getImage().getOffsets();
	assert(offsets.size() == pixData.size());

	//Wait for completion of previous uplaods
	m_vulkan.getDevice().waitForFences(
		frame->getReadyFence(),							//Fence
		true,											//Wait all
		std::numeric_limits<uint64_t>::max(),			//Timeout
		m_vulkan.getDispatcher()						//Dispatcher
	);

	//Copy the provided data to the staging buffer by mapping it
	// onto memory
	void* mappedMemory = m_vulkan.getDevice().mapMemory(memory, 
														0, VK_WHOLE_SIZE, 
														{}, 
														m_vulkan.getDispatcher());

	for(size_t i = 0; i < pixData.size(); i++) {
		std::memcpy(
			mappedMemory + offsets[i],
			pixData[i].data(),
			pixData[i].size()
		);
	}

	m_vulkan.getDevice().unmapMemory(memory, m_vulkan.getDispatcher());

	//Submit the commandbuffer to the queue
	m_vulkan.getDevice().resetFences(
		frame->getReadyFence(), 
		m_vulkan.getDispatcher()
	);
	m_vulkan.getTransferQueue().submit(
		data.commandBufferSubmit, 
		frame->getReadyFence(), 
		m_vulkan.getDispatcher()
	);

	return std::static_pointer_cast<const Frame>(frame);
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
}*/

std::shared_ptr<Frame> Uploader::createFrame() const {
	/*const vk::SubmitInfo submitInfo(
		0, nullptr,							//Wait semaphores
		nullptr,							//Pipeline stages
		1, &(*data.commandBuffer),			//Command buffers
		1, &(frame->getReadySemaphore())	//Signal semaphores
	);*/
	//TODO
	return {};
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





std::shared_ptr<vk::UniqueCommandPool> Uploader::createCommandPool(const Vulkan& vulkan){
	const vk::CommandPoolCreateInfo createInfo(
		{},													//Flags
		vulkan.getTransferQueueIndex()						//Queue index
	);

	return std::make_shared<vk::UniqueCommandPool>(vulkan.createCommandPool(createInfo));
}

}
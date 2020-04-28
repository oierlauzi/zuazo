#include <Graphics/ColorTransfer.h>
#include <Graphics/VulkanConversions.h>

#include "color_transfer.h"

namespace Zuazo::Graphics {


/*
 * ColorTransfer::Impl
 */
struct ColorTransfer::Impl {
	ct_data transferData;

	Impl() = default;
	Impl(	ColorFormat format,
			ColorRange range,
			ColorTransferFunction transferFunc,
			ColorModel model,
			ColorPrimaries primaries )
		: transferData {
			getConversionMatrix(primaries),
			getConversionMatrix(model),
			getTransferFunction(transferFunc),
			getRange(range, model),
			getPlaneFormat(format)
		}
	{
	}

	void optimize(	Utils::BufferView<Image::PlaneDescriptor> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) 
	{
		//For binary search:
		assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend()));

		if(	transferData.colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1 && 
			transferData.colorRange == ct_COLOR_RANGE_FULL_RGB )
		{
			//For being able to use Vulkan's built in sRGB formats, all planes need to support it
			transferData.colorTransferFunction = ct_COLOR_TRANSFER_FUNCTION_LINEAR; //Suppose it is supported
			for(size_t i = 0; i < planes.size(); i++) {
				//Evaluate if it can be optimized
				const auto optimized = toSrgb(planes[i].format);
				if(optimized == planes[i].format){ //toSrgb returns the format itself in case of failure
					//Optimization not available
					transferData.colorTransferFunction = ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1;
					break; 
				}
				if(	!std::binary_search(supportedFormats.cbegin(), 
										supportedFormats.cend(), 
										optimized ))
				{
					//Optimization not supported
					transferData.colorTransferFunction = ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1;
					break;
				}

				//This plane can be optimized
				planes[i].format = optimized;
			}

			if(transferData.colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1) {
				//Optimization failed
				for(size_t i = 0; i < planes.size(); i++) {
					//Reset all changes
					//Note that fromSrgb returns the format itself it is not sRGB, so that unchanged formats will remain unchanged
					planes[i].format = fromSrgb(planes[i].format);
				}
			}
		}
	}

	const std::byte* data() const {
		return reinterpret_cast<const std::byte*>(&transferData);
	}

	Buffer createBuffer(const Vulkan& vulkan) const {
		//Create the buffer itself
		constexpr vk::BufferUsageFlags usageFlags =
			vk::BufferUsageFlagBits::eUniformBuffer |
			vk::BufferUsageFlagBits::eTransferDst;

		constexpr vk::MemoryPropertyFlags memoryFlags =
			vk::MemoryPropertyFlagBits::eDeviceLocal;

		Buffer result(
			vulkan,
			usageFlags,
			memoryFlags,
			size()
		);
	
		//Create a staging buffer. Using this technique due to preferably having
		//device memory allocated for the UBO, as it will be only written once and
		//read frequently
		constexpr vk::BufferUsageFlags stagingUsageFlags =
			vk::BufferUsageFlagBits::eTransferSrc;

		constexpr vk::MemoryPropertyFlags stagingMemoryFlags =
			vk::MemoryPropertyFlagBits::eHostVisible;

		Buffer stagingBuffer(
			vulkan,
			stagingUsageFlags,
			stagingMemoryFlags,
			size()
		);

		//Upload the contents to the staging buffer
		const vk::MappedMemoryRange range(
			stagingBuffer.getDeviceMemory(),
			0, VK_WHOLE_SIZE
		);

		auto* stagingBufferData = vulkan.mapMemory(range);
		std::memcpy(
			stagingBufferData,
			data(),
			size()
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

		const auto cmdBuffer = vulkan.allocateCommnadBuffer(cbAllocInfo);

		//Record the command buffer
		constexpr vk::CommandBufferUsageFlags cbUsage =
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		const vk::CommandBufferBeginInfo cbBeginInfo(
			cbUsage,
			nullptr
		);

		vulkan.begin(*cmdBuffer, cbBeginInfo);
		{
			cmdBuffer->copyBuffer(
				stagingBuffer.getBuffer(),
				result.getBuffer(),
				vk::BufferCopy(0, 0, size()),
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
			
			cmdBuffer->pipelineBarrier(
				sourceStages,					//Generating stages
				destinationStages,				//Consuming stages
				{},								//dependency flags
				{},								//Memory barriers
				memoryBarrier,					//Buffer memory barriers
				{},								//Image memory barriers
				vulkan.getDispatcher()
			);
		}
		vulkan.end(*cmdBuffer);

		//Submit the command buffer
		const auto uploadFence = vulkan.createFence();
		const vk::SubmitInfo submitInfo(
			0, nullptr,							//Wait semaphores
			nullptr,							//Pipeline stages
			1, &(*cmdBuffer),					//Command buffers
			0, nullptr							//Signal semaphores
		);

		vulkan.submit(
			vulkan.getTransferQueue(),
			submitInfo,
			*uploadFence
		);

		//Wait for completion
		vulkan.waitForFences(*uploadFence);

		return result;
	}

private:
	static int32_t getTransferFunction(ColorTransferFunction transferFunction) {
		switch(transferFunction){
		case ColorTransferFunction::LINEAR: return ct_COLOR_TRANSFER_FUNCTION_LINEAR;
		case ColorTransferFunction::IEC61966_2_1: return ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1;
		default: break;
		}

		return -1;
	}

	static int32_t getRange(ColorRange range, ColorModel model) {
		switch(model){
		case ColorModel::RGB:
			switch(range){
			case ColorRange::FULL: return ct_COLOR_RANGE_FULL_RGB;
			case ColorRange::ITU_NARROW: return ct_COLOR_RANGE_ITU_NARROW_RGB;
			default: break;
			}
			break;
		case ColorModel::BT601:
		case ColorModel::BT709:
		case ColorModel::BT2020:
			switch(range){
			case ColorRange::FULL: return ct_COLOR_RANGE_FULL_YCBCR;
			case ColorRange::ITU_NARROW: return ct_COLOR_RANGE_ITU_NARROW_YCBCR;
			default: break;
			}
			break;
		default: break;
		}

		return -1;
	}

	static int32_t getPlaneFormat(ColorFormat format) {
		const auto planeCount = getPlaneCount(format);

		switch(planeCount){
		case 1: return ct_PLANE_FORMAT_RGBA;
		case 2: return ct_PLANE_FORMAT_G_BR;
		case 3: return ct_PLANE_FORMAT_G_B_R;
		default: return -1;
		}
	}
};




/*
 * ColorTransfer
 */

ColorTransfer::ColorTransfer() = default;

ColorTransfer::ColorTransfer(	ColorFormat format,
								ColorRange range,
								ColorTransferFunction transferFunc,
								ColorModel model,
								ColorPrimaries primaries )
	: m_impl(format, range, transferFunc, model, primaries)
{
}

ColorTransfer::ColorTransfer(ColorTransfer&& other) = default;

ColorTransfer::~ColorTransfer() = default;


ColorTransfer& ColorTransfer::operator=(ColorTransfer&& other) = default;


void ColorTransfer::optimize(	Utils::BufferView<Image::PlaneDescriptor> planes,
								Utils::BufferView<const vk::Format> supportedFormats ) 
{
	m_impl->optimize(planes, supportedFormats);
}


const std::byte* ColorTransfer::data() const {
	return m_impl->data();
}


Buffer ColorTransfer::createBuffer(const Vulkan& vulkan) const {
	return m_impl->createBuffer(vulkan);
}



uint32_t ColorTransfer::getSamplerCount() {
	return ct_SAMPLER_COUNT;
}

uint32_t ColorTransfer::getSamplerBinding() {
	return ct_SAMPLER_BINDING;
}

uint32_t ColorTransfer::getDataBinding() {
	return ct_DATA_BINDING;
}

size_t ColorTransfer::size() {
	return sizeof(ct_data);
}


Buffer 		createColorTransferBuffer(	const Vulkan& vulkan,
																	const ColorTransfer& colorTransfer );

}
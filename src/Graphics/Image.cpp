#include <zuazo/Graphics/Image.h>

#include <zuazo/Graphics/Sampler.h>
#include <zuazo/Graphics/VulkanConversions.h>

#include <algorithm>

namespace Zuazo::Graphics {

Image::Image(	const Vulkan& vulkan,
				Utils::BufferView<const Plane> planes,
				vk::ImageUsageFlags usage,
				vk::ImageTiling tiling,
				vk::MemoryPropertyFlags memoryProp,
				const Sampler* sampler )
	: m_planes(planes.cbegin(), planes.cend())
{
	createImages(vulkan, usage, tiling, memoryProp);
	createImageViews(vulkan, usage, sampler);
}


Utils::BufferView<const Image::Plane> Image::getPlanes() const {
	return m_planes;
}

const Vulkan::AggregatedAllocation& Image::getMemory() const {
	return m_memory;
}



void Image::createImages(	const Vulkan& vulkan, 
							vk::ImageUsageFlags usage,
							vk::ImageTiling tiling,
							vk::MemoryPropertyFlags memoryProp ) 
{
	//Create the necessary images
	for(auto& plane : m_planes) {
		if(!plane.getImage()) {
			//This plane does not have an image
			m_images.emplace_back(createImage(vulkan, plane, usage, tiling));
		}
	}

	//Bind memory for them if necessary
	if(!m_images.empty()) {
		//Allocate memory for the images
		std::vector<vk::MemoryRequirements> memoryReq;
		memoryReq.reserve(m_images.size());

		for(const auto& image : m_images) {
			assert(image);
			memoryReq.push_back(vulkan.getMemoryRequirements(*image));
		}
		assert(memoryReq.size() == m_images.size());

		m_memory = vulkan.allocateMemory(memoryReq, memoryProp);
		assert(m_memory.areas.size() == m_images.size());


		//Bind memory to the images
		//Manual approach:
		/*for(size_t i = 0; i < m_images.size(); ++i) {
			if(m_images[i]) {
				vulkan.bindMemory(
					*m_images[i], 
					*m_memory.memory, 
					m_memory.areas[i].offset()
				);
			}
		}*/

		std::vector<vk::BindImageMemoryInfo> bindInfos;
		bindInfos.reserve(m_images.size());
		for(size_t i = 0; i < m_images.size(); ++i) {
			bindInfos.emplace_back(*m_images[i], *m_memory.memory, m_memory.areas[i].offset());
		}
		
		vulkan.bindMemory(Utils::BufferView<const vk::BindImageMemoryInfo>(bindInfos));
	}
}

void Image::createImageViews(	const Vulkan& vulkan, 
								vk::ImageUsageFlags usage,
								const Sampler* sampler ) 
{
	constexpr vk::ImageUsageFlags IMAGE_VIEW_USAGE_FLAGS =
		vk::ImageUsageFlagBits::eSampled |
		vk::ImageUsageFlagBits::eStorage |
		vk::ImageUsageFlagBits::eColorAttachment |
		vk::ImageUsageFlagBits::eDepthStencilAttachment |
		vk::ImageUsageFlagBits::eInputAttachment |
		vk::ImageUsageFlagBits::eTransientAttachment ;

	if(usage & IMAGE_VIEW_USAGE_FLAGS) {
		for(auto& plane : m_planes) {
			if(!plane.getImageView()) {
				//This plane does not have an image view
				m_imageViews.push_back(createImageView(vulkan, plane, sampler));
			}
		}
	}
}


vk::UniqueImage Image::createImage(	const Vulkan& vulkan,
									Plane& plane, 
									vk::ImageUsageFlags usage,
									vk::ImageTiling tiling )
{
	const vk::ImageCreateInfo createInfo(
		{},											//Flags
		vk::ImageType::e2D,							//Image type
		plane.getFormat(),							//Pixel format
		plane.getExtent(), 							//Extent
		1,											//Mip levels
		1,											//Array layers
		vk::SampleCountFlagBits::e1,				//Sample count
		tiling,										//Tiling
		usage,										//Usage
		vk::SharingMode::eExclusive,				//Sharing mode
		0, nullptr,									//Queue family indices
		vk::ImageLayout::eUndefined					//Initial layout
	);

	auto result = vulkan.createImage(createInfo);
	plane.setImage(*result);
	return result;
}

vk::UniqueImageView Image::createImageView(	const Vulkan& vulkan,
											Plane& plane,
											const Sampler* sampler )
{
	//Decide the aspect mask of the image view
	vk::ImageAspectFlags aspectMask;

	if(hasDepth(plane.getFormat())) {
		aspectMask |= vk::ImageAspectFlagBits::eDepth;
	}

	if(hasStencil(plane.getFormat())) {
		aspectMask |= vk::ImageAspectFlagBits::eStencil;
	}

	if(aspectMask == vk::ImageAspectFlags()) {
		//None was selected, default to color
		aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	const vk::ImageSubresourceRange subresourceRange(
		aspectMask,										//Aspect mask
		0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
	);

	//We'll might need to know about the YCbCr sampler
	const vk::SamplerYcbcrConversionInfo samplerYCbCrConversionInfo(sampler ? sampler->getSamplerYCbCrConversion() : nullptr);
	const void* pNext = samplerYCbCrConversionInfo.conversion ? &samplerYCbCrConversionInfo : nullptr;
	const auto swizzle = pNext ? vk::ComponentMapping() : plane.getSwizzle(); //Only set swizzle if not set by pNext

	assert(plane.getImage());
	const auto createInfo = vk::ImageViewCreateInfo(
		{},												//Flags
		plane.getImage(),								//Image
		vk::ImageViewType::e2D,							//ImageView type
		plane.getFormat(),								//Image format
		swizzle,										//Swizzle
		subresourceRange								//Image subresources
	).setPNext(pNext);

	auto result = vulkan.createImageView(createInfo);
	plane.setImageView(*result);
	return result;
}



void copy(	const Vulkan& vulkan, 
			vk::CommandBuffer cmd,
			const Image& src, 
			Image& dst )
{
	const auto srcPlaneCnt = src.getPlanes().size();
	const auto dstPlaneCnt = dst.getPlanes().size();
	const auto planeCount = Math::max(srcPlaneCnt, dstPlaneCnt);
	
	//In order to work, it must either have the same plane count
	assert(srcPlaneCnt == planeCount || getPlaneCount(src.getPlanes().front().getFormat()) == planeCount);
	assert(dstPlaneCnt == planeCount || getPlaneCount(dst.getPlanes().front().getFormat()) == planeCount);
	
	//Copy plane by plane
	for(size_t i = 0; i < planeCount; ++i) {
		const auto srcImage = srcPlaneCnt < planeCount ? src.getPlanes().front().getImage() : src.getPlanes()[i].getImage();
		const auto dstImage = dstPlaneCnt < planeCount ? dst.getPlanes().front().getImage() : dst.getPlanes()[i].getImage();
		const vk::ImageAspectFlags srcAspectMask = srcPlaneCnt < planeCount ? getPlaneAspectBit(i) : vk::ImageAspectFlagBits::eColor;
		const vk::ImageAspectFlags dstAspectMask = dstPlaneCnt < planeCount ? getPlaneAspectBit(i) : vk::ImageAspectFlagBits::eColor;
		const auto extent = (srcPlaneCnt == planeCount) ? src.getPlanes()[i].getExtent() : dst.getPlanes()[i].getExtent();
		constexpr uint32_t baseMipLevel = 0;
		constexpr uint32_t baseArrayLevel = 0;
		constexpr uint32_t layerCount = 1;


		//Ensure that they are all valid:
		assert(srcImage); assert(dstImage); 
		assert(srcAspectMask); assert(dstAspectMask); 

		//Obtain the region to copy
		const vk::ImageCopy region(
			vk::ImageSubresourceLayers(srcAspectMask, baseMipLevel, baseArrayLevel, layerCount),			//Src subresource
			vk::Offset3D(),																					//Src offset
			vk::ImageSubresourceLayers(dstAspectMask, baseMipLevel, baseArrayLevel, layerCount),			//Dst subresource
			vk::Offset3D(),																					//Dst offset
			extent																							//Image extent
		);

		//Issue the copy command
		vulkan.copy(
			cmd,									//Command buffer
			srcImage,								//Src image
			vk::ImageLayout::eTransferSrcOptimal,	//Src image layout
			dstImage,								//Dst image
			vk::ImageLayout::eTransferDstOptimal,	//Dst image layout
			region									//Regions
		);
	}
}

}
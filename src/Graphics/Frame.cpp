#include <zuazo/Graphics/Frame.h>

#include <zuazo/Exception.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/StagedBuffer.h>

#include <zuazo/shaders/color_transfer.h>

#include <vector>

namespace Zuazo::Graphics {

struct Frame::Impl {
	const Vulkan&						vulkan;

	std::shared_ptr<const Descriptor> 	descriptor;
	Math::Vec2f 						size;
	std::shared_ptr<const Buffer> 		colorTransferBuffer;

	std::vector<vk::UniqueImage> 		images;
	Vulkan::AggregatedAllocation		memory;
	std::vector<vk::UniqueImageView>	imageViews;

	vk::UniqueDescriptorPool			descriptorPool;
	std::vector<vk::DescriptorSet>		descriptorSets;

	Impl(	const Vulkan& vulkan,
			std::shared_ptr<const Descriptor> desc,
			std::shared_ptr<const Buffer> colorTransfer,
			Utils::BufferView<const PlaneDescriptor> planes,
			vk::ImageUsageFlags usage )
		: vulkan(vulkan)
		, descriptor(std::move(desc))
		, size(descriptor->calculateSize())
		, colorTransferBuffer(std::move(colorTransfer))
		, images(createImages(vulkan, usage, planes))
		, memory(allocateMemory(vulkan, images))
		, imageViews(createImageViews(vulkan, planes, images))
		, descriptorPool(createDescriptorPool(vulkan))
		, descriptorSets(allocateDescriptorSets(vulkan, *descriptorPool))
	{
		assert(descriptor);
		assert(colorTransferBuffer);
		assert(images.size());

		writeDescriptorSets();
	}

	~Impl() = default;


	void bind(	vk::CommandBuffer cmd,
					vk::PipelineLayout layout,
					uint32_t index,
					vk::Filter filter ) const noexcept
	{
		const auto descriptorSet = descriptorSets[static_cast<size_t>(filter)];

		cmd.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,	//Pipeline bind point
			layout,								//Pipeline layout
			index,								//First index
			descriptorSet,						//Descriptor sets
			{},									//Dynamic offsets
			vulkan.getDispatcher()
		);
	}



	const Vulkan& getVulkan() const noexcept { 
		return vulkan;
	}

	const Descriptor& getDescriptor() const noexcept {
		return *descriptor;
	}

	const Math::Vec2f& getSize() const noexcept {
		return size;
	}

	const std::vector<vk::UniqueImage>& getImages() const noexcept {
		return images;
	}

	const std::vector<vk::UniqueImageView>& getImageViews() const noexcept {
		return imageViews;
	}

	const std::vector<Utils::Area>& getPlaneAreas() const noexcept {
		return memory.areas;
	}

	const vk::DeviceMemory& getMemory() const noexcept {
		return *(memory.memory);
	}


private:
	void writeDescriptorSets() {
		for(size_t i = 0; i < descriptorSets.size(); i++){
			//Update all images. As nullptr images can't be passed, repeat available images		
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
					colorTransferBuffer->getBuffer(),						//Buffer
					0,														//Offset
					InputColorTransfer::size()								//Size
				)
			};

			const std::array writeDescriptorSets ={
				vk::WriteDescriptorSet( //Image descriptor
					descriptorSets[i],										//Descriptor set
					InputColorTransfer::getSamplerBinding(),				//Binding
					0, 														//Index
					images.size(), 											//Descriptor count
					vk::DescriptorType::eCombinedImageSampler,				//Descriptor type
					images.data(), 											//Images
					nullptr, 												//Buffers
					nullptr													//Texel buffers
				),
				vk::WriteDescriptorSet( //Ubo descriptor set
					descriptorSets[i],										//Descriptor set
					InputColorTransfer::getDataBinding(),					//Binding
					0, 														//Index
					buffers.size(),											//Descriptor count		
					vk::DescriptorType::eUniformBuffer,						//Descriptor type
					nullptr, 												//Images 
					buffers.data(), 										//Buffers
					nullptr													//Texel buffers
				)
			};

			vulkan.updateDescriptorSets(Utils::BufferView<const vk::WriteDescriptorSet>(writeDescriptorSets));
		}
	}

	static std::vector<vk::UniqueImage> createImages(	const Vulkan& vulkan,
														vk::ImageUsageFlags usage,
														Utils::BufferView<const PlaneDescriptor> imagePlanes )
	{
		std::vector<vk::UniqueImage> result;
		result.reserve(imagePlanes.size());

		usage |= vk::ImageUsageFlagBits::eSampled; //Ensure sampled is set

		for(size_t i = 0; i < imagePlanes.size(); i++){
			const vk::ImageCreateInfo createInfo(
				{},											//Flags
				vk::ImageType::e2D,							//Image type
				imagePlanes[i].format,						//Pixel format
				vk::Extent3D(imagePlanes[i].extent, 1), 	//Extent
				1,											//Mip levels
				1,											//Array layers
				vk::SampleCountFlagBits::e1,				//Sample count
				vk::ImageTiling::eOptimal,					//Tiling
				usage,										//Usage
				vk::SharingMode::eExclusive,				//Sharing mode
				0, nullptr,									//Queue family indices
				vk::ImageLayout::eUndefined					//Initial layout
			);

			result.emplace_back(vulkan.createImage(createInfo));
		}
		
		return result;
	}

	static Vulkan::AggregatedAllocation	allocateMemory(	const Vulkan& vulkan,
														const std::vector<vk::UniqueImage>& images )
	{
		constexpr vk::MemoryPropertyFlags memoryProperties =
			vk::MemoryPropertyFlagBits::eDeviceLocal;

		std::vector<vk::MemoryRequirements> requirements;
		requirements.reserve(images.size());

		for(size_t i = 0; i < images.size(); i++) {
			requirements.push_back(vulkan.getMemoryRequirements(*images[i]));
		}

		auto result = vulkan.allocateMemory(requirements, memoryProperties);
		assert(result.areas.size() == images.size());

		//Bind image's memory
		for(size_t i = 0; i < images.size(); i++){
			vulkan.bindMemory(*images[i], *result.memory, result.areas[i].offset());
		}

		return result;
	}

	static std::vector<vk::UniqueImageView>	createImageViews(	const Vulkan& vulkan,
																Utils::BufferView<const PlaneDescriptor> imagePlanes,
																const std::vector<vk::UniqueImage>& images )
	{
		assert(imagePlanes.size() == images.size());
		std::vector<vk::UniqueImageView> result;
		result.reserve(images.size());

		for(size_t i = 0; i < images.size(); i++){
			const vk::ImageViewCreateInfo createInfo(
				{},												//Flags
				*(images[i]),									//Image
				vk::ImageViewType::e2D,							//ImageView type
				imagePlanes[i].format,							//Image format
				imagePlanes[i].swizzle,							//Swizzle
				vk::ImageSubresourceRange(						//Image subresources
					vk::ImageAspectFlagBits::eColor,				//Aspect mask
					0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
				)
			);

			result.emplace_back(vulkan.createImageView(createInfo));
		}

		return result;
	}

	static vk::UniqueDescriptorPool	createDescriptorPool(const Vulkan& vulkan) {
		//A descriptor pool is created, from which 2 descriptor sets, each one with 
		//one combined image sampler and one uniform buffer. Threrefore, 2 descriptors
		//of each type will be required.
		const std::array poolSizes = {
			vk::DescriptorPoolSize(
				vk::DescriptorType::eCombinedImageSampler,			//Descriptor type
				FILTER_COUNT * InputColorTransfer::getSamplerCount()//Descriptor count
			),
			vk::DescriptorPoolSize(
				vk::DescriptorType::eUniformBuffer,					//Descriptor type
				FILTER_COUNT										//Descriptor count
			)
		};

		const vk::DescriptorPoolCreateInfo createInfo(
			{},														//Flags
			FILTER_COUNT,											//Descriptor set count
			poolSizes.size(), poolSizes.data()						//Pool sizes
		);

		return vulkan.createDescriptorPool(createInfo);
	}

	static std::vector<vk::DescriptorSet> allocateDescriptorSets(	const Vulkan& vulkan,
																	vk::DescriptorPool pool )
	{
		std::vector<vk::DescriptorSet> result;

		std::array<vk::DescriptorSetLayout, FILTER_COUNT> layouts;
		for(size_t i = 0; i < layouts.size(); i++){
			const auto filter = static_cast<vk::Filter>(i);
			layouts[i] = getDescriptorSetLayout(vulkan, filter);
		}

		const vk::DescriptorSetAllocateInfo allocInfo(
			pool,													//Pool
			layouts.size(), layouts.data()							//Layouts
		);

		//Allocate descriptor sets
		auto uniqueDescriptorSets = vulkan.allocateDescriptorSets(allocInfo);

		//Copy them into the result buffer releasing them
		result.reserve(uniqueDescriptorSets.size());
		std::transform(
			uniqueDescriptorSets.begin(), uniqueDescriptorSets.end(),
			std::back_inserter(result),
			std::mem_fn(&vk::UniqueDescriptorSet::release)
		);

		return result;
	}
};


Frame::Frame(	const Vulkan& vulkan,
				std::shared_ptr<const Descriptor> desc,
				std::shared_ptr<const Buffer> colorTransfer,
				Utils::BufferView<const PlaneDescriptor> planes,
				vk::ImageUsageFlags usage )
	: m_impl({}, vulkan, std::move(desc), std::move(colorTransfer), planes, usage)
{
}

Frame::Frame(Frame&& other) noexcept = default;

Frame::~Frame() = default;

Frame& Frame::operator=(Frame&& other) noexcept = default;



void Frame::bind( 	vk::CommandBuffer cmd,
					vk::PipelineLayout layout,
					uint32_t index,
					vk::Filter filter ) const noexcept
{
	m_impl->bind(cmd, layout, index, filter);
}



const Vulkan& Frame::getVulkan() const noexcept { 
	return m_impl->getVulkan();
}

const Frame::Descriptor& Frame::getDescriptor() const noexcept {
	return m_impl->getDescriptor();
}

const Math::Vec2f& Frame::getSize() const noexcept {
	return m_impl->getSize();
}


const std::vector<vk::UniqueImage>& Frame::getImages() const noexcept {
	return m_impl->getImages();
}

const std::vector<vk::UniqueImageView>& Frame::getImageViews() const noexcept {
	return m_impl->getImageViews();
}

const std::vector<Utils::Area>& Frame::getPlaneAreas() const noexcept {
	return m_impl->getPlaneAreas();
}

const vk::DeviceMemory& Frame::getMemory() const noexcept {
	return m_impl->getMemory();
}



std::shared_ptr<StagedBuffer> Frame::createColorTransferBuffer(	const Vulkan& vulkan,
																const InputColorTransfer& colorTransfer )
{
	constexpr vk::BufferUsageFlags usage =
		vk::BufferUsageFlagBits::eUniformBuffer;

	auto result = std::make_unique<StagedBuffer>(vulkan, usage, colorTransfer.size());

	//Copy the data onto it
	std::memcpy(result->data(), colorTransfer.data(), colorTransfer.size());
	result->flushData(
		vulkan, 
		vulkan.getGraphicsQueueIndex(), 
		vk::AccessFlagBits::eUniformRead,
		vk::PipelineStageFlagBits::eAllGraphics
	);

	return result;
}

std::vector<Frame::PlaneDescriptor> Frame::getPlaneDescriptors(const Descriptor& desc) {
	const Resolution resolution = desc.getResolution();
	const ColorSubsampling subsampling = desc.getColorSubsampling();
	const ColorFormat format = desc.getColorFormat();

	const auto planeCount = getPlaneCount(format);
	std::vector<PlaneDescriptor> result;
	result.reserve(planeCount);

	const auto formats = toVulkan(format);
	const auto extent = toVulkan(resolution);
	const auto subsampledExtent = toVulkan(getSubsampledResolution(subsampling, resolution));

	assert(planeCount <= formats.size());

	for(size_t i = 0; i < planeCount; i++) {
		result.push_back(
			PlaneDescriptor {
				(i == 0) ? extent : subsampledExtent,
				std::get<vk::Format>(formats[i]),
				std::get<vk::ComponentMapping>(formats[i])
			}
		);
	}

	return result;
}

Frame::SamplerDescriptor Frame::getSamplerDescriptor(ScalingFilter filter) {
	Frame::SamplerDescriptor result;

	if(filter == ScalingFilter::CUBIC) {
		result = {
			vk::Filter::eLinear,
			ct_SAMPLE_MODE_BILINEAR2BICUBIC
		};
	} else {
		result = {
			toVulkan(filter),
			ct_SAMPLE_MODE_PASSTHOUGH
		};
	}

	return result;
}

vk::DescriptorSetLayout	Frame::getDescriptorSetLayout(	const Vulkan& vulkan,
														vk::Filter filter) 
{
	static const std::array<Utils::StaticId, FILTER_COUNT> ids;
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
		const std::vector<vk::Sampler> inmutableSamplers(InputColorTransfer::getSamplerCount(), sampler);

		//Create the bindings
		const std::array bindings = {
			vk::DescriptorSetLayoutBinding( //Sampled image binding
				InputColorTransfer::getSamplerBinding(),		//Binding
				vk::DescriptorType::eCombinedImageSampler,		//Type
				inmutableSamplers.size(),						//Count
				vk::ShaderStageFlagBits::eAllGraphics,			//Shader stage
				inmutableSamplers.data()						//Inmutable samplers
			), 
			vk::DescriptorSetLayoutBinding(	//Color transfer binding
				InputColorTransfer::getDataBinding(),			//Binding
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

}
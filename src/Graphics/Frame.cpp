#include <zuazo/Graphics/Frame.h>

#include <zuazo/Exception.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/StagedBuffer.h>

#include <vector>

namespace Zuazo::Graphics {

struct Frame::Impl {
	const Vulkan&						vulkan;

	std::shared_ptr<const Descriptor> 	descriptor;
	Math::Vec2f 						size;
	std::shared_ptr<const Buffer> 		colorTransferBuffer;
	Image								image;

	vk::UniqueDescriptorPool			descriptorPool;
	std::vector<vk::DescriptorSet>		descriptorSets;

	Impl(	const Vulkan& vulkan,
			std::shared_ptr<const Descriptor> desc,
			std::shared_ptr<const Buffer> colorTransfer,
			Utils::BufferView<const Image::PlaneDescriptor> planes,
			vk::ImageUsageFlags usage )
		: vulkan(vulkan)
		, descriptor(std::move(desc))
		, size(descriptor->calculateSize())
		, colorTransferBuffer(std::move(colorTransfer))
		, image(vulkan, planes, usage | vk::ImageUsageFlagBits::eSampled)
		, descriptorPool(createDescriptorPool(vulkan))
		, descriptorSets(allocateDescriptorSets(vulkan, *descriptorPool))
	{
		assert(descriptor);
		assert(colorTransferBuffer);

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

	const Image& getImage() const noexcept {
		return image;
	}


private:
	void writeDescriptorSets() {
		for(size_t i = 0; i < descriptorSets.size(); i++){
			//Update all images. As nullptr images can't be passed, repeat available images		
			const auto planes = image.getPlanes();
			std::vector<vk::DescriptorImageInfo> images;
			images.reserve(InputColorTransfer::getSamplerCount());
			for(const auto& plane : planes) {
				images.emplace_back(
					nullptr,												//Sampler
					*plane.imageView,										//Image views
					vk::ImageLayout::eShaderReadOnlyOptimal					//Layout
				);
			}
			while(images.size() < InputColorTransfer::getSamplerCount()) {
				images.emplace_back(
					nullptr,												//Sampler
					*planes.front().imageView,								//Image views
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
				Utils::BufferView<const Image::PlaneDescriptor> planes,
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


const Image& Frame::getImage() const noexcept {
	return m_impl->getImage();
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

std::vector<Image::PlaneDescriptor> Frame::getPlaneDescriptors(const Descriptor& desc) {
	const Resolution resolution = desc.getResolution();
	const ColorSubsampling subsampling = desc.getColorSubsampling();
	const ColorFormat format = desc.getColorFormat();

	const auto planeCount = getPlaneCount(format);
	std::vector<Image::PlaneDescriptor> result;
	result.reserve(planeCount);

	const auto formats = toVulkan(format);
	const auto extent = toVulkan(resolution);
	const auto subsampledExtent = toVulkan(getSubsampledResolution(subsampling, resolution));

	assert(planeCount <= formats.size());

	for(size_t i = 0; i < planeCount; i++) {
		result.push_back(
			Image::PlaneDescriptor {
				(i == 0) ? extent : subsampledExtent, //FIXME G_B_R_A planar format
				std::get<vk::Format>(formats[i]),
				std::get<vk::ComponentMapping>(formats[i])
			}
		);
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
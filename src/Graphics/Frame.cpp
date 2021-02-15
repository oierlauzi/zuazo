#include <zuazo/Graphics/Frame.h>

#include <zuazo/Exception.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/StagedBuffer.h>

#include <zuazo/shaders/frame.h>

#include <vector>
#include <bitset>

namespace Zuazo::Graphics {

struct Frame::Impl {
	static constexpr auto FILTER_COUNT = std::tuple_size<Samplers>::value;

	const Vulkan&										vulkan;

	std::shared_ptr<const Descriptor> 					descriptor;
	std::shared_ptr<const Buffer> 						colorTransferBuffer;
	std::shared_ptr<const Samplers> 					samplers;
	Image												image;

	vk::UniqueDescriptorPool							descriptorPool;
	std::array<vk::DescriptorSetLayout, FILTER_COUNT>	descriptorSetLayouts;
	std::array<vk::DescriptorSet, FILTER_COUNT>			descriptorSets;

	Impl(	const Vulkan& vulkan,
			std::shared_ptr<const Descriptor> desc,
			std::shared_ptr<const Buffer> colorTransferBuffer,
			std::shared_ptr<const Samplers> smp,
			Utils::BufferView<const Image::Plane> planes,
			vk::ImageUsageFlags usage )
		: vulkan(vulkan)
		, descriptor(std::move(desc))
		, colorTransferBuffer(std::move(colorTransferBuffer))
		, samplers(std::move(smp))
		, image(createImage(vulkan, planes, usage, *samplers))
		, descriptorPool(createDescriptorPool(vulkan))
		, descriptorSetLayouts(createDescriptorSetLayouts(vulkan, *samplers))
		, descriptorSets(allocateDescriptorSets(vulkan, *descriptorPool, descriptorSetLayouts))
	{
		writeDescriptorSets();
	}

	~Impl() = default;



	const Vulkan& getVulkan() const noexcept { 
		return vulkan;
	}

	const Descriptor& getDescriptor() const noexcept {
		assert(descriptor);
		return *descriptor;
	}

	const Image& getImage() const noexcept {
		return image;
	}


	vk::DescriptorSetLayout	getDescriptorSetLayout(ScalingFilter filter) const noexcept
	{
		assert(Math::isInRangeExclusive(filter, ScalingFilter::NONE, ScalingFilter::COUNT));
		return descriptorSetLayouts[static_cast<size_t>(filter)];
	}

	void bind(	vk::CommandBuffer cmd,
				vk::PipelineLayout layout,
				uint32_t index,
				ScalingFilter filter ) const noexcept
	{
		assert(Math::isInRangeExclusive(filter, ScalingFilter::NONE, ScalingFilter::COUNT));
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


private:
	void writeDescriptorSets() {
		for(size_t i = 0; i < descriptorSets.size(); i++){
			//Update all images. As nullptr images can't be passed, repeat available images		
			const auto planes = image.getPlanes();
			std::array<vk::DescriptorImageInfo, frame_SAMPLER_COUNT> images;
			for(size_t i = 0; i < planes.size(); ++i) {
				images[i] = vk::DescriptorImageInfo(
					nullptr,												//Sampler
					planes[i].getImageView(),								//Image views
					vk::ImageLayout::eShaderReadOnlyOptimal					//Layout
				);
			}
			for(size_t i = planes.size(); i < images.size(); ++i) {
				images[i] = vk::DescriptorImageInfo(
					nullptr,												//Sampler
					planes.front().getImageView(),							//Image views
					vk::ImageLayout::eShaderReadOnlyOptimal					//Layout
				);
			}
			static_assert(images.size() == frame_SAMPLER_COUNT, "Sampler count must match");

			//Obtain the sample mode buffer for this sampling mode
			const auto& samplingModeBuffer = createSampleModeBuffer(
				vulkan,
				(*samplers)[i],
				static_cast<ScalingFilter>(i)
			);


			//Create write descriptor
			const std::array sampleModeUniformBuffers = {
				vk::DescriptorBufferInfo(
					samplingModeBuffer.getBuffer(),							//Buffer
					0,														//Offset
					sizeof(int)											//Size
				)
			};

			const std::array colorTransferUniformBuffers = {
				vk::DescriptorBufferInfo(
					colorTransferBuffer->getBuffer(),						//Buffer
					0,														//Offset
					InputColorTransfer::size()								//Size
				)
			};

			const std::array writeDescriptorSets ={
				vk::WriteDescriptorSet( //Image descriptor
					descriptorSets[i],										//Descriptor set
					frame_SAMPLER_BINDING,									//Binding
					0, 														//Index
					images.size(), 											//Descriptor count
					vk::DescriptorType::eCombinedImageSampler,				//Descriptor type
					images.data(), 											//Images
					nullptr, 												//Buffers
					nullptr													//Texel buffers
				),
				vk::WriteDescriptorSet( //Ubo descriptor set
					descriptorSets[i],										//Descriptor set
					frame_SAMPLE_MODE_BINDING,								//Binding
					0, 														//Index
					sampleModeUniformBuffers.size(),						//Descriptor count	
					vk::DescriptorType::eUniformBuffer,						//Descriptor type
					nullptr, 												//Images 
					sampleModeUniformBuffers.data(), 						//Buffers
					nullptr													//Texel buffers
				),
				vk::WriteDescriptorSet( //Ubo descriptor set
					descriptorSets[i],										//Descriptor set
					frame_COLOR_TRANSFER_BINDING,							//Binding
					0, 														//Index
					colorTransferUniformBuffers.size(),						//Descriptor count		
					vk::DescriptorType::eUniformBuffer,						//Descriptor type
					nullptr, 												//Images 
					colorTransferUniformBuffers.data(), 					//Buffers
					nullptr													//Texel buffers
				)
			};

			vulkan.updateDescriptorSets(Utils::BufferView<const vk::WriteDescriptorSet>(writeDescriptorSets));
		}
	}

	static Image createImage(	const Vulkan& vulkan,
								Utils::BufferView<const Image::Plane> planes,
								vk::ImageUsageFlags usage,
								const Samplers& samplers )
	{
		usage |= vk::ImageUsageFlagBits::eSampled;

		constexpr vk::ImageTiling tiling =
			vk::ImageTiling::eOptimal;

		constexpr vk::MemoryPropertyFlags memory =
			vk::MemoryPropertyFlagBits::eDeviceLocal;

		return Image(
			vulkan,
			planes,
			usage,
			tiling,
			memory,
			&samplers.front()
		);
	}

	static vk::UniqueDescriptorPool	createDescriptorPool(const Vulkan& vulkan) {
		//A descriptor pool is created, from which 2 descriptor sets, each one with 
		//one combined image sampler and one uniform buffer. Threrefore, 2 descriptors
		//of each type will be required.
		const std::array poolSizes = {
			vk::DescriptorPoolSize(
				vk::DescriptorType::eCombinedImageSampler,			//Descriptor type
				frame_SAMPLER_COUNT * FILTER_COUNT					//Descriptor count
			),
			vk::DescriptorPoolSize(
				vk::DescriptorType::eUniformBuffer,					//Descriptor type
				2UL*FILTER_COUNT									//Descriptor count
			)
		};

		const vk::DescriptorPoolCreateInfo createInfo(
			{},														//Flags
			FILTER_COUNT,											//Descriptor set count
			poolSizes.size(), poolSizes.data()						//Pool sizes
		);

		return vulkan.createDescriptorPool(createInfo);
	}

	static std::array<vk::DescriptorSetLayout, FILTER_COUNT> createDescriptorSetLayouts(const Vulkan& vulkan,
																						const Samplers& samplers )
	{
		std::array<vk::DescriptorSetLayout, FILTER_COUNT> result;
		assert(samplers.size() == result.size());

		for(size_t i = 0; i < result.size(); ++i) {
			const auto& sampler = samplers[i].getSampler();
			const size_t id = reinterpret_cast<const uintptr_t&>(sampler); //TODO dont use the sampler as id, as it may be repeated somewhere else

			//Try to retrieve the descriptor from cache
			result[i] = vulkan.createDescriptorSetLayout(id);
			if(!result[i]) {
				//No luck, need to create it
				//Create the sampler array
				std::array<vk::Sampler, frame_SAMPLER_COUNT> immutableSamplers;
				for(size_t i = 0; i < immutableSamplers.size(); ++i) {
					immutableSamplers[i] = sampler;
				}
				static_assert(immutableSamplers.size() == frame_SAMPLER_COUNT, "Sampler count does not match");

				//Create the bindings
				const std::array bindings = {
					vk::DescriptorSetLayoutBinding( //Sampled image binding
						frame_SAMPLER_BINDING,							//Binding
						vk::DescriptorType::eCombinedImageSampler,		//Type
						immutableSamplers.size(),						//Count
						vk::ShaderStageFlagBits::eAllGraphics,			//Shader stage
						immutableSamplers.data()						//Immutable samplers
					), 
					vk::DescriptorSetLayoutBinding(	//Sample mode binding
						frame_SAMPLE_MODE_BINDING,						//Binding
						vk::DescriptorType::eUniformBuffer,				//Type
						1,												//Count
						vk::ShaderStageFlagBits::eAllGraphics,			//Shader stage
						nullptr											//Immutable samplers
					), 
					vk::DescriptorSetLayoutBinding(	//Color transfer binding
						frame_COLOR_TRANSFER_BINDING,					//Binding
						vk::DescriptorType::eUniformBuffer,				//Type
						1,												//Count
						vk::ShaderStageFlagBits::eAllGraphics,			//Shader stage
						nullptr											//Immutable samplers
					), 
				};

				const vk::DescriptorSetLayoutCreateInfo createInfo(
					{},
					bindings.size(), bindings.data()
				);

				result[i] = vulkan.createDescriptorSetLayout(id, createInfo);
			}

			//Ensure that all positions are filled
			assert(result[i]);
		}

		return result;
	}

	static std::array<vk::DescriptorSet, FILTER_COUNT> allocateDescriptorSets(	const Vulkan& vulkan,
																				vk::DescriptorPool pool,
																				const std::array<vk::DescriptorSetLayout, FILTER_COUNT>& layouts )
	{
		std::array<vk::DescriptorSet, FILTER_COUNT> result;

		const vk::DescriptorSetAllocateInfo allocInfo(
			pool,													//Pool
			layouts.size(), layouts.data()							//Layouts
		);

		//Allocate descriptor sets
		auto uniqueDescriptorSets = vulkan.allocateDescriptorSets(allocInfo);
		assert(uniqueDescriptorSets.size() == result.size());

		//Copy them into the result buffer releasing them
		std::transform(
			uniqueDescriptorSets.begin(), uniqueDescriptorSets.end(),
			result.begin(),
			std::mem_fn(&vk::UniqueDescriptorSet::release)
		);

		return result;
	}

	static const Buffer& createSampleModeBuffer(const Vulkan& vulkan,
												const Sampler& sampler,
												ScalingFilter filter )
	{
		using Index = std::bitset<2*sizeof(size_t)>;
		static std::unordered_map<Index, const Utils::StaticId> ids; //TODO concurrent access

		//Obtain the desired sample mode
		int32_t sampleMode = InputColorTransfer::getSamplingMode(filter, sampler.getFilter());

		//Create a index for retrieving it from cache
		Index index;
		index |= reinterpret_cast<uintptr_t>(&vulkan);
		index <<= sizeof(uintptr_t) * Utils::getByteSize();
		index |= sampleMode;
		const auto& id = ids[index];

		//Check if the buffer exists in cache
		auto result = std::static_pointer_cast<StagedBuffer>(vulkan.getUserPointer(id));
		if(!result.get()) {
			constexpr auto BUFFER_SIZE = sizeof(int);

			//Create a new staging buffer
			result = Utils::makeShared<StagedBuffer>(
				vulkan,
				vk::BufferUsageFlagBits::eUniformBuffer,
				BUFFER_SIZE
			);

			//Copy the data onto it
			assert(result);
			std::memcpy(
				result->data(),
				&sampleMode,
				BUFFER_SIZE
			);
			result->flushData(
				vulkan, 
				vulkan.getTransferQueueIndex(),
				vk::AccessFlagBits::eShaderRead,
				vk::PipelineStageFlagBits::eAllCommands
			);

			//Store the buffer in cache
			const_cast<Vulkan&>(vulkan).setUserPointer(id, result); //TODO ugly const_cast
		}

		assert(result);
		assert(result.use_count() > 1); //Should be backed somewhere else
		return *result;
	}
};


Frame::Frame(	const Vulkan& vulkan,
				std::shared_ptr<const Descriptor> desc,
				std::shared_ptr<const Buffer> colorTransferBuffer,
				std::shared_ptr<const Samplers> samplers,
				Utils::BufferView<const Image::Plane> planes,
				vk::ImageUsageFlags usage )
	: m_impl({}, vulkan, std::move(desc), std::move(colorTransferBuffer), std::move(samplers), planes, usage)
{
}

Frame::Frame(Frame&& other) noexcept = default;

Frame::~Frame() = default;

Frame& Frame::operator=(Frame&& other) noexcept = default;



const Vulkan& Frame::getVulkan() const noexcept { 
	return m_impl->getVulkan();
}

const Frame::Descriptor& Frame::getDescriptor() const noexcept {
	return m_impl->getDescriptor();
}

const Image& Frame::getImage() const noexcept {
	return m_impl->getImage();
}



vk::DescriptorSetLayout Frame::getDescriptorSetLayout(ScalingFilter filter) const noexcept
{
	return m_impl->getDescriptorSetLayout(filter);
}

void Frame::bind( 	vk::CommandBuffer cmd,
					vk::PipelineLayout layout,
					uint32_t index,
					ScalingFilter filter ) const noexcept
{
	m_impl->bind(cmd, layout, index, filter);
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

std::shared_ptr<Frame::Samplers> Frame::createSamplers(	const Vulkan& vulkan,
														InputColorTransfer& colorTransfer,
														Utils::BufferView<const Image::Plane> planes )
{
	std::shared_ptr<Samplers> result = Utils::makeShared<Samplers>();
	assert(result);
	auto& samplers = *result;

	for(size_t i = 0; i < samplers.size(); ++i) {
		samplers[i] = Sampler(
			vulkan,
			planes.front(),
			colorTransfer,
			static_cast<ScalingFilter>(i)
		);
	}

	colorTransfer.optimize(samplers.front());

	return result;
}

std::vector<Image::Plane> Frame::getPlaneDescriptors(const Descriptor& desc) {
	const Resolution resolution = desc.getResolution();
	const ColorSubsampling subsampling = desc.getColorSubsampling();
	const ColorFormat format = desc.getColorFormat();

	const auto planeCount = getPlaneCount(format);
	std::vector<Image::Plane> result;
	result.reserve(planeCount);

	const auto formats = toVulkan(format);
	const auto extent = toVulkan(resolution);
	const auto subsampledExtent = toVulkan(getSubsampledResolution(subsampling, resolution));

	assert(planeCount <= formats.size());

	for(size_t i = 0; i < planeCount; i++) {
		result.emplace_back(
			to3D((i == 0 || i == 3) ? extent : subsampledExtent),
			std::get<vk::Format>(formats[i]),
			std::get<vk::ComponentMapping>(formats[i])
		);
	}

	return result;
}

}
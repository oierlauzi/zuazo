#include <zuazo/Graphics/Frame.h>

#include <zuazo/Exception.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/Graphics/Sampler.h>

#include <zuazo/shaders/frame.h>

#include <vector>
#include <bitset>

namespace Zuazo::Graphics {

/*
 * Constants
 */
static constexpr auto FILTER_COUNT = static_cast<size_t>(ScalingFilter::count);


/*
 * Frame::Cache
 */

class Frame::Cache {
public:
	Cache(	const Vulkan& vulkan,
			const Image::Plane& plane )
		: m_samplers(createSamplers(vulkan, plane))
		, m_descriptorSetLayouts(createDescriptorSetLayouts(vulkan, m_samplers))
		, m_uniqueDescriptorSetLayouts(m_descriptorSetLayouts)
		, m_uniqueDescriptorSetLayoutCount(getUniqueDescriptorSetLayouts(m_uniqueDescriptorSetLayouts))
		, m_samplingModes(createSamplingModes(m_samplers))
	{
	}

	~Cache() = default;

	const Sampler& getSampler(ScalingFilter filter) const noexcept {
		assert(Math::isInRangeExclusive(filter, ScalingFilter::none, ScalingFilter::count));
		return m_samplers[static_cast<size_t>(filter)];
	}

	vk::DescriptorSetLayout getDescriptorSetLayout(ScalingFilter filter) const noexcept {
		assert(Math::isInRangeExclusive(filter, ScalingFilter::none, ScalingFilter::count));
		return m_descriptorSetLayouts[static_cast<size_t>(filter)];
	}

	Utils::BufferView<const vk::DescriptorSetLayout> getDescriptorSetLayouts() const noexcept {
		return m_descriptorSetLayouts;
	}

	Utils::BufferView<const vk::DescriptorSetLayout> getUniqueDescriptorSetLayouts() const noexcept {
		return Utils::BufferView<const vk::DescriptorSetLayout>(
			m_uniqueDescriptorSetLayouts.data(), 
			m_uniqueDescriptorSetLayoutCount
		);
	}

	uint32_t getSamplingMode(ScalingFilter filter) const noexcept {
		assert(Math::isInRangeExclusive(filter, ScalingFilter::none, ScalingFilter::count));
		return m_samplingModes[static_cast<size_t>(filter)];
	}


private:
	std::array<Sampler, FILTER_COUNT>					m_samplers;
	std::array<vk::DescriptorSetLayout, FILTER_COUNT>	m_descriptorSetLayouts;
	std::array<vk::DescriptorSetLayout, FILTER_COUNT>	m_uniqueDescriptorSetLayouts;
	size_t												m_uniqueDescriptorSetLayoutCount;
	std::array<uint32_t, FILTER_COUNT>					m_samplingModes;

	static std::array<Sampler, FILTER_COUNT> createSamplers(const Vulkan& vulkan,
															const Image::Plane& plane ) 
	{
		std::array<Sampler, FILTER_COUNT> result;

		//Creates a plain old (No YCbCr conversion) shader for each of the filters
		for(size_t i = 0; i < result.size(); ++i) {
			result[i] = Sampler(
				vulkan,
				plane,
				ColorRange::full,
				ColorModel::rgb,
				ColorTransferFunction::linear,
				Math::Vec2<ColorChromaLocation>(ColorChromaLocation::cosited0, ColorChromaLocation::cosited0),
				static_cast<ScalingFilter>(i)
			);
		}

		return result;
	}

	static std::array<vk::DescriptorSetLayout, FILTER_COUNT> createDescriptorSetLayouts(const Vulkan& vulkan,
																						const std::array<Sampler, FILTER_COUNT>& samplers )
	{
		using Index = vk::Sampler;
		static std::unordered_map<Index, const Utils::StaticId> ids; 

		std::array<vk::DescriptorSetLayout, FILTER_COUNT> result;
		assert(samplers.size() == result.size());

		//Create a descriptor set layout for each of the samplers
		for(size_t i = 0; i < result.size(); ++i) {
			const auto sampler = samplers[i].getSampler();
			const auto& id = ids[sampler]; //TODO concurrent access

			//Try to retrieve the descriptor from cache
			result[i] = vulkan.createDescriptorSetLayout(id);
			if(!result[i]) {
				//No luck, need to create it
				//Create the bindings
				const std::array<vk::Sampler, 1> immutableSamplers = {
					sampler
				};
				const std::array bindings = {
					vk::DescriptorSetLayoutBinding( //Sampled image binding
						frame_SAMPLER_BINDING,							//Binding
						vk::DescriptorType::eCombinedImageSampler,		//Type
						immutableSamplers.size(),						//Count
						vk::ShaderStageFlagBits::eAllGraphics,			//Shader stage
						immutableSamplers.data()						//Immutable samplers
					)
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

	static size_t getUniqueDescriptorSetLayouts(std::array<vk::DescriptorSetLayout, FILTER_COUNT>& layouts) {
		std::sort(layouts.begin(), layouts.end());
		const auto ite = std::unique(layouts.begin(), layouts.end());

		const auto count = static_cast<size_t>(std::distance(layouts.begin(), ite));
		assert(count <= layouts.size());
		return count;
	}

	static std::array<uint32_t, FILTER_COUNT> createSamplingModes(const std::array<Sampler, FILTER_COUNT>& samplers) {
		std::array<uint32_t, FILTER_COUNT> result;

		for(size_t i = 0; i < result.size(); ++i) {
			const auto& sampler = samplers[i];

			//Get the sampling mode depending on the actual configuration of the sampler
			//and the desired sampling mode
			uint32_t samplingMode = 0;
			switch (static_cast<ScalingFilter>(i)) {
			case ScalingFilter::cubic:
				switch (sampler.getFilter()) {
				case vk::Filter::eCubicEXT:
					samplingMode = frame_SAMPLE_MODE_PASSTHOUGH;
					break;

				case vk::Filter::eLinear:
					samplingMode = frame_SAMPLE_MODE_BICUBIC_FROM_BILINEAR;
					break;

				case vk::Filter::eNearest:
					samplingMode = frame_SAMPLE_MODE_BICUBIC;
					break;
				
				default:
					assert(false); //Not expected
					break;
				}
				break;

			case ScalingFilter::linear:
				switch (sampler.getFilter()) {
				case vk::Filter::eLinear:
					samplingMode = frame_SAMPLE_MODE_PASSTHOUGH;
					break;

				case vk::Filter::eNearest:
					samplingMode = frame_SAMPLE_MODE_BILINEAR;
					break;
				
				default:
					assert(false); //Not expected
					break;
				}
				break;

			case ScalingFilter::nearest:
				switch (sampler.getFilter()) {
				case vk::Filter::eNearest:
					samplingMode = frame_SAMPLE_MODE_PASSTHOUGH;
					break;
				
				default:
					assert(false); //Not expected
					break;
				}
				break;
			
			default:
				assert(false); //Not expected
				break;
			}
			
			//Write the result
			result[i] = samplingMode;
		}

		return result;
	}


};



/*
 * Frame::Impl
 */

struct Frame::Impl {


	std::reference_wrapper<const Vulkan>				vulkan;
	std::shared_ptr<const Descriptor> 					descriptor;
	std::shared_ptr<const Cache>						cache;
	std::shared_ptr<void>								userPointer;


	Image												image;

	vk::UniqueDescriptorPool							descriptorPool;
	std::array<vk::DescriptorSet, FILTER_COUNT>			descriptorSets;

	Impl(	const Vulkan& vulkan,
			const Image::Plane& plane,
			vk::ImageUsageFlags usage,
			std::shared_ptr<const Descriptor> desc,
			std::shared_ptr<const Cache> c,
			std::shared_ptr<void> usrPtr )
		: vulkan(vulkan)
		, descriptor(std::move(desc))
		, cache(c ? std::move(c) : createCache(vulkan, plane))
		, userPointer(std::move(usrPtr))
		, image(createImage(vulkan, plane, usage, *cache))
		, descriptorPool(createDescriptorPool(vulkan))
		, descriptorSets(allocateDescriptorSets(vulkan, image, *descriptorPool, *cache))
	{
	}

	~Impl() = default;



	const Vulkan& getVulkan() const noexcept { 
		return vulkan;
	}

	const std::shared_ptr<const Descriptor>& getDescriptor() const noexcept {
		return descriptor;
	}

	const std::shared_ptr<const Cache>& getCache() const noexcept {
		return cache;
	}

	const Image& getImage() const noexcept {
		return image;
	}


	vk::DescriptorSetLayout	getDescriptorSetLayout(ScalingFilter filter) const noexcept {
		return cache->getDescriptorSetLayout(filter);
	}

	uint32_t getSamplingMode(ScalingFilter filter) const noexcept {
		return cache->getSamplingMode(filter);
	}

	void bind(	vk::CommandBuffer cmd,
				vk::PipelineLayout layout,
				uint32_t index,
				ScalingFilter filter ) const noexcept
	{
		assert(Math::isInRangeExclusive(filter, ScalingFilter::none, ScalingFilter::count));
		const auto descriptorSet = descriptorSets[static_cast<size_t>(filter)];

		vulkan.get().bindDescriptorSets(
			cmd,								//Commnad buffer
			vk::PipelineBindPoint::eGraphics,	//Pipeline bind point
			layout,								//Pipeline layout
			index,								//First index
			descriptorSet,						//Descriptor sets
			{}									//Dynamic offsets
		);
	}


	void setUserPointer(std::shared_ptr<void> usrPtr) {
		userPointer = std::move(usrPtr);
	}

	void* getUserPointer() const noexcept {
		return userPointer.get();
	}


	static std::shared_ptr<const Cache> createCache(	const Vulkan& vulkan,
												const Image::Plane& plane )
	{
		return Utils::makeShared<const Cache>(vulkan, plane);
	}



private:
	static Image createImage(	const Vulkan& vulkan,
								const Image::Plane& plane,
								vk::ImageUsageFlags usage,
								const Cache& cache )
	{
		//Ensure that the sampled bit is set
		usage |= vk::ImageUsageFlagBits::eSampled;

		constexpr vk::ImageTiling tiling =
			vk::ImageTiling::eOptimal;

		constexpr vk::MemoryPropertyFlags memory =
			vk::MemoryPropertyFlagBits::eDeviceLocal;

		return Image(
			vulkan,
			plane,
			usage,
			tiling,
			memory,
			&cache.getSampler(ScalingFilter::nearest) //Actually not needed, just for convenience
		);
	}

	static vk::UniqueDescriptorPool	createDescriptorPool(const Vulkan& vulkan) {
		//A descriptor pool is created, from which at most, 3 descriptor sets will
		//be allocated. Each one of those descriptor sets will have one combined
		//image sampler. Therefore, 3 descriptors of each type will be required.
		constexpr std::array<vk::DescriptorPoolSize, 1> poolSizes = {
			vk::DescriptorPoolSize(
				vk::DescriptorType::eCombinedImageSampler,			//Descriptor type
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


	static std::array<vk::DescriptorSet, FILTER_COUNT> allocateDescriptorSets(	const Vulkan& vulkan,
																				const Image& image,
																				vk::DescriptorPool pool,
																				const Cache& cache )
	{
		std::array<vk::DescriptorSet, FILTER_COUNT> result;

		const auto layouts = cache.getDescriptorSetLayouts();
		const auto uniqueLayouts = cache.getUniqueDescriptorSetLayouts();
		assert(layouts.size() == result.size());

		//Allocate descriptor sets for those unique layouts
		const vk::DescriptorSetAllocateInfo allocInfo(
			pool,													//Pool
			uniqueLayouts.size(), uniqueLayouts.data()				//Layouts
		);
		auto uniqueDescriptorSets = vulkan.allocateDescriptorSets(allocInfo);
		assert(uniqueDescriptorSets.size() == uniqueLayouts.size());

		//Fill the result array
		std::transform(
			layouts.cbegin(), layouts.cend(),
			result.begin(),
			[&uniqueLayouts, &uniqueDescriptorSets] (vk::DescriptorSetLayout layout) -> vk::DescriptorSet {
				//Find the corresponding unique layout's index
				const auto uniqueLayoutIte = std::find(uniqueLayouts.cbegin(), uniqueLayouts.cend(), layout);
				const auto uniqueLayoutIdx = static_cast<size_t>(std::distance(uniqueLayouts.cbegin(), uniqueLayoutIte));
				assert(uniqueLayoutIdx < uniqueLayouts.size()); //Must be found

				//The result will be the descriptor set with the same index as the unique layout
				return uniqueDescriptorSets[uniqueLayoutIdx].get();
			}
		);

		//Write the image to each of the *unique* descriptor sets
		const auto planes = image.getPlanes();
		assert(planes.size() == 1);
		const std::array<vk::DescriptorImageInfo, 1> descriptorImageInfos = {
			vk::DescriptorImageInfo(
				nullptr,												//Sampler (already set)
				planes.front().getImageView(),							//Image view
				vk::ImageLayout::eShaderReadOnlyOptimal					//Layout
			)
		};
		std::for_each(
			uniqueDescriptorSets.cbegin(), uniqueDescriptorSets.cend(),
			[&descriptorImageInfos, &vulkan] (const vk::UniqueDescriptorSet& descSet) {
				//Create write descriptor
				const std::array<vk::WriteDescriptorSet, 1> writeDescriptorSets = {
					vk::WriteDescriptorSet( //Image descriptor
						*descSet,												//Descriptor set
						frame_SAMPLER_BINDING,									//Binding
						0, 														//Index
						descriptorImageInfos.size(), 							//Descriptor count
						vk::DescriptorType::eCombinedImageSampler,				//Descriptor type
						descriptorImageInfos.data(), 							//Images
						nullptr, 												//Buffers
						nullptr													//Texel buffers
					)
				};

				vulkan.updateDescriptorSets(Utils::BufferView<const vk::WriteDescriptorSet>(writeDescriptorSets));
			}
		);

		//Release all the descriptor sets, as they will be freed with the descriptor pool
		std::for_each(
			uniqueDescriptorSets.begin(), uniqueDescriptorSets.end(),
			std::mem_fn(&vk::UniqueDescriptorSet::release)
		);

		return result;
	}

};



/*
 * Frame
 */

Frame::Frame(	const Vulkan& vulkan,
				const Image::Plane& plane,
				vk::ImageUsageFlags usage,
				std::shared_ptr<const Descriptor> desc,
				std::shared_ptr<const Cache> cache,
				std::shared_ptr<void> usrPtr )
	: m_impl({}, vulkan, plane, usage, std::move(desc), std::move(cache), std::move(usrPtr))
{
}

Frame::Frame(Frame&& other) noexcept = default;

Frame::~Frame() = default;

Frame& Frame::operator=(Frame&& other) noexcept = default;



const Vulkan& Frame::getVulkan() const noexcept { 
	return m_impl->getVulkan();
}

const std::shared_ptr<const Frame::Descriptor>& Frame::getDescriptor() const noexcept {
	return m_impl->getDescriptor();
}

const std::shared_ptr<const Frame::Cache>& Frame::getCache() const noexcept {
	return m_impl->getCache();
}

const Image& Frame::getImage() const noexcept {
	return m_impl->getImage();
}



vk::DescriptorSetLayout Frame::getDescriptorSetLayout(ScalingFilter filter) const noexcept
{
	return m_impl->getDescriptorSetLayout(filter);
}

uint32_t Frame::getSamplingMode(ScalingFilter filter) const noexcept {
	return m_impl->getSamplingMode(filter);
}

void Frame::bind( 	vk::CommandBuffer cmd,
					vk::PipelineLayout layout,
					uint32_t index,
					ScalingFilter filter ) const noexcept
{
	m_impl->bind(cmd, layout, index, filter);
}


void Frame::setUserPointer(std::shared_ptr<void> usrPtr) {
	m_impl->setUserPointer(std::move(usrPtr));
}

void* Frame::getUserPointer() const noexcept {
	return m_impl->getUserPointer();
}


std::shared_ptr<const Frame::Cache> Frame::createCache(	const Vulkan& vulkan,
														const Image::Plane& plane )
{
	return Impl::createCache(vulkan, plane);
}

}
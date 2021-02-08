#include <zuazo/Graphics/UniformBuffer.h>

#include <zuazo/Graphics/VulkanConversions.h>

#include <tuple>

namespace Zuazo::Graphics {

UniformBuffer::UniformBuffer(	const Vulkan& vulkan,
								Utils::BufferView<const std::pair<uint32_t, size_t>> bindings )
	: m_uniformBufferLayout(createUniformBufferLayout(vulkan, bindings))
	, m_uniformBuffer(createUniformBuffer(vulkan, m_uniformBufferLayout))
	, m_uniformFlushArea(0, 0)
	, m_uniformFlushStages(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment) //TODO decide based on each write
{
}

Utils::BufferView<const std::byte> UniformBuffer::getBindingData(uint32_t binding) const {
	Utils::BufferView<const std::byte> result;

	const auto ite = m_uniformBufferLayout.find(binding);
	if(ite != m_uniformBufferLayout.cend()) {
		const auto buffer = m_uniformBuffer.getData();
		result = ite->second.apply(buffer);
		assert(result.cend() <= buffer.cend());
	}

	return result;
}

void UniformBuffer::writeDescirptorSet(	const Vulkan& vulkan,
										vk::DescriptorSet descriptorSet ) const 
{
	//Create an array with all the buffer infos. A single one
	//will be used for each descriptor
	std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
	descriptorBufferInfos.reserve(m_uniformBufferLayout.size());
	for(const auto& binding : m_uniformBufferLayout) {
		descriptorBufferInfos.emplace_back(
			m_uniformBuffer.getBuffer(),
			binding.second.offset(),
			binding.second.size()
		);
	}

	//Create an array of write infos for it
	std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
	writeDescriptorSets.reserve(descriptorBufferInfos.size());
	auto ite = m_uniformBufferLayout.cbegin();
	for(const auto& descriptorBufferInfo : descriptorBufferInfos) {
		assert(ite != m_uniformBufferLayout.cend());

		writeDescriptorSets.emplace_back(
			descriptorSet,						//Descriptor set
			ite->first,							//Binding
			0,									//Index
			1,									//Descriptor count
			vk::DescriptorType::eUniformBuffer,	//Descriptor type
			nullptr,							//Image descriptors
			&descriptorBufferInfo,				//Uniform buffer descriptors
			nullptr								//Texel buffers
		);

		++ite;
	}

	if(writeDescriptorSets.size()) {
		vulkan.updateDescriptorSets(writeDescriptorSets, {});
	}
}

void UniformBuffer::writeDescirptorSet(	const Vulkan& vulkan,
										vk::DescriptorSet descriptorSet,
										Utils::BufferView<const std::pair<uint32_t, uint32_t>> bindingRemapping ) const
{
	//Create an array with all the buffer infos. A single one
	//will be used for each descriptor
	std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
	descriptorBufferInfos.reserve(bindingRemapping.size()); //Worst case scenario
	for(const auto& remapping : bindingRemapping) {
		//Try to find the source binding in the layout
		const auto ite = m_uniformBufferLayout.find(remapping.first);

		if(ite != m_uniformBufferLayout.cend()) {
			descriptorBufferInfos.emplace_back(
				m_uniformBuffer.getBuffer(),
				ite->second.offset(),
				ite->second.size()
			);
		} else {
			//Insert an empty struct in order to have the same size as bindingRemapping
			//and index togetherin the next step
			descriptorBufferInfos.emplace_back();
		}
	}

	//Create an array of write infos for it
	std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
	writeDescriptorSets.reserve(descriptorBufferInfos.size()); //Worst case scenario
	for(size_t i = 0; i < descriptorBufferInfos.size(); ++i) {
		//Only create the write info if the descriptorBufferInfo is valid
		if(descriptorBufferInfos[i].buffer) {
			writeDescriptorSets.emplace_back(
				descriptorSet,						//Descriptor set
				bindingRemapping[i].second,			//Binding
				0,									//Index
				1,									//Descriptor count
				vk::DescriptorType::eUniformBuffer,	//Descriptor type
				nullptr,							//Image descriptors
				&descriptorBufferInfos[i],			//Uniform buffer descriptors
				nullptr								//Texel buffers
			);
		}
	}

	if(writeDescriptorSets.size()) {
		vulkan.updateDescriptorSets(writeDescriptorSets, {});
	}
}

void UniformBuffer::write(	const Vulkan& vulkan,
							uint32_t binding,
							const void* data,
							size_t size,
							size_t offset )
{
	//Any previous upload should have ended
	assert(waitCompletion(vulkan, 0));

	const auto ite = m_uniformBufferLayout.find(binding);
	if(ite != m_uniformBufferLayout.cend()) {
		//Sanitize the offset and size
		offset = Math::min(offset, ite->second.size());
		size = Math::min(size, ite->second.size() - offset); //Can't underflow: offset <= size

		//Translate the offset to the real placement
		offset += ite->second.offset();

		//Create a area object for the calculated size
		const Utils::Area writeArea(offset, size);
		assert(writeArea.begin() >= ite->second.begin());
		assert(writeArea.end() <= ite->second.end());

		//Write data onto the buffer
		const auto buffer = writeArea.apply(m_uniformBuffer.getData());
		std::memcpy(buffer.data(), data, buffer.size());

		//Signal flushing
		m_uniformFlushArea |= writeArea;
		//m_uniformFlushStages |= stages; //TODO
	}
}

void UniformBuffer::flush(const Vulkan& vulkan) {
	if(m_uniformFlushArea.size() > 0) {
		//Flush the unform buffer
		m_uniformBuffer.flushData(
			vulkan,
			m_uniformFlushArea,
			vulkan.getGraphicsQueueIndex(),
			vk::AccessFlagBits::eUniformRead,
			toPipelineStageFlags(m_uniformFlushStages)
		);
	}

	//Clear the flags
	m_uniformFlushArea = {};
	//m_uniformFlushStages = {}; //TODO
}

bool UniformBuffer::waitCompletion(	const Vulkan& vulkan,
									uint64_t timeout ) const 
{
	return m_uniformBuffer.waitCompletion(vulkan, timeout);
}


std::map<uint32_t, Utils::Area> UniformBuffer::createUniformBufferLayout(	const Vulkan& vulkan,
																			Utils::BufferView<const std::pair<uint32_t, size_t>> bindings )
{
	std::map<uint32_t, Utils::Area> result;

	//Insert all bindings without an area assigned
	for(const auto& binding : bindings) {
		result.emplace(binding.first, Utils::Area(0, binding.second));
	}

	//Assign an area to each binding. The first one already assigned
	//Size is checked as if the result is empty, std::next(cend())
	//is called
	if(result.size() > 1) {
		const auto& limits = vulkan.getPhysicalDeviceProperties().limits;
		for(auto ite = std::next(result.begin()); ite != result.end(); ++ite) {
			//Align the offset based on the end of the previous element
			const auto offset = Utils::align(
				std::prev(ite)->second.end(),
				limits.minUniformBufferOffsetAlignment
			);

			//Write it
			ite->second = Utils::Area(offset, ite->second.size());
		}
	}

	return result;
}

StagedBuffer UniformBuffer::createUniformBuffer(const Vulkan& vulkan,
												const std::map<uint32_t, Utils::Area>& layout )
{
	return Graphics::StagedBuffer(
		vulkan,
		vk::BufferUsageFlagBits::eUniformBuffer,
		layout.rbegin()->second.end()
	);
}

}
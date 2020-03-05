#pragma once

#include "Vulkan.h"
#include "ColorTransfer.h"
#include "Image.h"
#include "Buffer.h"
#include "MappedMemory.h"
#include "../Resolution.h"

#include <memory>
#include <span>

namespace Zuazo::Graphics {

class Frame {
public:	
	using PixelData = std::array<std::span<std::byte>, IMAGE_COUNT>;
	class Geometry;

	Frame(	const Vulkan& vulkan,
			Image&& image,
			std::shared_ptr<const Buffer>&& colorTransfer );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	virtual ~Frame();

	Frame& 							operator=(const Frame& other) = delete;

	vk::Fence&						getReadyFence();
	const vk::Fence&				getReadyFence() const;

	Image&							getImage();
	const Image&					getImage() const;

	const Buffer&					getColorTransfer() const;

	void							bind(	vk::CommandBuffer cmd,
											vk::PipelineBindPoint bindPoint,
											vk::PipelineLayout layout,
											uint32_t index,
											vk::Filter filter );

	static std::shared_ptr<Buffer> 	createColorTransferBuffer(	const Vulkan& vulkan,
																const ColorTransfer& colorTransfer );

protected:
	const Vulkan&					m_vulkan;
	
private:
	static constexpr size_t DESCRIPTOR_COUNT = VK_FILTER_RANGE_SIZE;
	using DescriptorSets = std::array<vk::DescriptorSet, DESCRIPTOR_COUNT>;

	vk::UniqueFence					m_readyFence;

	Image							m_image;
	std::shared_ptr<const Buffer>	m_colorTransfer;

	vk::UniqueDescriptorPool		m_descriptorPool;
	DescriptorSets					m_descriptorSets;

	vk::UniqueDescriptorPool		createDescriptorPool(const Vulkan& vulkan);
	DescriptorSets					allocateDescriptorSets(	const Vulkan& vulkan,
															vk::DescriptorPool pool );
};

class Frame::Geometry {
public:
	enum class ScalingMode {
		STRETCH,
		BOXED,
		CROPPED,
		CLAMP_HORIZONTALLY,
		CLAMP_VERTICALLY
	};

	Geometry(const Vulkan& vulkan, const Resolution& resolution);
	Geometry(const Geometry& other) = delete;
	Geometry(Geometry&& other) = default;
	~Geometry() = default;

	static vk::VertexInputBindingDescription getBindingDescription(uint32_t binding);
	static vk::VertexInputAttributeDescription getPositionAttributeDescription(uint32_t binding, uint32_t location);
	static vk::VertexInputAttributeDescription getTexCoordAttributeDescription(uint32_t binding, uint32_t location);
	
private:
	struct Vertex {
		glm::vec2 position;
		glm::vec2 texCoord;
	};

	static constexpr size_t VERTEX_COUNT = 4;
	static constexpr size_t BUFFER_SIZE = sizeof(Vertex) * VERTEX_COUNT;

	const Vulkan&					m_vulkan;

	Resolution 						m_srcResolution;
	Resolution						m_dstResolution;

	Buffer							m_vertexBuffer;
	Buffer							m_stagingBuffer;
	MappedMemory					m_stagingBufferMapping;

	vk::UniqueCommandPool			m_commandPool;
	vk::CommandBuffer				m_uploadCommand;
	vk::SubmitInfo 					m_uploadCommandSubmit;

	void							updateVertexBuffer();
	void							calculateVertices(const std::span<Vertex, VERTEX_COUNT>& vertices) const;

	static Buffer					createVertexBuffer(const Vulkan& vulkan);
	static Buffer					createStagingBuffer(const Vulkan& vulkan);
	static MappedMemory				mapStagingBuffer(	const Vulkan& vulkan, 
														const Buffer& stagingBuffer );
	static vk::UniqueCommandPool	createCommandPool(const Vulkan& vulkan);
	static vk::CommandBuffer		createCommandBuffer(const Vulkan& vulkan,
														const vk::CommandPool& cmdPool,
														const Buffer& stagingBuffer,
														const Buffer& vertexBuffer );
	static vk::SubmitInfo			createSubmitInfo(const vk::CommandBuffer& cmdBuffer);
};

}
#pragma once

#include "Vulkan.h"
#include "ColorTransfer.h"
#include "Image.h"
#include "Buffer.h"
#include "../Resolution.h"
#include "../Utils/BufferView.h"

#include <memory>

namespace Zuazo::Graphics {

class Frame {
public:	
	using PixelData = std::array<Utils::BufferView<std::byte>, IMAGE_COUNT>;

	struct Descriptor {
		using Extents = std::array<vk::Extent2D, IMAGE_COUNT>;
		using Formats = std::array<std::tuple<vk::Format, vk::ComponentMapping>, IMAGE_COUNT>;

		Extents extents;
		Formats colorFormat;
		ColorTransfer colorTransfer;
	};

	class Geometry;

	Frame(	const Vulkan& vulkan,
			const Descriptor& desc,
			const std::shared_ptr<const Buffer>& colorTransfer );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	virtual ~Frame();

	Frame& 							operator=(const Frame& other) = delete;

	void							bind(	vk::CommandBuffer cmd,
											vk::PipelineLayout layout,
											uint32_t index,
											vk::Filter filter );

	const Descriptor&				getDescriptor() const;
	const Image&					getImage() const;
	const Buffer&					getColorTransfer() const;
	const vk::Fence&				getReadyFence() const;

	static std::shared_ptr<Buffer> 	createColorTransferBuffer(	const Vulkan& vulkan,
																const ColorTransfer& colorTransfer );

	static vk::DescriptorSetLayout	getDescriptorSetLayout(	const Vulkan& vulkan,
															vk::Filter filt );

protected:
	const Vulkan&					m_vulkan;
	
private:
	static constexpr size_t DESCRIPTOR_COUNT = VK_FILTER_RANGE_SIZE;
	using DescriptorSets = std::array<vk::DescriptorSet, DESCRIPTOR_COUNT>;

	Descriptor						m_descriptor;
	Image							m_image;
	std::shared_ptr<const Buffer>	m_colorTransfer;

	vk::UniqueDescriptorPool		m_descriptorPool;
	DescriptorSets					m_descriptorSets;

	vk::UniqueFence					m_readyFence;

	static Image					createImage(const Vulkan& vulkan,
												const Descriptor& desc);
	static vk::UniqueDescriptorPool	createDescriptorPool(const Vulkan& vulkan);
	static DescriptorSets			allocateDescriptorSets(	const Vulkan& vulkan,
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

	static constexpr size_t ATTRIBUTE_COUNT = 2;

	Geometry(	const Vulkan& vulkan, 
				uint32_t binding, 
				const Resolution& resolution);
	Geometry(const Geometry& other) = delete;
	Geometry(Geometry&& other) = default;
	~Geometry() = default;

	void					upd() { updateVertexBuffer(); } //TODO testing

	vk::VertexInputBindingDescription getBindingDescription() const;
	std::array<vk::VertexInputAttributeDescription, ATTRIBUTE_COUNT> getAttributeDescriptions(	uint32_t posLocation, 
																								uint32_t texLocation ) const;

	void bind(vk::CommandBuffer cmd) const;
	
private:
	struct Vertex {
		glm::vec2 position;
		glm::vec2 texCoord;
	};

	static constexpr size_t VERTEX_COUNT = 4;
	static constexpr size_t BUFFER_SIZE = sizeof(Vertex) * VERTEX_COUNT;

	const Vulkan&					m_vulkan;

	uint32_t						m_binding;

	Resolution 						m_srcResolution;
	Resolution						m_dstResolution;

	Buffer							m_vertexBuffer;
	Buffer							m_stagingBuffer;
	Utils::BufferView<Vertex>		m_vertices;

	vk::UniqueCommandPool			m_commandPool;
	vk::CommandBuffer				m_uploadCommand;
	vk::SubmitInfo 					m_uploadCommandSubmit;

	void							updateVertexBuffer();

	static Buffer					createVertexBuffer(const Vulkan& vulkan);
	static Buffer					createStagingBuffer(const Vulkan& vulkan);
	static Utils::BufferView<Vertex> mapStagingBuffer(	const Vulkan& vulkan, 
														const Buffer& stagingBuffer );
	static vk::UniqueCommandPool	createCommandPool(const Vulkan& vulkan);
	static vk::CommandBuffer		createCommandBuffer(const Vulkan& vulkan,
														vk::CommandPool cmdPool,
														const Buffer& stagingBuffer,
														const Buffer& vertexBuffer );
	static vk::SubmitInfo			createSubmitInfo(const vk::CommandBuffer& cmdBuffer);
};

}
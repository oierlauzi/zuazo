#pragma once

#include "Vulkan.h"
#include "VulkanConversions.h"
#include "ColorTransfer.h"
#include "Image.h"
#include "Buffer.h"
#include "../Resolution.h"
#include "../Utils/BufferView.h"
#include "../Math/Vector.h"

#include <memory>
#include <array>

namespace Zuazo::Graphics {

class Frame {
public:
	class Geometry;

	Frame(	const Vulkan& vulkan,
			Math::Vec2f size,
			Utils::BufferView<const Image::PlaneDescriptor> planes,
			const std::shared_ptr<const Buffer>& colorTransfer );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) = default;
	virtual ~Frame() = default;

	Frame& 							operator=(const Frame& other) = delete;

	void							bind(	vk::CommandBuffer cmd,
											vk::PipelineLayout layout,
											uint32_t index,
											vk::Filter filter );

	const Math::Vec2f&				getSize() const;
	const Image&					getImage() const;
	const Buffer&					getColorTransfer() const;
	const vk::Fence&				getReadyFence() const;

	static Math::Vec2f				getFrameSize(Resolution resolution, AspectRatio par);

	static std::shared_ptr<Buffer> 	createColorTransferBuffer(	const Vulkan& vulkan,
																const ColorTransfer& colorTransfer );

	static vk::DescriptorSetLayout	getDescriptorSetLayout(	const Vulkan& vulkan,
															vk::Filter filt );

protected:
	const Vulkan&					m_vulkan;
	
private:
	static constexpr size_t DESCRIPTOR_COUNT = VK_FILTER_RANGE_SIZE;
	using DescriptorSets = std::array<vk::DescriptorSet, DESCRIPTOR_COUNT>;

	Math::Vec2f						m_size;
	Image							m_image;
	std::shared_ptr<const Buffer>	m_colorTransfer;

	vk::UniqueDescriptorPool		m_descriptorPool;
	DescriptorSets					m_descriptorSets;

	vk::UniqueFence					m_readyFence;

	static Image					createImage(const Vulkan& vulkan,
												Utils::BufferView<const Image::PlaneDescriptor> planes );
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
				Math::Vec2f targetExtent);
	Geometry(const Geometry& other) = delete;
	Geometry(Geometry&& other) = default;
	~Geometry() = default;

	void							setTargetSize(Math::Vec2f res);
	const Math::Vec2f&				getTargetSize() const;

	void							setSourceSize(Math::Vec2f res);
	const Math::Vec2f&				getSourceSize() const;

	vk::VertexInputBindingDescription getBindingDescription() const;
	std::array<vk::VertexInputAttributeDescription, ATTRIBUTE_COUNT> getAttributeDescriptions(	uint32_t posLocation, 
																								uint32_t texLocation ) const;

	void							useFrame(const Frame& frame);
	void 							bind(vk::CommandBuffer cmd) const;
	
private:
	struct Vertex {
		glm::vec2 position;
		glm::vec2 texCoord;
	};

	static constexpr size_t VERTEX_COUNT = 4;
	static constexpr size_t BUFFER_SIZE = sizeof(Vertex) * VERTEX_COUNT;

	const Vulkan&					m_vulkan;

	uint32_t						m_binding;

	Math::Vec2f						m_targetSize;
	Math::Vec2f 					m_sourceSize;

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
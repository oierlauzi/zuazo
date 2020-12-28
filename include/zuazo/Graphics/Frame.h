#pragma once

#include "Vulkan.h"
#include "Buffer.h"
#include "StagedBuffer.h"
#include "VulkanConversions.h"
#include "../ScalingMode.h"
#include "../Utils/BufferView.h"
#include "../Utils/Pimpl.h"
#include "../Math/Vector.h"
#include "../Resolution.h"
#include "../ColorSubsampling.h"
#include "../ColorFormat.h"
#include "../ColorRange.h"
#include "../ColorTransferFunction.h"
#include "../ColorModel.h"
#include "../ColorPrimaries.h"

#include <vector>
#include <memory>
#include <array>
#include <cstddef>

namespace Zuazo::Graphics {

class InputColorTransfer;

class Frame {
public:
	struct PlaneDescriptor {
		vk::Extent2D 							extent;
		vk::Format 								format;
		vk::ComponentMapping 					swizzle;
	};

	struct SamplerDescriptor {
		vk::Filter								filter;
		int32_t									samplingMode;
	};

	class Descriptor;
	class Geometry;

	static constexpr size_t FILTER_COUNT = VK_FILTER_LINEAR - VK_FILTER_NEAREST + 1;

	Frame(	const Vulkan& vulkan,
			std::shared_ptr<const Descriptor> desc,
			std::shared_ptr<const Buffer> colorTransfer,
			Utils::BufferView<const PlaneDescriptor> planes,
			vk::ImageUsageFlags usage );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) noexcept;
	virtual ~Frame();

	Frame& 									operator=(const Frame& other) = delete;
	Frame&									operator=(Frame&& other) noexcept;

	void									bind(	vk::CommandBuffer cmd,
													vk::PipelineLayout layout,
													uint32_t index,
													vk::Filter filter ) const noexcept;

	const Vulkan&							getVulkan() const noexcept;
	const Descriptor&						getDescriptor() const noexcept;
	const Math::Vec2f&						getSize() const noexcept;

	const std::vector<vk::UniqueImage>&		getImages() const noexcept;
	const std::vector<vk::UniqueImageView>&	getImageViews() const noexcept;
	const std::vector<Utils::Area>&			getPlaneAreas() const noexcept;
	const vk::DeviceMemory&					getMemory() const noexcept;

	static std::shared_ptr<StagedBuffer>	createColorTransferBuffer(	const Vulkan& vulkan,
																		const InputColorTransfer& colorTransfer );
	static std::vector<PlaneDescriptor>		getPlaneDescriptors(const Descriptor& desc);
	static SamplerDescriptor				getSamplerDescriptor(ScalingFilter filter);
	static vk::DescriptorSetLayout			getDescriptorSetLayout(	const Vulkan& vulkan,
																	vk::Filter filt );

private:
	struct Impl;
	Utils::Pimpl<Impl>						m_impl;

};


class Frame::Descriptor {
public:
	Descriptor(	Resolution resolution, 
				AspectRatio pixelAspectRatio, 
				ColorPrimaries colorPrimaries, 
				ColorModel colorModel, 
				ColorTransferFunction colorTransferFunction, 
				ColorSubsampling colorSubsampling, 
				ColorRange colorRange, 
				ColorFormat colorFormat ) noexcept;
	Descriptor(const Descriptor& other) noexcept = default;
	~Descriptor() = default;

	Descriptor&								operator=(const Descriptor& other) noexcept = default;

	void									setResolution(Resolution resolution) noexcept;
	Resolution								getResolution() const noexcept;

	void									setPixelAspectRatio(AspectRatio pixelAspectRatio) noexcept;
	AspectRatio								getPixelAspectRatio() const noexcept;
	
	void									setColorPrimaries(ColorPrimaries colorPrimaries) noexcept;
	ColorPrimaries							getColorPrimaries() const noexcept;

	void									setColorModel(ColorModel colorModel) noexcept;
	ColorModel								getColorModel() const noexcept;

	void									setColorTransferFunction(ColorTransferFunction colorTransferFunction) noexcept;
	ColorTransferFunction					getColorTransferFunction() const noexcept;

	void									setColorSubsampling(ColorSubsampling colorSubsampling) noexcept;
	ColorSubsampling						getColorSubsampling() const noexcept;

	void									setColorRange(ColorRange colorRange) noexcept;
	ColorRange								getColorRange() const noexcept;

	void									setColorFormat(ColorFormat colorFormat) noexcept;
	ColorFormat								getColorFormat() const noexcept;

	Math::Vec2f								calculateSize() const noexcept;

private:
	Resolution								m_resolution;
	AspectRatio								m_pixelAspectRatio;
	ColorPrimaries							m_colorPrimaries;
	ColorModel								m_colorModel;
	ColorTransferFunction					m_colorTransferFunction;
	ColorSubsampling						m_colorSubsampling;
	ColorRange								m_colorRange;
	ColorFormat								m_colorFormat;

};


class Frame::Geometry {
public:
	Geometry(	std::byte* data,
				size_t stride,
				size_t positionOffset,
				size_t texCoordOffset,
				ScalingMode scaling = ScalingMode::STRETCH,
				Math::Vec2f targetSize = Math::Vec2f() ) noexcept;
	Geometry(const Geometry& other) noexcept = default;
	~Geometry() = default;

	Geometry&								operator=(const Geometry& other) noexcept = default;

	void									setScalingMode(ScalingMode scaling) noexcept;
	ScalingMode								getScalingMode() const noexcept;

	void									setTargetSize(Math::Vec2f size) noexcept;
	const Math::Vec2f&						getTargetSize() const noexcept;

	bool									useFrame(const Frame& frame);
	
	static constexpr size_t					VERTEX_COUNT = 4;

private:
	std::byte*								m_data;
	size_t									m_stride;
	size_t									m_positionOffset;
	size_t									m_texCoordOffset;

	ScalingMode								m_scalingMode;
	Math::Vec2f								m_targetSize;
	Math::Vec2f 							m_sourceSize;

	void									updateBuffer();
};

}
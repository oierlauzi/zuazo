#pragma once

#include "Vulkan.h"
#include "Image.h"
#include "VulkanConversions.h"
#include "../ScalingMode.h"
#include "../Utils/BufferView.h"
#include "../Utils/Pimpl.h"
#include "../Math/Vector.h"
#include "../Resolution.h"
#include "../ColorSubsampling.h"
#include "../ColorChromaLocation.h"
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

class Frame {
public:
	class Descriptor;
	class Geometry;
	class Cache;

	Frame(	const Vulkan& vulkan,
			const Image::Plane& plane,
			vk::ImageUsageFlags usage,
			std::shared_ptr<const Descriptor> desc,
			std::shared_ptr<const Cache> cache = nullptr,
			std::shared_ptr<void> usrPtr = nullptr );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other) noexcept;
	virtual ~Frame();

	Frame& 									operator=(const Frame& other) = delete;
	Frame&									operator=(Frame&& other) noexcept;													

	const Vulkan&							getVulkan() const noexcept;
	const std::shared_ptr<const Descriptor>&getDescriptor() const noexcept;
	const std::shared_ptr<const Cache>&		getCache() const noexcept;
	const Image&							getImage() const noexcept;

	vk::DescriptorSetLayout					getDescriptorSetLayout(ScalingFilter filter) const noexcept;
	uint32_t								getSamplingMode(ScalingFilter filter) const noexcept;
	void									bind(	vk::CommandBuffer cmd,
													vk::PipelineLayout layout,
													uint32_t index,
													ScalingFilter filter ) const noexcept;

	void									setUserPointer(std::shared_ptr<void> usrPtr);
	void*									getUserPointer() const noexcept;

	static std::shared_ptr<const Cache>		createCache(const Vulkan& vulkan,
														const Image::Plane& plane );

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
				Math::Vec2<ColorChromaLocation> colorChromaLocation,
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

	void									setColorChromaLocation(Math::Vec2<ColorChromaLocation> colorChromaLocation) noexcept;
	Math::Vec2<ColorChromaLocation>			getColorChromaLocation() const noexcept;

	void									setColorRange(ColorRange colorRange) noexcept;
	ColorRange								getColorRange() const noexcept;

	void									setColorFormat(ColorFormat colorFormat) noexcept;
	ColorFormat								getColorFormat() const noexcept;

	Math::Vec2f								calculateSize() const noexcept;
	std::vector<Image::Plane>				getPlanes() const;

private:
	Resolution								m_resolution;
	AspectRatio								m_pixelAspectRatio;
	ColorPrimaries							m_colorPrimaries;
	ColorModel								m_colorModel;
	ColorTransferFunction					m_colorTransferFunction;
	ColorSubsampling						m_colorSubsampling;
	Math::Vec2<ColorChromaLocation>			m_colorChromaLocation;
	ColorRange								m_colorRange;
	ColorFormat								m_colorFormat;

};


class Frame::Geometry {
public:
	Geometry(	ScalingMode scaling = ScalingMode::stretch,
				Math::Vec2f targetSize = Math::Vec2f() ) noexcept;
	Geometry(const Geometry& other) noexcept = default;
	~Geometry() = default;

	Geometry&								operator=(const Geometry& other) noexcept = default;

	void									setScalingMode(ScalingMode scaling) noexcept;
	ScalingMode								getScalingMode() const noexcept;

	void									setTargetSize(Math::Vec2f size) noexcept;
	const Math::Vec2f&						getTargetSize() const noexcept;

	std::pair<Math::Vec2f, Math::Vec2f>		calculateSurfaceSize() const noexcept;

	bool									useFrame(const Frame& frame);
	void									writeQuadVertices(	Math::Vec2f* position,
																Math::Vec2f* texCoord,
																size_t positionStride = sizeof(Math::Vec2f),
																size_t texCoordStride = sizeof(Math::Vec2f) ) const noexcept;
	
	static constexpr size_t					VERTEX_COUNT = 4;

private:
	ScalingMode								m_scalingMode;
	Math::Vec2f								m_targetSize;
	Math::Vec2f 							m_sourceSize;

};

}
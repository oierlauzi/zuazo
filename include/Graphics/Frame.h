#pragma once

#include "Vulkan.h"
#include "VulkanConversions.h"
#include "Buffer.h"
#include "../ScalingMode.h"
#include "../Utils/BufferView.h"
#include "../Utils/Pimpl.h"
#include "../Math/Vector.h"
#include "../Utils/Configuration.h"
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

class ColorTransfer;

class Frame {
public:
	struct Descriptor;

	class Geometry;

	struct PlaneDescriptor {
		vk::Extent2D extent;
		vk::Format format;
		vk::ComponentMapping swizzle;
	};

	using PixelData = std::vector<Utils::BufferView<std::byte>>;

	static constexpr size_t FILTER_COUNT = VK_FILTER_LINEAR - VK_FILTER_NEAREST + 1;

	Frame(	const Vulkan& vulkan,
			const std::shared_ptr<const Descriptor> desc,
			const std::shared_ptr<const Buffer>& colorTransfer,
			Utils::BufferView<const PlaneDescriptor> planes,
			vk::ImageUsageFlags usage );
	Frame(const Frame& other) = delete;
	Frame(Frame&& other);
	virtual ~Frame();

	Frame& 									operator=(const Frame& other) = delete;
	Frame&									operator=(Frame&& other);

	void									bind(	vk::CommandBuffer cmd,
													vk::PipelineLayout layout,
													uint32_t index,
													vk::Filter filter ) const;

	void									addDependecy(vk::Fence fence);
	void									waitDependecies(uint64_t timeo = Vulkan::NO_TIMEOUT) const;

	const Vulkan&							getVulkan() const;
	const Descriptor&						getDescriptor() const;
	const Math::Vec2f&						getSize() const;

	const std::vector<vk::UniqueImage>&		getImages() const;
	const std::vector<vk::UniqueImageView>&	getImageViews() const;
	const std::vector<Utils::Area>&			getPlaneAreas() const;
	const vk::DeviceMemory&					getMemory() const;

	static std::shared_ptr<Buffer>			createColorTransferBuffer(	const Vulkan& vulkan,
																		const ColorTransfer& colorTransfer );
	static std::vector<PlaneDescriptor>		getPlaneDescriptors(Resolution resolution,
																ColorSubsampling subsampling,
																ColorFormat format );
	static vk::DescriptorSetLayout			getDescriptorSetLayout(	const Vulkan& vulkan,
																	vk::Filter filt );

private:
	struct Impl;
	Utils::Pimpl<Impl>						m_impl;

};


struct Frame::Descriptor : Utils::Configuration {
	using Configuration::Configuration;

	static inline const std::string RESOLUTION = "resolution";							static constexpr auto DEFAULT_RESOLUTION = Resolution(1280, 720);
	static inline const std::string PIXEL_ASPECT_RATIO = "pixelAspectRatio";			static constexpr auto DEFAULT_PIXEL_ASPECT_RATIO = AspectRatio(1, 1);
	static inline const std::string COLOR_PRIMARIES = "colorPrimaries";					static constexpr auto DEFAULT_COLOR_PRIMARIES = ColorPrimaries::BT709;
	static inline const std::string COLOR_MODEL = "colorModel";							static constexpr auto DEFAULT_COLOR_MODEL = ColorModel::RGB;
	static inline const std::string COLOR_TRANSFER_FUNCTION = "colorTransferFunction";	static constexpr auto DEFAULT_COLOR_TRANSFER_FUNCTION = ColorTransferFunction::IEC61966_2_1;
	static inline const std::string COLOR_SUBSAMPLING = "colorSubsampling";				static constexpr auto DEFAULT_COLOR_SUBSAMPLING = ColorSubsampling::NONE;
	static inline const std::string COLOR_RANGE = "colorRange";							static constexpr auto DEFAULT_COLOR_RANGE = ColorRange::FULL;
	static inline const std::string COLOR_FORMAT = "colorFormat";						static constexpr auto DEFAULT_COLOR_FORMAT = ColorFormat::B8G8R8A8;

	Descriptor(Resolution res = DEFAULT_RESOLUTION);

	void 					setResolution(Resolution res);
	Resolution 				getResolution() const;

	void					setPixelAspectRatio(AspectRatio ratio);
	AspectRatio				getPixelAspectRatio() const;

	Math::Vec2f				calculateSize() const;

	void					setColorPrimaries(ColorPrimaries primaries);
	ColorPrimaries			getColorPrimaries() const;

	void					setColorModel(ColorModel model);
	ColorModel				getColorModel() const;

	void					setColorTransferFunction(ColorTransferFunction xferFn);
	ColorTransferFunction	getColorTransferFunction() const;

	void					setColorSubsampling(ColorSubsampling subs);
	ColorSubsampling		getColorSubsampling() const;

	void					setColorRange(ColorRange range);
	ColorRange				getColorRange() const;

	void					setColorFormat(ColorFormat format);
	ColorFormat				getColorFormat() const;
};


class Frame::Geometry {
public:
	Geometry(	std::byte* data,
				size_t stride,
				size_t positionOffset,
				size_t texCoordOffset,
				ScalingMode scaling = ScalingMode::STRETCH,
				Math::Vec2f targetSize = Math::Vec2f() );
	Geometry(const Geometry& other) = default;
	~Geometry() = default;

	Geometry&								operator=(const Geometry& other) = default;

	void									setScalingMode(ScalingMode scaling);
	ScalingMode								getScalingMode() const;

	void									setTargetSize(Math::Vec2f size);
	const Math::Vec2f&						getTargetSize() const;

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
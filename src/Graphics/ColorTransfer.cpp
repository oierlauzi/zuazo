#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Math/Transformations.h>

#include <zuazo/shaders/color_transfer.h>

#include <iostream> //TODO

namespace Zuazo::Graphics {

/*
 * Conversions
 */

constexpr int32_t getColorPrimaries(ColorPrimaries primaries, const Chromaticities& chroma) noexcept {
	int32_t result = 0;

	if(getChromaticities(primaries) == chroma) {
		result = static_cast<int32_t>(primaries);
	} else {
		//If chromaticities are not the expected ones, force conversion
		result = ct_COLOR_PRIMARIES_UNKNOWN;
	}

	return result;
}

constexpr int32_t getColorTransferFunction(ColorTransferFunction transferFunction) noexcept {
	switch(transferFunction){		
	case ColorTransferFunction::BT601:
	case ColorTransferFunction::BT709:
	case ColorTransferFunction::BT2020_10:
	case ColorTransferFunction::BT2020_12:		return ct_COLOR_TRANSFER_FUNCTION_BT601_709_2020;
	case ColorTransferFunction::GAMMA22:		return ct_COLOR_TRANSFER_FUNCTION_GAMMA22;
	case ColorTransferFunction::GAMMA26:		return ct_COLOR_TRANSFER_FUNCTION_GAMMA26;
	case ColorTransferFunction::GAMMA28:		return ct_COLOR_TRANSFER_FUNCTION_GAMMA28;
	case ColorTransferFunction::IEC61966_2_1:	return ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1;
	case ColorTransferFunction::IEC61966_2_4:	return ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_4;
	case ColorTransferFunction::SMPTE240M:		return ct_COLOR_TRANSFER_FUNCTION_SMPTE240M;
	case ColorTransferFunction::SMPTE2084:		return ct_COLOR_TRANSFER_FUNCTION_SMPTE2084;
	case ColorTransferFunction::ARIB_STD_B67:	return ct_COLOR_TRANSFER_FUNCTION_ARIB_STD_B67;
	default: /*ColorTransferFunction::LINEAR*/	return ct_COLOR_TRANSFER_FUNCTION_LINEAR;
	}
}

inline Math::Mat4x4f getRGB2YCbCrMatrix(ColorModel model) noexcept {
	Math::Mat4x4f result = getRGB2YCbCrConversionMatrix(model);

	if(isYCbCr(model)) {
		//If it is YCbCr a displacement needs to be applied from [-0.5, 0.5] to [0.0, 1.0]
		//							Cr		Y		Cb
		constexpr Math::Vec3f delta(0.5f, 	0.0f, 	0.5f );
		result[result.columns() - 1] = Math::Vec4f(delta, 1.0f);
	}

	return result;
}

constexpr int32_t getColorModel(ColorModel model) {
	int32_t result = ct_COLOR_MODEL_RGB;

	if(isYCbCr(model)) {
		result = ct_COLOR_MODEL_YCBCR;
	}

	return result;
}

constexpr int32_t getColorRange(ColorRange range, ColorModel model) noexcept {
	switch(range){
	case ColorRange::ITU_NARROW: 		return isYCbCr(model) ? ct_COLOR_RANGE_ITU_NARROW_YCBCR : ct_COLOR_RANGE_ITU_NARROW_RGB;
	default: /*ColorRange::FULL*/		return ct_COLOR_RANGE_FULL;
	}
}

constexpr int32_t getPlaneFormat(ColorFormat format) noexcept {
	const auto planeCount = getPlaneCount(format);

	switch(planeCount){
	case 2: 		return ct_PLANE_FORMAT_G_BR;
	case 3: 		return ct_PLANE_FORMAT_G_B_R;
	case 4: 		return ct_PLANE_FORMAT_G_B_R_A;
	default: /*1*/	return ct_PLANE_FORMAT_RGBA;
	}
}

static int32_t optimizeColorTransferFunction(	int32_t colorRange,
												int32_t colorModel,
												int32_t colorTransferFunction,
												Utils::BufferView<Image::Plane> planes,
												Utils::BufferView<const vk::Format> supportedFormats ) noexcept
{
	//For binary search:
	assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend()));

	//Test if sRGB formats can be used
	if(	colorRange == ct_COLOR_RANGE_FULL && 
		colorModel == ct_COLOR_MODEL_RGB && 
		colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1 )
	{
		//For being able to use Vulkan's built in sRGB formats, all planes need to support it
		colorTransferFunction = ct_COLOR_TRANSFER_FUNCTION_LINEAR; //Suppose it is supported
		for(size_t i = 0; i < planes.size(); i++) {
			//Evaluate if it can be optimized
			const auto optimized = toSrgb(planes[i].getFormat());
			if(optimized == planes[i].getFormat()){ //toSrgb returns the format itself in case of failure
				//Optimization not available
				colorTransferFunction = ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1;
				break; 
			}

			//Evaluate if it is supported
			if(	!std::binary_search(supportedFormats.cbegin(), 
									supportedFormats.cend(), 
									optimized ))
			{
				//Optimization not supported
				colorTransferFunction = ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1;
				break;
			}

			//This plane can be optimized
			planes[i].setFormat(optimized); //Write
		}

		//Test for failure
		if(colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1) {
			//Optimization failed
			for(size_t i = 0; i < planes.size(); i++) {
				//Reset all changes
				//Note that fromSrgb returns the format itself it is not sRGB, so that unchanged formats will remain unchanged
				planes[i].setFormat(fromSrgb(planes[i].getFormat()));
			}
		}
	}

	return colorTransferFunction;
}

static int32_t optimizePlanes(	int32_t planeFormat,
								Utils::BufferView<Image::Plane> planes,
								Utils::BufferView<const vk::Format> supportedFormats )
{
	if(planeFormat > ct_PLANE_FORMAT_RGBA) {
		//It is a multiplanar format. Try to use Vulkan's multiplanar formats if supported
		//Obtain the chroma subsampling. Always between the 1st and 2nd planes
		const auto subsampling = static_cast<Math::Vec2f>(
			fromVulkan(to2D(planes[0].getExtent())) / 
			fromVulkan(to2D(planes[1].getExtent()))
		);

		constexpr Math::Vec2f SUBSAMPLING_444(1U, 1U);
		constexpr Math::Vec2f SUBSAMPLING_422(2U, 1U);
		constexpr Math::Vec2f SUBSAMPLING_420(2U, 2U);

		//Try to optimize the format
		vk::Format optimizedFormat = vk::Format::eUndefined;

		switch(planeFormat) {
		case ct_PLANE_FORMAT_G_B_R:
			//3 Plane format
			switch (planes.front().getFormat()) {
			case vk::Format::eR8Unorm:
				//8bpp
				if(subsampling == SUBSAMPLING_444) {
					optimizedFormat = vk::Format::eG8B8R83Plane444Unorm;
				} else if (subsampling == SUBSAMPLING_422) {
					optimizedFormat = vk::Format::eG8B8R83Plane422Unorm;
				} else if (subsampling == SUBSAMPLING_420) {
					optimizedFormat = vk::Format::eG8B8R83Plane420Unorm;
				}
				break;

			case vk::Format::eR10X6UnormPack16:
				//10bpp
				if(subsampling == SUBSAMPLING_444) {
					optimizedFormat = vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16;
				} else if (subsampling == SUBSAMPLING_422) {
					optimizedFormat = vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16;
				} else if (subsampling == SUBSAMPLING_420) {
					optimizedFormat = vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16;
				}
				break;

			case vk::Format::eR12X4UnormPack16:
				//10bpp
				if(subsampling == SUBSAMPLING_444) {
					optimizedFormat = vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16;
				} else if (subsampling == SUBSAMPLING_422) {
					optimizedFormat = vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16;
				} else if (subsampling == SUBSAMPLING_420) {
					optimizedFormat = vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16;
				}
				break;

			case vk::Format::eR16Unorm:
				//8bpp
				if(subsampling == SUBSAMPLING_444) {
					optimizedFormat = vk::Format::eG16B16R163Plane444Unorm;
				} else if (subsampling == SUBSAMPLING_422) {
					optimizedFormat = vk::Format::eG16B16R163Plane422Unorm;
				} else if (subsampling == SUBSAMPLING_420) {
					optimizedFormat = vk::Format::eG16B16R163Plane420Unorm;
				}
				break;

			default:
				//Unknown, leave it as it is
				break;
			}
			break;

		case ct_PLANE_FORMAT_G_BR:
			//2 Plane format
			switch (planes.front().getFormat()) {
			case vk::Format::eR8Unorm:
				//8bpp
				if (subsampling == SUBSAMPLING_422) {
					optimizedFormat = vk::Format::eG8B8R82Plane422Unorm;
				} else if (subsampling == SUBSAMPLING_420) {
					optimizedFormat = vk::Format::eG8B8R82Plane420Unorm;
				}
				break;

			case vk::Format::eR10X6UnormPack16:
				//10bpp
				if (subsampling == SUBSAMPLING_422) {
					optimizedFormat = vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16;
				} else if (subsampling == SUBSAMPLING_420) {
					optimizedFormat = vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16;
				}
				break;

			case vk::Format::eR12X4UnormPack16:
				//10bpp
				if (subsampling == SUBSAMPLING_422) {
					optimizedFormat = vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16;
				} else if (subsampling == SUBSAMPLING_420) {
					optimizedFormat = vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16;
				}
				break;

			case vk::Format::eR16Unorm:
				//8bpp
				if (subsampling == SUBSAMPLING_422) {
					optimizedFormat = vk::Format::eG16B16R162Plane422Unorm;
				} else if (subsampling == SUBSAMPLING_420) {
					optimizedFormat = vk::Format::eG16B16R162Plane420Unorm;
				}
				break;
			
			default:
				//Unknown, leave it as it is
				break;
			}
			break;

		default:
			break;

		}


		//Check for support
		if(optimizedFormat != vk::Format::eUndefined) {
			//For binary search:
			assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend()));

			if(std::binary_search(supportedFormats.cbegin(), supportedFormats.cend(), optimizedFormat)) {
				//Format is supported
				planeFormat = ct_PLANE_FORMAT_RGBA; //Just a passthough
				planes.front().setFormat(optimizedFormat);

				//Consider if the 2 plane have their 2nd plane components inverted (Swap R and B)
				planes.front().setSwizzle((planes[1].getSwizzle() == swizzle("GR")) ? swizzle("BIRI") : vk::ComponentMapping()); 

				//Reset the rest of the planes
				for(size_t i = 1; i < planes.size(); ++i) {
					planes[i] = Image::Plane(vk::Extent3D(), vk::Format::eUndefined);
				}
			}
		}
	}

	return planeFormat;
}

static ct_write_data convert2Output(const ct_read_data& read) noexcept {
	return ct_write_data {
		Math::inv(read.mtxRGB2XYZ),
		Math::inv(read.mtxYCbCr2RGB),
		read.colorPrimaries,
		read.colorTransferFunction,
		read.colorModel,
		read.colorRange,
		read.planeFormat
	};
}



/*
 * InputColorTransfer::Impl
 */
struct InputColorTransfer::Impl {
	ct_read_data transferData;

	Impl() noexcept = default;
	Impl(const Frame::Descriptor& desc, const Chromaticities& chromaticities) noexcept
		: transferData {
			Math::Mat4x4f(chromaticities.calculateRGB2XYZConversionMatrix()),
			Math::inv(getRGB2YCbCrMatrix(desc.getColorModel())),
			getColorPrimaries(desc.getColorPrimaries(), chromaticities),
			getColorTransferFunction(desc.getColorTransferFunction()),
			getColorModel(desc.getColorModel()),
			getColorRange(desc.getColorRange(), desc.getColorModel()),
			getPlaneFormat(desc.getColorFormat())
		}
	{
	}

	bool equals(const Impl& other) const noexcept {
		return std::memcmp(data(), other.data(), size()) == 0;
	}

	void optimize(	Utils::BufferView<Image::Plane> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) noexcept
	{
		transferData.planeFormat = optimizePlanes(
			transferData.planeFormat,
			planes,
			supportedFormats
		);

		transferData.colorTransferFunction = optimizeColorTransferFunction(
			transferData.colorRange,
			transferData.colorModel,
			transferData.colorTransferFunction,
			planes,
			supportedFormats
		);
	}

	const std::byte* data() const noexcept {
		return reinterpret_cast<const std::byte*>(&transferData);
	}


	bool isPassthough() const noexcept {
		return	transferData.colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_LINEAR &&
				transferData.colorModel == ct_COLOR_MODEL_RGB &&
				transferData.colorRange == ct_COLOR_RANGE_FULL &&
				transferData.planeFormat == ct_PLANE_FORMAT_RGBA ;
	}

	

	static int32_t getSamplingMode(	ScalingFilter filter,
									vk::Filter samplerFilter ) noexcept
	{
		int32_t result;

		switch(filter) {
		case ScalingFilter::LINEAR:
			if(samplerFilter == vk::Filter::eLinear) {
				//Perform bilinear sampling using bilinear samplers
				result = ct_SAMPLE_MODE_PASSTHOUGH;
			} else {
				//Perform bilinear sampling using nearest samplers
				assert(samplerFilter == vk::Filter::eNearest);
				result = ct_SAMPLE_MODE_BILINEAR;
			}
			break;
		
		case ScalingFilter::CUBIC:
			if(samplerFilter == vk::Filter::eCubicEXT) {
				//Perform bicubic sampling using bicubic samplers
				result = ct_SAMPLE_MODE_PASSTHOUGH;
			} else if(samplerFilter == vk::Filter::eLinear) {
				//Perform bicubic sampling using linear samplers
				result = ct_SAMPLE_MODE_BILINEAR_TO_BICUBIC;
			} else {
				//Perform bicubic sampling using nearest samplers
				assert(samplerFilter == vk::Filter::eNearest);
				result = ct_SAMPLE_MODE_BICUBIC;
			}
			break;

		default: //ScalingFilter::NEAREST
			assert(samplerFilter == vk::Filter::eNearest);
			result = ct_SAMPLE_MODE_PASSTHOUGH;
			break;
		}

		return result;
	}
};


/*
 * InputColorTransfer
 */

InputColorTransfer::InputColorTransfer()
	: m_impl({})
{
}

InputColorTransfer::InputColorTransfer(const Frame::Descriptor& desc)
	: InputColorTransfer(desc, getChromaticities(desc.getColorPrimaries()))
{
}

InputColorTransfer::InputColorTransfer(const Frame::Descriptor& desc, const Chromaticities& customPrimaries)
	: m_impl({}, desc, customPrimaries)
{
}

InputColorTransfer::InputColorTransfer(InputColorTransfer&& other) noexcept = default;

InputColorTransfer::~InputColorTransfer() = default;


InputColorTransfer& InputColorTransfer::operator=(InputColorTransfer&& other) noexcept = default;

bool InputColorTransfer::operator==(const InputColorTransfer& other) const noexcept {
	return m_impl->equals(*other.m_impl);
}

bool InputColorTransfer::operator!=(const InputColorTransfer& other) const noexcept {
	return !operator==(other);
}




void InputColorTransfer::optimize(	Utils::BufferView<Image::Plane> planes,
									Utils::BufferView<const vk::Format> supportedFormats ) noexcept
{
	m_impl->optimize(planes, supportedFormats);
}


const std::byte* InputColorTransfer::data() const noexcept {
	return m_impl->data();
}

bool InputColorTransfer::isPassthough() const noexcept {
	return m_impl->isPassthough();
}

int32_t InputColorTransfer::getSamplingMode(ScalingFilter filter,
											vk::Filter samplerFilter ) noexcept
{
	return Impl::getSamplingMode(filter, samplerFilter);
}

size_t InputColorTransfer::size() noexcept {
	return sizeof(ct_read_data);
}



/*
 * OutputColorTransfer::Impl
 */
struct OutputColorTransfer::Impl {
	ct_write_data transferData;

	Impl() noexcept = default;
	Impl(const Frame::Descriptor& desc, const Chromaticities& chromaticities) noexcept
		: transferData {
			Math::Mat4x4f(chromaticities.calculateXYZ2RGBConversionMatrix()),
			getRGB2YCbCrMatrix(desc.getColorModel()),
			getColorPrimaries(desc.getColorPrimaries(), chromaticities),
			getColorTransferFunction(desc.getColorTransferFunction()),
			getColorModel(desc.getColorModel()),
			getColorRange(desc.getColorRange(), desc.getColorModel()),
			getPlaneFormat(desc.getColorFormat())
		}
	{
	}

	Impl(const InputColorTransfer::Impl& input) noexcept
		: transferData(convert2Output(input.transferData))
	{
	}


	bool equals(const Impl& other) const {
		return std::memcmp(data(), other.data(), size()) == 0;
	}

	void optimize(	Utils::BufferView<Image::Plane> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) noexcept
	{
		transferData.colorTransferFunction = optimizeColorTransferFunction(
			transferData.colorRange,
			transferData.colorModel,
			transferData.colorTransferFunction,
			planes,
			supportedFormats
		);
	}

	const std::byte* data() const noexcept {
		return reinterpret_cast<const std::byte*>(&transferData);
	}

	bool isPassthough() const noexcept {
		return	transferData.colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_LINEAR &&
				transferData.colorModel == ct_COLOR_MODEL_RGB &&
				transferData.colorRange == ct_COLOR_RANGE_FULL &&
				transferData.planeFormat == ct_PLANE_FORMAT_RGBA ;
	}
};


/*
 * OutputColorTransfer
 */

OutputColorTransfer::OutputColorTransfer()
	: m_impl({})
{
}

OutputColorTransfer::OutputColorTransfer(const Frame::Descriptor& desc)
	: OutputColorTransfer(desc, getChromaticities(desc.getColorPrimaries()))
{
}

OutputColorTransfer::OutputColorTransfer(const Frame::Descriptor& desc, const Chromaticities& customPrimaries)
	: m_impl({}, desc, customPrimaries)
{
}

OutputColorTransfer::OutputColorTransfer(const InputColorTransfer& inputTransfer)
	: m_impl({}, *inputTransfer.m_impl)
{
}

OutputColorTransfer::OutputColorTransfer(OutputColorTransfer&& other) noexcept = default;

OutputColorTransfer::~OutputColorTransfer() = default;


OutputColorTransfer& OutputColorTransfer::operator=(OutputColorTransfer&& other) noexcept = default;

bool OutputColorTransfer::operator==(const OutputColorTransfer& other) const noexcept {
	return m_impl->equals(*other.m_impl);
}

bool OutputColorTransfer::operator!=(const OutputColorTransfer& other) const noexcept {
	return !operator==(other);
}




void OutputColorTransfer::optimize(	Utils::BufferView<Image::Plane> planes,
									Utils::BufferView<const vk::Format> supportedFormats ) noexcept
{
	m_impl->optimize(planes, supportedFormats);
}


const std::byte* OutputColorTransfer::data() const noexcept {
	return m_impl->data();
}

bool OutputColorTransfer::isPassthough() const noexcept {
	return m_impl->isPassthough();
}

size_t OutputColorTransfer::size() noexcept {
	return sizeof(ct_read_data);
}

}
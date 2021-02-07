#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/VulkanConversions.h>
#include <zuazo/Math/Transformations.h>

#include <zuazo/shaders/color_transfer.h>

#include <iostream> //TODO

namespace Zuazo::Graphics {

/*
 * Conversions
 */
namespace ColorTransfer {

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
												Utils::BufferView<Image::PlaneDescriptor> planes,
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
			const auto optimized = toSrgb(planes[i].format);
			if(optimized == planes[i].format){ //toSrgb returns the format itself in case of failure
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
			planes[i].format = optimized; //Write
		}

		//Test for failure
		if(colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1) {
			//Optimization failed
			for(size_t i = 0; i < planes.size(); i++) {
				//Reset all changes
				//Note that fromSrgb returns the format itself it is not sRGB, so that unchanged formats will remain unchanged
				planes[i].format = fromSrgb(planes[i].format);
			}
		}
	}

	return colorTransferFunction;
}

static uint32_t optimizePlanes(	Utils::BufferView<Image::PlaneDescriptor> planes,
								Utils::BufferView<const vk::Format> supportedFormats )
{
	const size_t planeCount = planes.size();
	int32_t planeFormat = ct_PLANE_FORMAT_RGBA;

	if(planeCount > 1) {
		//It is a multiplanar format. Try to use Vulkan's multiplanar formats if supported
		//Obtain the chroma subsampling. Always between the 1st and 2nd planes
		const auto subsampling = 
			Math::Vec2f(planes[0].extent.width, planes[0].extent.height) / 
			Math::Vec2f(planes[1].extent.width, planes[1].extent.height) ;

		constexpr Math::Vec2f SUBSAMPLING_444(1U, 1U);
		constexpr Math::Vec2f SUBSAMPLING_422(2U, 1U);
		constexpr Math::Vec2f SUBSAMPLING_420(2U, 2U);

		//Try to optimize the format
		vk::Format optimizedFormat = vk::Format::eUndefined;

		if(planeCount == 4) {
			//4 Plane format. Not supported natively. //TODO fall back to a combination of 3plane + alpha
			planeFormat = ct_PLANE_FORMAT_G_B_R_A;
		} else if (planeCount == 3) {
			//3 Plane format
			planeFormat = ct_PLANE_FORMAT_G_B_R;

			switch (planes.front().format) {
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

		} else if (planeCount == 2) {
			//2 Plane format
			planeFormat = ct_PLANE_FORMAT_G_B_R;

			switch (planes.front().format) {
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

		}

		//Check for support
		if(optimizedFormat != vk::Format::eUndefined) {
			//For binary search:
			assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend()));

			if(std::binary_search(supportedFormats.cbegin(), supportedFormats.cend(), optimizedFormat)) {
				//Format is supported
				planeFormat = ct_PLANE_FORMAT_RGBA; //Just a passthough
				planes.front().format = optimizedFormat;

				//Consider if the 2 plane have their 2nd plane components inverted (Swap R and B)
				planes.front().swizzle = (planes[1].swizzle == swizzle("GR")) ? swizzle("BIRI") : vk::ComponentMapping(); 

				//Reset the rest of the planes
				for(size_t i = 1; i < planes.size(); ++i) {
					planes[i] = {};
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
			Math::inv(ColorTransfer::getRGB2YCbCrMatrix(desc.getColorModel())),
			ColorTransfer::getColorPrimaries(desc.getColorPrimaries(), chromaticities),
			ColorTransfer::getColorTransferFunction(desc.getColorTransferFunction()),
			ColorTransfer::getColorModel(desc.getColorModel()),
			ColorTransfer::getColorRange(desc.getColorRange(), desc.getColorModel()),
			ColorTransfer::getPlaneFormat(desc.getColorFormat())
		}
	{
	}

	bool equals(const Impl& other) const noexcept {
		return std::memcmp(data(), other.data(), size()) == 0;
	}

	void optimize(	Utils::BufferView<Image::PlaneDescriptor> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) noexcept
	{
		transferData.colorTransferFunction = ColorTransfer::optimizeColorTransferFunction(
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

	SamplerDescriptor getSamplerDescriptor(ScalingFilter filter) const noexcept {
		SamplerDescriptor result;

		switch(filter) {
		case ScalingFilter::LINEAR:
			if(transferData.colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_LINEAR) {
				//Perform bilinear sampling using bilinear samplers
				result = {
					vk::Filter::eLinear,
					ct_SAMPLE_MODE_PASSTHOUGH
				};
			} else {
				//Perform bilinear sampling using nearest samplers as gamma correction needs to 
				//be done prior to interpolating
				result = {
					vk::Filter::eNearest,
					ct_SAMPLE_MODE_BILINEAR
				};
			}
			break;
		
		case ScalingFilter::CUBIC:
			if(transferData.colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_LINEAR) {
				//Perform bicubic sampling using linear samplers
				result = {
					vk::Filter::eLinear,
					ct_SAMPLE_MODE_BICUBIC
				};
			} else {
				//Perform bicubic sampling using nearest samplers as gamma correction needs to 
				//be done prior to interpolating
				result = {
					vk::Filter::eNearest,
					ct_SAMPLE_MODE_BICUBIC
				};
			}
			break;

		default: //ScalingFilter::NEAREST
			result = {
				vk::Filter::eNearest,
				ct_SAMPLE_MODE_PASSTHOUGH
			};
			break;
		}

		return result;
	}

	bool isPassthough() const noexcept {
		return	transferData.colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_LINEAR &&
				transferData.colorModel == ct_COLOR_MODEL_RGB &&
				transferData.colorRange == ct_COLOR_RANGE_FULL &&
				transferData.planeFormat == ct_PLANE_FORMAT_RGBA ;
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




void InputColorTransfer::optimize(	Utils::BufferView<Image::PlaneDescriptor> planes,
									Utils::BufferView<const vk::Format> supportedFormats ) noexcept
{
	m_impl->optimize(planes, supportedFormats);
}


const std::byte* InputColorTransfer::data() const noexcept {
	return m_impl->data();
}

InputColorTransfer::SamplerDescriptor InputColorTransfer::getSamplerDescriptor(ScalingFilter filter) const noexcept {
	return m_impl->getSamplerDescriptor(filter);
}


bool InputColorTransfer::isPassthough() const noexcept {
	return m_impl->isPassthough();
}


uint32_t InputColorTransfer::getSamplerCount() noexcept {
	return ct_SAMPLER_COUNT;
}

uint32_t InputColorTransfer::getSamplerBinding() noexcept {
	return ct_SAMPLER_BINDING;
}

uint32_t InputColorTransfer::getDataBinding() noexcept {
	return ct_DATA_BINDING;
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
			ColorTransfer::getRGB2YCbCrMatrix(desc.getColorModel()),
			ColorTransfer::getColorPrimaries(desc.getColorPrimaries(), chromaticities),
			ColorTransfer::getColorTransferFunction(desc.getColorTransferFunction()),
			ColorTransfer::getColorModel(desc.getColorModel()),
			ColorTransfer::getColorRange(desc.getColorRange(), desc.getColorModel()),
			ColorTransfer::getPlaneFormat(desc.getColorFormat())
		}
	{
	}

	Impl(const InputColorTransfer::Impl& input) noexcept
		: transferData(ColorTransfer::convert2Output(input.transferData))
	{
	}


	bool equals(const Impl& other) const {
		return std::memcmp(data(), other.data(), size()) == 0;
	}

	void optimize(	Utils::BufferView<Image::PlaneDescriptor> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) noexcept
	{
		transferData.colorTransferFunction = ColorTransfer::optimizeColorTransferFunction(
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




void OutputColorTransfer::optimize(	Utils::BufferView<Image::PlaneDescriptor> planes,
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


uint32_t OutputColorTransfer::getAttachmentCount() noexcept {
	return ct_OUTPUT_COUNT;
}

size_t OutputColorTransfer::size() noexcept {
	return sizeof(ct_read_data);
}

}
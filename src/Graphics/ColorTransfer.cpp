#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/VulkanConversions.h>

#include <zuazo/shaders/color_transfer.h>

namespace Zuazo::Graphics {

/*
 * Conversions
 */
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

constexpr int32_t getColorRange(ColorRange range) noexcept {
	switch(range){
	case ColorRange::FULL_YCBCR: 		return ct_COLOR_RANGE_FULL_YCBCR;
	case ColorRange::ITU_NARROW_RGB: 	return ct_COLOR_RANGE_ITU_NARROW_RGB;
	case ColorRange::ITU_NARROW_YCBCR: 	return ct_COLOR_RANGE_ITU_NARROW_YCBCR;
	default: /*ColorRange::FULL_RGB*/	return ct_COLOR_RANGE_FULL_RGB;
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
												int32_t colorTransferFunction,
												Utils::BufferView<Frame::PlaneDescriptor> planes,
												Utils::BufferView<const vk::Format> supportedFormats ) noexcept
{
	//For binary search:
	assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend()));

	//Test if SRGB formats can be used
	if(colorRange == ct_COLOR_RANGE_FULL_RGB && colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1) {
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

static ct_write_data convert2Output(const ct_read_data& read) noexcept {
	return ct_write_data {
		glm::inverse(read.mtxRGB2XYZ),
		glm::inverse(read.mtxYCbCr2RGB),
		read.colorTransferFunction,
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
	Impl(const Frame::Descriptor& desc) noexcept
		: transferData {
			getRGB2XYZConversionMatrix(desc.colorPrimaries),
			glm::inverse(getRGB2YCbCrConversionMatrix(desc.colorModel)),
			getColorTransferFunction(desc.colorTransferFunction),
			getColorRange(desc.colorRange),
			getPlaneFormat(desc.colorFormat)
		}
	{
	}

	bool equals(const Impl& other) const noexcept {
		return std::memcmp(data(), other.data(), size()) == 0;
	}

	void optimize(	Utils::BufferView<Frame::PlaneDescriptor> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) noexcept
	{
		transferData.colorTransferFunction = optimizeColorTransferFunction(
			transferData.colorRange,
			transferData.colorTransferFunction,
			planes,
			supportedFormats
		);
	}

	const std::byte* data() const noexcept {
		return reinterpret_cast<const std::byte*>(&transferData);
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
	: m_impl({}, desc)
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




void InputColorTransfer::optimize(	Utils::BufferView<Frame::PlaneDescriptor> planes,
									Utils::BufferView<const vk::Format> supportedFormats ) noexcept
{
	m_impl->optimize(planes, supportedFormats);
}


const std::byte* InputColorTransfer::data() const noexcept {
	return m_impl->data();
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
	Impl(const Frame::Descriptor& desc) noexcept
		: transferData {
			glm::inverse(getRGB2XYZConversionMatrix(desc.colorPrimaries)),
			getRGB2YCbCrConversionMatrix(desc.colorModel),
			getColorTransferFunction(desc.colorTransferFunction),
			getColorRange(desc.colorRange),
			getPlaneFormat(desc.colorFormat)
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

	void optimize(	Utils::BufferView<Frame::PlaneDescriptor> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) noexcept
	{
		transferData.colorTransferFunction = optimizeColorTransferFunction(
			transferData.colorRange,
			transferData.colorTransferFunction,
			planes,
			supportedFormats
		);
	}

	const std::byte* data() const noexcept {
		return reinterpret_cast<const std::byte*>(&transferData);
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
	: m_impl({}, desc)
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




void OutputColorTransfer::optimize(	Utils::BufferView<Frame::PlaneDescriptor> planes,
									Utils::BufferView<const vk::Format> supportedFormats ) noexcept
{
	m_impl->optimize(planes, supportedFormats);
}


const std::byte* OutputColorTransfer::data() const noexcept {
	return m_impl->data();
}

size_t OutputColorTransfer::size() noexcept {
	return sizeof(ct_read_data);
}

}
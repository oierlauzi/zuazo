#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Graphics/VulkanConversions.h>

#include "color_transfer.h"

namespace Zuazo::Graphics {


/*
 * ColorTransfer::Impl
 */
struct ColorTransfer::Impl {
	ct_data transferData;

	Impl() = default;
	Impl(const Frame::Descriptor& desc)
		: transferData {
			getConversionMatrix(desc.colorPrimaries),
			getConversionMatrix(desc.colorModel),
			getTransferFunction(desc.colorTransferFunction),
			getRange(desc.colorRange, desc.colorModel),
			getPlaneFormat(desc.colorFormat)
		}
	{
	}

	bool equals(const Impl& other) const {
		return std::memcmp(data(), other.data(), size());
	}

	void optimize(	Utils::BufferView<Frame::PlaneDescriptor> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) 
	{
		//For binary search:
		assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend()));

		if(	transferData.colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1 && 
			transferData.colorRange == ct_COLOR_RANGE_FULL_RGB )
		{
			//For being able to use Vulkan's built in sRGB formats, all planes need to support it
			transferData.colorTransferFunction = ct_COLOR_TRANSFER_FUNCTION_LINEAR; //Suppose it is supported
			for(size_t i = 0; i < planes.size(); i++) {
				//Evaluate if it can be optimized
				const auto optimized = toSrgb(planes[i].format);
				if(optimized == planes[i].format){ //toSrgb returns the format itself in case of failure
					//Optimization not available
					transferData.colorTransferFunction = ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1;
					break; 
				}
				if(	!std::binary_search(supportedFormats.cbegin(), 
										supportedFormats.cend(), 
										optimized ))
				{
					//Optimization not supported
					transferData.colorTransferFunction = ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1;
					break;
				}

				//This plane can be optimized
				planes[i].format = optimized;
			}

			if(transferData.colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1) {
				//Optimization failed
				for(size_t i = 0; i < planes.size(); i++) {
					//Reset all changes
					//Note that fromSrgb returns the format itself it is not sRGB, so that unchanged formats will remain unchanged
					planes[i].format = fromSrgb(planes[i].format);
				}
			}
		}
	}

	const std::byte* data() const {
		return reinterpret_cast<const std::byte*>(&transferData);
	}

private:
	static int32_t getTransferFunction(ColorTransferFunction transferFunction) {
		switch(transferFunction){
		case ColorTransferFunction::LINEAR: return ct_COLOR_TRANSFER_FUNCTION_LINEAR;
		case ColorTransferFunction::IEC61966_2_1: return ct_COLOR_TRANSFER_FUNCTION_IEC61966_2_1;
		default: break;
		}

		return -1;
	}

	static int32_t getRange(ColorRange range, ColorModel model) {
		switch(model){
		case ColorModel::RGB:
			switch(range){
			case ColorRange::FULL: return ct_COLOR_RANGE_FULL_RGB;
			case ColorRange::ITU_NARROW: return ct_COLOR_RANGE_ITU_NARROW_RGB;
			default: break;
			}
			break;
		case ColorModel::BT601:
		case ColorModel::BT709:
		case ColorModel::BT2020:
			switch(range){
			case ColorRange::FULL: return ct_COLOR_RANGE_FULL_YCBCR;
			case ColorRange::ITU_NARROW: return ct_COLOR_RANGE_ITU_NARROW_YCBCR;
			default: break;
			}
			break;
		default: break;
		}

		return -1;
	}

	static int32_t getPlaneFormat(ColorFormat format) {
		const auto planeCount = getPlaneCount(format);

		switch(planeCount){
		case 1: return ct_PLANE_FORMAT_RGBA;
		case 2: return ct_PLANE_FORMAT_G_BR;
		case 3: return ct_PLANE_FORMAT_G_B_R;
		default: return -1;
		}
	}
};




/*
 * ColorTransfer
 */

ColorTransfer::ColorTransfer() = default;

ColorTransfer::ColorTransfer(const Frame::Descriptor& desc)
	: m_impl(desc)
{
}

ColorTransfer::ColorTransfer(ColorTransfer&& other) = default;

ColorTransfer::~ColorTransfer() = default;


ColorTransfer& ColorTransfer::operator=(ColorTransfer&& other) = default;

bool ColorTransfer::operator==(const ColorTransfer& other) const {
	return m_impl->equals(*other.m_impl);
}

bool ColorTransfer::operator!=(const ColorTransfer& other) const {
	return !operator==(other);
}




void ColorTransfer::optimize(	Utils::BufferView<Frame::PlaneDescriptor> planes,
								Utils::BufferView<const vk::Format> supportedFormats ) 
{
	m_impl->optimize(planes, supportedFormats);
}


const std::byte* ColorTransfer::data() const {
	return m_impl->data();
}



uint32_t ColorTransfer::getSamplerCount() {
	return ct_SAMPLER_COUNT;
}

uint32_t ColorTransfer::getSamplerBinding() {
	return ct_SAMPLER_BINDING;
}

uint32_t ColorTransfer::getDataBinding() {
	return ct_DATA_BINDING;
}

size_t ColorTransfer::size() {
	return sizeof(ct_data);
}

}
#include <zuazo/Graphics/ColorTransfer.h>

#include <zuazo/Utils/Hasher.h>
#include <zuazo/Utils/StaticId.h>

#include <unordered_map>
#include <cstring>

#include "../../shaders/color_transfer.h"

namespace Zuazo::Graphics {

/*
 * Conversions
 */

struct UnalignedMat3x3 {
	UnalignedMat3x3(const Math::Mat3x3f& m)
		: m00(m[0][0])
		, m01(m[0][1])
		, m02(m[0][2])
		, m10(m[1][0])
		, m11(m[1][1])
		, m12(m[1][2])
		, m20(m[2][0])
		, m21(m[2][1])
		, m22(m[2][2])
	{
	}
	~UnalignedMat3x3() = default;

	float 	m00, m01, m02,
			m10, m11, m12,
			m20, m21, m22;
};

constexpr int32_t getPlaneFormat(ColorFormat format) noexcept {
	const auto planeCount = getPlaneCount(format);

	switch(planeCount){
	case 2: 		return ct_PLANE_FORMAT_G_BR;
	case 3: 		return hasAlpha(format) ? ct_PLANE_FORMAT_G_BR_A : ct_PLANE_FORMAT_G_B_R;
	case 4: 		return ct_PLANE_FORMAT_G_B_R_A;
	default: /*1*/	return ct_PLANE_FORMAT_RGBA;
	}
}

constexpr int32_t getColorRange(ColorRange range, ColorModel model) noexcept {
	switch(range){
	case ColorRange::ITU_NARROW: 		return isYCbCr(model) ? ct_COLOR_RANGE_ITU_NARROW_YCBCR : ct_COLOR_RANGE_ITU_NARROW_RGB;
	default: /*ColorRange::FULL*/		return isYCbCr(model) ? ct_COLOR_RANGE_FULL_YCBCR : ct_COLOR_RANGE_FULL_RGB;
	}
}

constexpr int32_t getColorModel(ColorModel model) {
	int32_t result = ct_COLOR_MODEL_RGB;

	if(isYCbCr(model)) {
		result = ct_COLOR_MODEL_YCBCR;
	}

	return result;
}

constexpr int32_t getColorTransferFunction(ColorTransferFunction transferFunction) noexcept {
	switch(transferFunction){		
	case ColorTransferFunction::BT1886:			return ct_COLOR_TRANSFER_FUNCTION_BT1886;
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

constexpr float getChromaOffset(ColorChromaLocation location, size_t size) {
	float result = 0.0f;

	if(location == ColorChromaLocation::MIDPOINT) {
		result = 0.5f / size;
	}

	return result;
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
	if(	colorRange == ct_COLOR_RANGE_FULL_RGB && 
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

		//TODO optimize formats with alpha
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





/*
 * ColorTransferRead::Impl
 */

struct ColorTransferRead::Impl {
	uint32_t 		planeCount;
	uint32_t 		planeFormat;
	uint32_t 		colorRange;
	uint32_t 		colorModel;
	uint32_t 		colorTransferFunction;
	UnalignedMat3x3	colorModelConversion;
	float			colorChromaOffsetX;
	float			colorChromaOffsetY;

	Impl() 
		: planeCount(1)
		, planeFormat(ct_PLANE_FORMAT_RGBA)
		, colorRange(ct_COLOR_RANGE_FULL_RGB)
		, colorModel(ct_COLOR_MODEL_RGB)
		, colorTransferFunction(ct_COLOR_TRANSFER_FUNCTION_LINEAR)
		, colorModelConversion(Math::Mat3x3f(1.0f))
		, colorChromaOffsetX(0.0f)
		, colorChromaOffsetY(0.0f)
	{
		assert(isPassthough());
	}

	Impl(const Frame::Descriptor& desc)
		: planeCount(Zuazo::getPlaneCount(desc.getColorFormat()))
		, planeFormat(getPlaneFormat(desc.getColorFormat()))
		, colorRange(getColorRange(desc.getColorRange(), desc.getColorModel()))
		, colorModel(getColorModel(desc.getColorModel()))
		, colorTransferFunction(getColorTransferFunction(desc.getColorTransferFunction()))
		, colorModelConversion(Math::inv(getRGB2YCbCrConversionMatrix(desc.getColorModel())))
		, colorChromaOffsetX(getChromaOffset(desc.getColorChromaLocation().x, desc.getResolution().x))
		, colorChromaOffsetY(getChromaOffset(desc.getColorChromaLocation().y, desc.getResolution().y))
	{
	}
	~Impl() = default;



	size_t size() const noexcept {
		return sizeof(*this);
	}

	std::byte* data() noexcept {
		return reinterpret_cast<std::byte*>(this);
	}

	const std::byte* data() const noexcept {
		return reinterpret_cast<const std::byte*>(this);
	}



	void optimize(	Utils::BufferView<Image::Plane> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) noexcept
	{
		planeFormat = optimizePlanes(
			planeFormat,
			planes,
			supportedFormats
		);
		
		colorTransferFunction = optimizeColorTransferFunction(
			colorRange,
			colorModel,
			colorTransferFunction,
			planes,
			supportedFormats
		);
	}

	void optimize(const Sampler& sampler) noexcept {
		//If a YCbCr sampler is used, the shader will only sample 
		//one plane and sumsampling offset is already considered
		if(sampler.getSamplerYCbCrConversion()) {
			planeCount = 1;
			colorChromaOffsetX = 0.0f;
			colorChromaOffsetY = 0.0f;
		}

		//Modify the specialization constants to match the
		//YCbCr sampling settings
		if(sampler.getModel() > vk::SamplerYcbcrModelConversion::eRgbIdentity) {
			//If model conversion is anything but RGB identity,
			//the range expansion will be applied. This means
			//that this provess will be transparent for the
			//shader
			colorRange = ct_COLOR_RANGE_FULL_RGB; //passthru
		}
		if(sampler.getModel() > vk::SamplerYcbcrModelConversion::eYcbcrIdentity) {
			//If model conversion is anything but the identities,
			//a model conversion will be applied. This means that
			//the shader does not have to perform the conversion.
			colorModel = ct_COLOR_MODEL_RGB; //passthru
		}
	}

	bool isPassthough() const noexcept {
		return 	planeFormat == ct_PLANE_FORMAT_RGBA &&
				colorRange == ct_COLOR_RANGE_FULL_RGB &&
				colorModel == ct_COLOR_MODEL_RGB &&
				colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_LINEAR &&
				colorChromaOffsetX == 0.0f &&
				colorChromaOffsetY == 0.0f;
	}
	
	bool isLinear() const noexcept {
		return colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_LINEAR;
	}

	size_t getPlaneCount() const noexcept {
		return planeCount;
	}
	


	vk::DescriptorSetLayout createDescriptorSetLayout(	const Vulkan& vulkan,
														const Sampler& sampler ) const
	{
		using Index = std::tuple<uint32_t, vk::Sampler>;
		static std::unordered_map<Index, const Utils::StaticId, Utils::Hasher<Index>> ids;

		const Index index(planeCount, sampler.getSampler());
		const auto& id = ids[index];  //TODO thread safe

		//Try to retrive the layout from cache
		auto result = vulkan.createDescriptorSetLayout(id);
		if(!result) {
			const std::vector<vk::Sampler> samplers(planeCount, sampler.getSampler());

			const std::array<vk::DescriptorSetLayoutBinding, 1> bindings = {
				vk::DescriptorSetLayoutBinding(
					ct_SAMPLER_BINDING,
					vk::DescriptorType::eCombinedImageSampler,
					samplers.size(),
					vk::ShaderStageFlagBits::eFragment,
					samplers.data()
				)
			};

			const vk::DescriptorSetLayoutCreateInfo createInfo(
				{},
				bindings.size(), bindings.data()
			);

			result = vulkan.createDescriptorSetLayout(id, createInfo);
		}
		assert(result);

		return result;
	}

	static Utils::BufferView<const vk::SpecializationMapEntry> getSpecializationMap() noexcept {
		static const std::array<vk::SpecializationMapEntry, 16> fragmentShaderSpecializationMap = {
			vk::SpecializationMapEntry(
				ct_PLANE_COUNT_ID,
				offsetof(Impl, planeCount),
				sizeof(Impl::planeCount)
			),
			vk::SpecializationMapEntry(
				ct_PLANE_FORMAT_ID,
				offsetof(Impl, planeFormat),
				sizeof(Impl::planeFormat)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_RANGE_ID,
				offsetof(Impl, colorRange),
				sizeof(Impl::colorRange)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_ID,
				offsetof(Impl, colorModel),
				sizeof(Impl::colorModel)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_TRANSFER_FUNCTION_ID,
				offsetof(Impl, colorTransferFunction),
				sizeof(Impl::colorTransferFunction)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M00_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m00),
				sizeof(UnalignedMat3x3::m00)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M01_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m01),
				sizeof(UnalignedMat3x3::m01)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M02_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m02),
				sizeof(UnalignedMat3x3::m02)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M10_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m10),
				sizeof(UnalignedMat3x3::m10)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M11_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m11),
				sizeof(UnalignedMat3x3::m11)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M12_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m12),
				sizeof(UnalignedMat3x3::m12)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M20_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m20),
				sizeof(UnalignedMat3x3::m20)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M21_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m21),
				sizeof(UnalignedMat3x3::m21)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M22_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m22),
				sizeof(UnalignedMat3x3::m22)
			),
			vk::SpecializationMapEntry(
				ct_CHROMA_SAMPLE_OFFSET_X_ID,
				offsetof(Impl, colorChromaOffsetX),
				sizeof(Impl::colorChromaOffsetX)
			),
			vk::SpecializationMapEntry(
				ct_CHROMA_SAMPLE_OFFSET_Y_ID,
				offsetof(Impl, colorChromaOffsetY),
				sizeof(Impl::colorChromaOffsetY)
			),
		};

		return fragmentShaderSpecializationMap;
	}

	static Utils::BufferView<const uint32_t> getSPIRV() noexcept {
		static
		#include <color_transfer_read_frag.h>
		return color_transfer_read_frag;
	}

};



/*
 * ColorTransferRead
 */

ColorTransferRead::ColorTransferRead()
	: m_impl({})
{
}

ColorTransferRead::ColorTransferRead(const Frame::Descriptor& desc)
	: m_impl({}, desc)
{
}

ColorTransferRead::ColorTransferRead(ColorTransferRead&& other) = default;

ColorTransferRead::~ColorTransferRead() = default;

ColorTransferRead& ColorTransferRead::operator=(ColorTransferRead&& other) = default;


bool ColorTransferRead::operator==(const ColorTransferRead& rhs) const noexcept {
	assert(size() == rhs.size());
	return std::memcmp(data(), rhs.data(), size()) == 0;
}

bool ColorTransferRead::operator!=(const ColorTransferRead& rhs) const noexcept {
	return !operator==(rhs);
}



size_t ColorTransferRead::size() const noexcept {
	return m_impl->size();
}

std::byte* ColorTransferRead::data() noexcept {
	return m_impl->data();
}

const std::byte* ColorTransferRead::data() const noexcept {
	return m_impl->data();
}



void ColorTransferRead::optimize(	Utils::BufferView<Image::Plane> planes,
									Utils::BufferView<const vk::Format> supportedFormats ) noexcept 
{
	return m_impl->optimize(planes, supportedFormats);
}

void ColorTransferRead::optimize(const Sampler& sampler) noexcept {
	return m_impl->optimize(sampler);
}

bool ColorTransferRead::isPassthough() const noexcept {
	return m_impl->isPassthough();
}

bool ColorTransferRead::isLinear() const noexcept {
	return m_impl->isLinear();
}

size_t ColorTransferRead::getPlaneCount() const noexcept {
	return m_impl->getPlaneCount();
}



vk::DescriptorSetLayout ColorTransferRead::createDescriptorSetLayout(	const Vulkan& vulkan,
																		const Sampler& sampler ) const
{
	return m_impl->createDescriptorSetLayout(vulkan, sampler);
}

Utils::BufferView<const vk::SpecializationMapEntry> ColorTransferRead::getSpecializationMap() noexcept {
	return Impl::getSpecializationMap();
}

Utils::BufferView<const uint32_t> ColorTransferRead::getSPIRV() noexcept {
	return Impl::getSPIRV();
}

size_t ColorTransferRead::getSamplerBinding() noexcept {
	return ct_SAMPLER_BINDING;
}





/*
 * ColorTransferWrite::Impl
 */

struct ColorTransferWrite::Impl {
	uint32_t 		planeCount;
	uint32_t 		planeFormat;
	uint32_t 		colorRange;
	uint32_t 		colorModel;
	uint32_t 		colorTransferFunction;
	UnalignedMat3x3	colorModelConversion;

	Impl() 
		: planeCount(1)
		, planeFormat(ct_PLANE_FORMAT_RGBA)
		, colorRange(ct_COLOR_RANGE_FULL_RGB)
		, colorModel(ct_COLOR_MODEL_RGB)
		, colorTransferFunction(ct_COLOR_TRANSFER_FUNCTION_LINEAR)
		, colorModelConversion(Math::Mat3x3f(1.0f))
	{
		assert(isPassthough());
	}

	Impl(const Frame::Descriptor& desc)
		: planeCount(Zuazo::getPlaneCount(desc.getColorFormat()))
		, planeFormat(getPlaneFormat(desc.getColorFormat()))
		, colorRange(getColorRange(desc.getColorRange(), desc.getColorModel()))
		, colorModel(getColorModel(desc.getColorModel()))
		, colorTransferFunction(getColorTransferFunction(desc.getColorTransferFunction()))
		, colorModelConversion(getRGB2YCbCrConversionMatrix(desc.getColorModel()))
	{
	}

	~Impl() = default;



	size_t size() const noexcept {
		return sizeof(*this);
	}

	std::byte* data() noexcept {
		return reinterpret_cast<std::byte*>(this);
	}

	const std::byte* data() const noexcept {
		return reinterpret_cast<const std::byte*>(this);
	}



	void optimize(	Utils::BufferView<Image::Plane> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) noexcept
	{
		colorTransferFunction = optimizeColorTransferFunction(
			colorRange,
			colorModel,
			colorTransferFunction,
			planes,
			supportedFormats
		);
	}


	bool isPassthough() const noexcept {
		return 	planeFormat == ct_PLANE_FORMAT_RGBA &&
				colorRange == ct_COLOR_RANGE_FULL_RGB &&
				colorModel == ct_COLOR_MODEL_RGB &&
				colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_LINEAR ;
	}
	
	bool isLinear() const noexcept {
		return colorTransferFunction == ct_COLOR_TRANSFER_FUNCTION_LINEAR;
	}

	size_t getPlaneCount() const noexcept {
		return planeCount;
	}



	static vk::DescriptorSetLayout createDescriptorSetLayout(const Vulkan& vulkan) {
		static const Utils::StaticId id;

		//Try to retrive the layout from cache
		auto result = vulkan.createDescriptorSetLayout(id);
		if(!result) {
			const std::array<vk::DescriptorSetLayoutBinding, 1> bindings = {
				vk::DescriptorSetLayoutBinding(
					ct_SAMPLER_BINDING,
					vk::DescriptorType::eInputAttachment,
					1,
					vk::ShaderStageFlagBits::eFragment,
					nullptr
				)
			};

			const vk::DescriptorSetLayoutCreateInfo createInfo(
				{},
				bindings.size(), bindings.data()
			);

			result = vulkan.createDescriptorSetLayout(id, createInfo);
		}
		assert(result);

		return result;
	}

	static Utils::BufferView<const vk::SpecializationMapEntry> getSpecializationMap() noexcept {
		static const std::array<vk::SpecializationMapEntry, 14> fragmentShaderSpecializationMap = {
			vk::SpecializationMapEntry(
				ct_PLANE_COUNT_ID,
				offsetof(Impl, planeCount),
				sizeof(Impl::planeCount)
			),
			vk::SpecializationMapEntry(
				ct_PLANE_FORMAT_ID,
				offsetof(Impl, planeFormat),
				sizeof(Impl::planeFormat)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_RANGE_ID,
				offsetof(Impl, colorRange),
				sizeof(Impl::colorRange)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_ID,
				offsetof(Impl, colorModel),
				sizeof(Impl::colorModel)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_TRANSFER_FUNCTION_ID,
				offsetof(Impl, colorTransferFunction),
				sizeof(Impl::colorTransferFunction)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M00_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m00),
				sizeof(UnalignedMat3x3::m00)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M01_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m01),
				sizeof(UnalignedMat3x3::m01)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M02_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m02),
				sizeof(UnalignedMat3x3::m02)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M10_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m10),
				sizeof(UnalignedMat3x3::m10)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M11_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m11),
				sizeof(UnalignedMat3x3::m11)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M12_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m12),
				sizeof(UnalignedMat3x3::m12)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M20_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m20),
				sizeof(UnalignedMat3x3::m20)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M21_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m21),
				sizeof(UnalignedMat3x3::m21)
			),
			vk::SpecializationMapEntry(
				ct_COLOR_MODEL_MATRIX_BASE_ID + ct_MAT3x3_M22_OFFSET,
				offsetof(Impl, colorModelConversion) + offsetof(UnalignedMat3x3, m22),
				sizeof(UnalignedMat3x3::m22)
			)
		};

		return fragmentShaderSpecializationMap;
	}

	static Utils::BufferView<const uint32_t> getSPIRV() noexcept {
		static
		#include <color_transfer_write_frag.h>
		return color_transfer_write_frag;
	}


};



/*
 * ColorTransferWrite
 */

ColorTransferWrite::ColorTransferWrite()
	: m_impl({})
{
}

ColorTransferWrite::ColorTransferWrite(const Frame::Descriptor& desc)
	: m_impl({}, desc)
{
}

ColorTransferWrite::ColorTransferWrite(ColorTransferWrite&& other) = default;

ColorTransferWrite::~ColorTransferWrite() = default;

ColorTransferWrite& ColorTransferWrite::operator=(ColorTransferWrite&& other) = default;


bool ColorTransferWrite::operator==(const ColorTransferWrite& rhs) const noexcept {
	assert(size() == rhs.size());
	return std::memcmp(data(), rhs.data(), size()) == 0;
}

bool ColorTransferWrite::operator!=(const ColorTransferWrite& rhs) const noexcept {
	return !operator==(rhs);
}



size_t ColorTransferWrite::size() const noexcept {
	return m_impl->size();
}

std::byte* ColorTransferWrite::data() noexcept {
	return m_impl->data();
}

const std::byte* ColorTransferWrite::data() const noexcept {
	return m_impl->data();
}


void ColorTransferWrite::optimize(	Utils::BufferView<Image::Plane> planes,
									Utils::BufferView<const vk::Format> supportedFormats ) noexcept 
{
	return m_impl->optimize(planes, supportedFormats);
}


bool ColorTransferWrite::isPassthough() const noexcept {
	return m_impl->isPassthough();
}

bool ColorTransferWrite::isLinear() const noexcept {
	return m_impl->isLinear();
}

size_t ColorTransferWrite::getPlaneCount() const noexcept {
	return m_impl->getPlaneCount();
}



vk::DescriptorSetLayout ColorTransferWrite::createDescriptorSetLayout(const Vulkan& vulkan) {
	return Impl::createDescriptorSetLayout(vulkan);
}

Utils::BufferView<const vk::SpecializationMapEntry> ColorTransferWrite::getSpecializationMap() noexcept {
	return Impl::getSpecializationMap();
}

Utils::BufferView<const uint32_t> ColorTransferWrite::getSPIRV() noexcept {
	return Impl::getSPIRV();
}

size_t ColorTransferWrite::getInputAttachmentBinding() noexcept {
	return ct_SAMPLER_BINDING;
}


}

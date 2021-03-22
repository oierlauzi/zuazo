#include "VulkanConversions.h"

#include "../Macros.h"

#include <utility>
#include <cassert>
#include <algorithm>

namespace Zuazo::Graphics {

constexpr uint32_t toVulkan(Version version) noexcept {
	return VK_MAKE_VERSION(
		version.getMajor(),
		version.getMinor(),
		version.getPatch()
	);
}


constexpr vk::DebugUtilsMessageSeverityFlagsEXT toVulkan(Verbosity ver) noexcept {
	vk::DebugUtilsMessageSeverityFlagsEXT result = {};

	if((ver & Verbosity::VERBOSE) != Verbosity::SILENT)	result |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
	if((ver & Verbosity::INFO) != Verbosity::SILENT) 	result |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
	if((ver & Verbosity::WARNING) != Verbosity::SILENT)	result |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
	if((ver & Verbosity::ERROR) != Verbosity::SILENT) 	result |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

	return result;
}

constexpr Verbosity fromVulkan(vk::DebugUtilsMessageSeverityFlagsEXT ver) noexcept {
	constexpr auto silent = vk::DebugUtilsMessageSeverityFlagsEXT();
	Verbosity result = Verbosity::SILENT;

	if((ver & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) != silent)	result |= Verbosity::VERBOSE;
	if((ver & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) != silent)		result |= Verbosity::INFO;
	if((ver & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) != silent)	result |= Verbosity::WARNING;
	if((ver & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) != silent)		result |= Verbosity::ERROR;
	
	

	return result;
}	


constexpr vk::DebugUtilsMessageSeverityFlagBitsEXT toVulkan(Severity sev) noexcept {
	constexpr auto FAILURE = static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(0);

	switch(sev) {
	case Severity::VERBOSE:	return vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
	case Severity::INFO:	return vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
	case Severity::WARNING:	return vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
	case Severity::ERROR:	return vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	default: return FAILURE;
	}
}

constexpr Severity fromVulkan(vk::DebugUtilsMessageSeverityFlagBitsEXT sev) noexcept {
	constexpr auto FAILURE = static_cast<Severity>(0);

	switch(sev) {
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:	return Severity::VERBOSE;
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:		return Severity::INFO;
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:	return Severity::WARNING;
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:		return Severity::ERROR;
	default: return FAILURE;
	}
}


constexpr vk::Extent2D toVulkan(Resolution res) noexcept {
	return vk::Extent2D(
		res.width,
		res.height
	);
}

constexpr Resolution fromVulkan(vk::Extent2D res) noexcept {
	return Resolution(
		res.width,
		res.height
	);
}

constexpr vk::Extent2D to2D(vk::Extent3D ext) noexcept {
	return vk::Extent2D(ext.width, ext.height);
}

constexpr vk::Extent3D to3D(vk::Extent2D ext) noexcept {
	return vk::Extent3D(ext.width, ext.height, 1);
}




constexpr vk::SamplerYcbcrModelConversion toVulkan(ColorModel colorModel) noexcept {
	switch(colorModel) {
	case ColorModel::RGB:		return vk::SamplerYcbcrModelConversion::eRgbIdentity;
	case ColorModel::BT601:		return vk::SamplerYcbcrModelConversion::eYcbcr601;
	case ColorModel::BT709:		return vk::SamplerYcbcrModelConversion::eYcbcr709;
	case ColorModel::BT2020:	return vk::SamplerYcbcrModelConversion::eYcbcr2020;
	default:					return static_cast<vk::SamplerYcbcrModelConversion>(-1);
	}
}

constexpr ColorModel fromVulkan(vk::SamplerYcbcrModelConversion model) noexcept {
	switch(model) {
	case vk::SamplerYcbcrModelConversion::eRgbIdentity:	return ColorModel::RGB;
	case vk::SamplerYcbcrModelConversion::eYcbcr601:	return ColorModel::BT601;
	case vk::SamplerYcbcrModelConversion::eYcbcr709:	return ColorModel::BT709;
	case vk::SamplerYcbcrModelConversion::eYcbcr2020:	return ColorModel::BT2020;
	default:											return ColorModel::NONE;
	}
}


constexpr vk::SamplerYcbcrRange toVulkan(ColorRange colorRange) noexcept {
	switch(colorRange) {
	case ColorRange::FULL:			return vk::SamplerYcbcrRange::eItuFull;
	case ColorRange::ITU_NARROW:	return vk::SamplerYcbcrRange::eItuNarrow;
	default:						return static_cast<vk::SamplerYcbcrRange>(-1);
	}
}

constexpr ColorRange fromVulkan(vk::SamplerYcbcrRange range) noexcept {
	switch(range) {
	case vk::SamplerYcbcrRange::eItuFull:	return ColorRange::FULL;
	case vk::SamplerYcbcrRange::eItuNarrow:	return ColorRange::ITU_NARROW;
	default:								return ColorRange::NONE;
	}
}



constexpr std::array<std::tuple<vk::Format, vk::ComponentMapping>, MAX_PLANE_COUNT> toVulkan(ColorFormat fmt) noexcept {
	switch(fmt){
	//4 bit compoents
	case ColorFormat::Y4X4_8: 			return { std::tuple{ vk::Format::eR4G4UnormPack8, swizzle("RRR1") } };
	case ColorFormat::X4Y4_8:			return { std::tuple{ vk::Format::eR4G4UnormPack8, swizzle("GGG1") } };
	case ColorFormat::Y4A4_8: 			return { std::tuple{ vk::Format::eR4G4UnormPack8, swizzle("RRRG") } };
	case ColorFormat::A4Y4_8:			return { std::tuple{ vk::Format::eR4G4UnormPack8, swizzle("GGGR") } };

	case ColorFormat::X4R4G4B4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "XRGB") } };
	case ColorFormat::X4B4G4R4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "XBGR") } };
	case ColorFormat::X4G4R4B4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "XGBR") } };
	case ColorFormat::X4G4B4R4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "XGBR") } };
	case ColorFormat::R4G4B4X4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "RGBX") } };
	case ColorFormat::B4G4R4X4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "BGRX") } };
	case ColorFormat::G4R4B4X4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "GRBX") } };
	case ColorFormat::G4B4R4X4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "GBRX") } };

	case ColorFormat::A4R4G4B4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "ARGB") } };
	case ColorFormat::A4B4G4R4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "ABGR") } };
	case ColorFormat::A4G4R4B4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "AGRB") } };
	case ColorFormat::A4G4B4R4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "AGBR") } };
	case ColorFormat::R4G4B4A4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B4G4R4A4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G4R4B4A4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G4B4R4A4_16:		return { std::tuple{ vk::Format::eR4G4B4A4UnormPack16, swizzle("RGBA", "GBRA") } };

	//5 bit components
	case ColorFormat::X1R5G5B5_16:		return { std::tuple{ vk::Format::eA1R5G5B5UnormPack16, swizzle("ARGB", "XRGB") } };
	case ColorFormat::X1B5G5R5_16:		return { std::tuple{ vk::Format::eA1R5G5B5UnormPack16, swizzle("ARGB", "XBGR") } };
	case ColorFormat::X1G5R5B5_16:		return { std::tuple{ vk::Format::eA1R5G5B5UnormPack16, swizzle("ARGB", "XGRB") } };
	case ColorFormat::X1G5B5R5_16:		return { std::tuple{ vk::Format::eA1R5G5B5UnormPack16, swizzle("ARGB", "XGBR") } };
	case ColorFormat::R5G5B5X1_16:		return { std::tuple{ vk::Format::eR5G5B5A1UnormPack16, swizzle("RGBA", "RGBX") } };
	case ColorFormat::B5G5R5X1_16:		return { std::tuple{ vk::Format::eR5G5B5A1UnormPack16, swizzle("RGBA", "BGRX") } };
	case ColorFormat::G5R5B5X1_16:		return { std::tuple{ vk::Format::eR5G5B5A1UnormPack16, swizzle("RGBA", "GRBX") } };
	case ColorFormat::G5B5R5X1_16:		return { std::tuple{ vk::Format::eR5G5B5A1UnormPack16, swizzle("RGBA", "GBRX") } };

	case ColorFormat::A1R5G5B5_16:		return { std::tuple{ vk::Format::eA1R5G5B5UnormPack16, swizzle("ARGB", "ARGB")  } };
	case ColorFormat::A1B5G5R5_16:		return { std::tuple{ vk::Format::eA1R5G5B5UnormPack16, swizzle("ARGB", "ABGR") } };
	case ColorFormat::A1G5R5B5_16:		return { std::tuple{ vk::Format::eA1R5G5B5UnormPack16, swizzle("ARGB", "AGRB") } };
	case ColorFormat::A1G5B5R5_16:		return { std::tuple{ vk::Format::eA1R5G5B5UnormPack16, swizzle("ARGB", "AGBR") } };
	case ColorFormat::R5G5B5A1_16:		return { std::tuple{ vk::Format::eR5G5B5A1UnormPack16, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B5G5R5A1_16:		return { std::tuple{ vk::Format::eR5G5B5A1UnormPack16, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G5R5B5A1_16:		return { std::tuple{ vk::Format::eR5G5B5A1UnormPack16, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G5B5R5A1_16:		return { std::tuple{ vk::Format::eR5G5B5A1UnormPack16, swizzle("RGBA", "GBRA") } };

	//6 bit components
	case ColorFormat::R5G6B5_16:		return { std::tuple{ vk::Format::eR5G6B5UnormPack16, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B5G6R5_16:		return { std::tuple{ vk::Format::eR5G6B5UnormPack16, swizzle("RGBA", "BGRA") } };

	//8 bit components
	case ColorFormat::Y8:				return { std::tuple{ vk::Format::eR8Unorm, swizzle("RRR1") } };

	case ColorFormat::Y8A8:				return { std::tuple{ vk::Format::eR8G8Unorm, swizzle("RRRG") } };
	case ColorFormat::A8Y8:				return { std::tuple{ vk::Format::eR8G8Unorm, swizzle("GGGR") } };

	case ColorFormat::R8G8B8:			return { std::tuple{ vk::Format::eR8G8B8Unorm, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B8G8R8:			return { std::tuple{ vk::Format::eR8G8B8Unorm, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G8R8B8:			return { std::tuple{ vk::Format::eR8G8B8Unorm, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G8B8R8:			return { std::tuple{ vk::Format::eR8G8B8Unorm, swizzle("RGBA", "GBRA") } };

	case ColorFormat::A8R8G8B8:			return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "ARGB") } };
	case ColorFormat::A8B8G8R8:			return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "ABGR") } };
	case ColorFormat::A8G8R8B8:			return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "AGRB") } };
	case ColorFormat::A8G8B8R8:			return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "AGBR") } };
	case ColorFormat::R8G8B8A8:			return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B8G8R8A8:			return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G8R8B8A8:			return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G8B8R8A8:			return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "GBRA") } };

	case ColorFormat::X8R8G8B8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", Utils::bele("XRGB", "BGRX")) } };
	case ColorFormat::X8B8G8R8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", Utils::bele("XBGR", "RGBX")) } };
	case ColorFormat::R8G8B8X8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", Utils::bele("RGBX", "XBGR")) } };
	case ColorFormat::B8G8R8X8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", Utils::bele("BGRX", "XRGB")) } };

	case ColorFormat::A8R8G8B8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", Utils::bele("ARGB", "BGRA")) } };
	case ColorFormat::A8B8G8R8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", Utils::bele("ABGR", "RGBA")) } };
	case ColorFormat::R8G8B8A8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", Utils::bele("RGBA", "ABGR")) } };
	case ColorFormat::B8G8R8A8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", Utils::bele("BGRA", "ARGB")) } };

	case ColorFormat::R8G8B8G8:			return { std::tuple{ vk::Format::eB8G8R8G8422Unorm, swizzle("RGBA", "BGRA") } };
	case ColorFormat::B8G8R8G8:			return { std::tuple{ vk::Format::eB8G8R8G8422Unorm, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::G8R8G8B8:			return { std::tuple{ vk::Format::eG8B8G8R8422Unorm, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G8B8G8R8:			return { std::tuple{ vk::Format::eG8B8G8R8422Unorm, swizzle("RGBA", "RGBA")  } };

	case ColorFormat::G8_B8_R8_A8:		return { 
		std::tuple{ vk::Format::eR8Unorm, swizzle("") },
		std::tuple{ vk::Format::eR8Unorm, swizzle("") },
		std::tuple{ vk::Format::eR8Unorm, swizzle("") },
		std::tuple{ vk::Format::eR8Unorm, swizzle("") } };
	case ColorFormat::G8_B8_R8:			return { 
		std::tuple{ vk::Format::eR8Unorm, swizzle("") },
		std::tuple{ vk::Format::eR8Unorm, swizzle("") },
		std::tuple{ vk::Format::eR8Unorm, swizzle("") } };
	case ColorFormat::G8_B8R8:			return { 
		std::tuple{ vk::Format::eR8Unorm, swizzle("") },
		std::tuple{ vk::Format::eR8G8Unorm, swizzle("") } };
	case ColorFormat::G8_R8B8:			return {
		std::tuple{ vk::Format::eR8Unorm, swizzle("") },
		std::tuple{ vk::Format::eR8G8Unorm, swizzle("GR") } };

	//10 bit components
	case ColorFormat::Y10X6_16:			return { std::tuple{ vk::Format::eR10X6UnormPack16, swizzle("RRR1") } };

	case ColorFormat::Y10X6A10X6_16:	return { std::tuple{ vk::Format::eR10X6G10X6Unorm2Pack16, swizzle("RRRG") } };
	case ColorFormat::A10X6Y10X6_16:	return { std::tuple{ vk::Format::eR10X6G10X6Unorm2Pack16, swizzle("GGGR") } };

	case ColorFormat::A10X6R10X6G10X6B10X6_16:	return { std::tuple{ vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, swizzle("RGBA", "ARGB") } };
	case ColorFormat::A10X6B10X6G10X6R10X6_16:	return { std::tuple{ vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, swizzle("RGBA", "ABGR") } };
	case ColorFormat::A10X6G10X6R10X6B10X6_16:	return { std::tuple{ vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, swizzle("RGBA", "AGRB") } };
	case ColorFormat::A10X6G10X6B10X6R10X6_16:	return { std::tuple{ vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, swizzle("RGBA", "AGBR") } };
	case ColorFormat::R10X6G10X6B10X6A10X6_16:	return { std::tuple{ vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B10X6G10X6R10X6A10X6_16:	return { std::tuple{ vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G10X6R10X6B10X6A10X6_16:	return { std::tuple{ vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G10X6B10X6R10X6A10X6_16:	return { std::tuple{ vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16, swizzle("RGBA", "GBRA") } };

	case ColorFormat::X2R10G10B10_32:	return { std::tuple{ vk::Format::eA2R10G10B10UnormPack32, swizzle("ARGB", "XRGB") } };
	case ColorFormat::X2B10G10R10_32:	return { std::tuple{ vk::Format::eA2R10G10B10UnormPack32, swizzle("ARGB", "XBGR") } };
	case ColorFormat::X2G10R10B10_32:	return { std::tuple{ vk::Format::eA2R10G10B10UnormPack32, swizzle("ARGB", "XGRB") } };
	case ColorFormat::X2G10B10R10_32:	return { std::tuple{ vk::Format::eA2R10G10B10UnormPack32, swizzle("ARGB", "XGBR") } };

	case ColorFormat::A2R10G10B10_32:	return { std::tuple{ vk::Format::eA2R10G10B10UnormPack32, swizzle("ARGB", "ARGB")  } };
	case ColorFormat::A2B10G10R10_32:	return { std::tuple{ vk::Format::eA2R10G10B10UnormPack32, swizzle("ARGB", "ABGR") } };
	case ColorFormat::A2G10R10B10_32:	return { std::tuple{ vk::Format::eA2R10G10B10UnormPack32, swizzle("ARGB", "AGRB") } };
	case ColorFormat::A2G10B10R10_32:	return { std::tuple{ vk::Format::eA2R10G10B10UnormPack32, swizzle("ARGB", "AGBR") } };

	case ColorFormat::R10X6G10X6B10X6G10X6_16:	return { std::tuple{ vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16, swizzle("RGBA", "BGRA") } };
	case ColorFormat::B10X6G10X6R10X6G10X6_16:	return { std::tuple{ vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::G10X6R10X6G10X6B10X6_16:	return { std::tuple{ vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G10X6B10X6G10X6R10X6_16:	return { std::tuple{ vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16, swizzle("RGBA", "RGBA")  } };

	case ColorFormat::G10X6_B10X6_R10X6_A10X6_16: return { 
		std::tuple{ vk::Format::eR10X6UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR10X6UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR10X6UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR10X6UnormPack16, swizzle("") } };
	case ColorFormat::G10X6_B10X6_R10X6_16:	return { 
		std::tuple{ vk::Format::eR10X6UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR10X6UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR10X6UnormPack16, swizzle("") } };
	case ColorFormat::G10X6_B10X6R10X6_16:		return { 
		std::tuple{ vk::Format::eR10X6UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR10X6G10X6Unorm2Pack16, swizzle("") } };
	case ColorFormat::G10X6_R10X6B10X6_16:		return {
		std::tuple{ vk::Format::eR10X6UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR10X6G10X6Unorm2Pack16, swizzle("GR") } };
	
	//12 bits per component
	case ColorFormat::Y12X4_16:			return { std::tuple{ vk::Format::eR12X4UnormPack16, swizzle("RRR1") } };

	case ColorFormat::Y12X4A12X4_16:	return { std::tuple{ vk::Format::eR12X4G12X4Unorm2Pack16, swizzle("RRRG") } };
	case ColorFormat::A12X4Y12X4_16:	return { std::tuple{ vk::Format::eR12X4G12X4Unorm2Pack16, swizzle("GGGR") } };

	case ColorFormat::A12X4R12X4G12X4B12X4_16:	return { std::tuple{ vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, swizzle("RGBA", "ARGB") } };
	case ColorFormat::A12X4B12X4G12X4R12X4_16:	return { std::tuple{ vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, swizzle("RGBA", "ABGR") } };
	case ColorFormat::A12X4G12X4R12X4B12X4_16:	return { std::tuple{ vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, swizzle("RGBA", "AGRB") } };
	case ColorFormat::A12X4G12X4B12X4R12X4_16:	return { std::tuple{ vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, swizzle("RGBA", "AGBR") } };
	case ColorFormat::R12X4G12X4B12X4A12X4_16:	return { std::tuple{ vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B12X4G12X4R12X4A12X4_16:	return { std::tuple{ vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G12X4R12X4B12X4A12X4_16:	return { std::tuple{ vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G12X4B12X4R12X4A12X4_16:	return { std::tuple{ vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16, swizzle("RGBA", "GBRA") } };

	case ColorFormat::R12X4G12X4B12X4G12X4_16:	return { std::tuple{ vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16, swizzle("RGBA", "BGRA") } };
	case ColorFormat::B12X4G12X4R12X4G12X4_16:	return { std::tuple{ vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::G12X4R12X4G12X4B12X4_16:	return { std::tuple{ vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G12X4B12X4G12X4R12X4_16:	return { std::tuple{ vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16, swizzle("RGBA", "RGBA")  } };


	case ColorFormat::G12X4_B12X4_R12X4_A12X4_16: return { 
		std::tuple{ vk::Format::eR12X4UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR12X4UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR12X4UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR12X4UnormPack16, swizzle("") } };
	case ColorFormat::G12X4_B12X4_R12X4_16:	return { 
		std::tuple{ vk::Format::eR12X4UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR12X4UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR12X4UnormPack16, swizzle("") } };
	case ColorFormat::G12X4_B12X4R12X4_16:		return { 
		std::tuple{ vk::Format::eR12X4UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR12X4G12X4Unorm2Pack16, swizzle("") } };
	case ColorFormat::G12X4_R12X4B12X4_16:		return {
		std::tuple{ vk::Format::eR12X4UnormPack16, swizzle("") },
		std::tuple{ vk::Format::eR12X4G12X4Unorm2Pack16, swizzle("GR") } };

	//16 bits per component
	case ColorFormat::Y16:				return { std::tuple{ vk::Format::eR16Unorm, swizzle("RRR1") } };

	case ColorFormat::Y16f:				return { std::tuple{ vk::Format::eR16Sfloat, swizzle("RRR1") } };

	case ColorFormat::Y16A16:			return { std::tuple{ vk::Format::eR16G16Unorm, swizzle("RRRG") } };
	case ColorFormat::A16Y16:			return { std::tuple{ vk::Format::eR16G16Unorm, swizzle("GGGR") } };

	case ColorFormat::Y16fA16f:			return { std::tuple{ vk::Format::eR16G16Sfloat, swizzle("RRRG") } };
	case ColorFormat::A16fY16f:			return { std::tuple{ vk::Format::eR16G16Sfloat, swizzle("GGGR") } };
		
	case ColorFormat::R16G16B16:		return { std::tuple{ vk::Format::eR16G16B16Unorm, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B16G16R16:		return { std::tuple{ vk::Format::eR16G16B16Unorm, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G16R16B16:		return { std::tuple{ vk::Format::eR16G16B16Unorm, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G16B16R16:		return { std::tuple{ vk::Format::eR16G16B16Unorm, swizzle("RGBA", "GBRA") } };

	case ColorFormat::R16fG16fB16f:		return { std::tuple{ vk::Format::eR16G16B16Sfloat, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B16fG16fR16f:		return { std::tuple{ vk::Format::eR16G16B16Sfloat, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G16fR16fB16f:		return { std::tuple{ vk::Format::eR16G16B16Sfloat, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G16fB16fR16f:		return { std::tuple{ vk::Format::eR16G16B16Sfloat, swizzle("RGBA", "GBRA") } };

	case ColorFormat::A16R16G16B16:		return { std::tuple{ vk::Format::eR16G16B16A16Unorm, swizzle("RGBA", "ARGB") } };
	case ColorFormat::A16B16G16R16:		return { std::tuple{ vk::Format::eR16G16B16A16Unorm, swizzle("RGBA", "ABGR") } };
	case ColorFormat::A16G16R16B16:		return { std::tuple{ vk::Format::eR16G16B16A16Unorm, swizzle("RGBA", "AGRB") } };
	case ColorFormat::A16G16B16R16:		return { std::tuple{ vk::Format::eR16G16B16A16Unorm, swizzle("RGBA", "AGBR") } };
	case ColorFormat::R16G16B16A16:		return { std::tuple{ vk::Format::eR16G16B16A16Unorm, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B16G16R16A16:		return { std::tuple{ vk::Format::eR16G16B16A16Unorm, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G16R16B16A16:		return { std::tuple{ vk::Format::eR16G16B16A16Unorm, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G16B16R16A16:		return { std::tuple{ vk::Format::eR16G16B16A16Unorm, swizzle("RGBA", "GBRA") } };

	case ColorFormat::A16fR16fG16fB16f:	return { std::tuple{ vk::Format::eR16G16B16A16Sfloat, swizzle("RGBA", "ARGB") } };
	case ColorFormat::A16fB16fG16fR16f:	return { std::tuple{ vk::Format::eR16G16B16A16Sfloat, swizzle("RGBA", "ABGR") } };
	case ColorFormat::A16fG16fR16fB16f:	return { std::tuple{ vk::Format::eR16G16B16A16Sfloat, swizzle("RGBA", "AGRB") } };
	case ColorFormat::A16fG16fB16fR16f:	return { std::tuple{ vk::Format::eR16G16B16A16Sfloat, swizzle("RGBA", "AGBR") } };
	case ColorFormat::R16fG16fB16fA16f:	return { std::tuple{ vk::Format::eR16G16B16A16Sfloat, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B16fG16fR16fA16f:	return { std::tuple{ vk::Format::eR16G16B16A16Sfloat, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G16fR16fB16fA16f:	return { std::tuple{ vk::Format::eR16G16B16A16Sfloat, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G16fB16fR16fA16f:	return { std::tuple{ vk::Format::eR16G16B16A16Sfloat, swizzle("RGBA", "GBRA") } };

	case ColorFormat::R16G16B16G16:		return { std::tuple{ vk::Format::eB16G16R16G16422Unorm, swizzle("RGBA", "BGRA") } };
	case ColorFormat::B16G16R16G16:		return { std::tuple{ vk::Format::eB16G16R16G16422Unorm, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::G16R16G16B16:		return { std::tuple{ vk::Format::eG16B16G16R16422Unorm, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G16B16G16R16:		return { std::tuple{ vk::Format::eG16B16G16R16422Unorm, swizzle("RGBA", "RGBA")  } };


	case ColorFormat::G16_B16_R16_A16: return { 
		std::tuple{ vk::Format::eR16Unorm, swizzle("") },
		std::tuple{ vk::Format::eR16Unorm, swizzle("") },
		std::tuple{ vk::Format::eR16Unorm, swizzle("") },
		std::tuple{ vk::Format::eR16Unorm, swizzle("") } };
	case ColorFormat::G16_B16_R16:			return { 
		std::tuple{ vk::Format::eR16Unorm, swizzle("") },
		std::tuple{ vk::Format::eR16Unorm, swizzle("") },
		std::tuple{ vk::Format::eR16Unorm, swizzle("") } };
	case ColorFormat::G16_B16R16:			return { 
		std::tuple{ vk::Format::eR16Unorm, swizzle("") },
		std::tuple{ vk::Format::eR16G16Unorm, swizzle("") } };
	case ColorFormat::G16_R16B16:			return {
		std::tuple{ vk::Format::eR16Unorm, swizzle("") },
		std::tuple{ vk::Format::eR16G16Unorm, swizzle("GR") } };

	case ColorFormat::G16f_B16f_R16f_A16f: return { 
		std::tuple{ vk::Format::eR16Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR16Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR16Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR16Sfloat, swizzle("") } };
	case ColorFormat::G16f_B16f_R16f: return { 
		std::tuple{ vk::Format::eR16Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR16Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR16Sfloat, swizzle("") } };


	//32 bits per component
	case ColorFormat::Y32f:				return { std::tuple{ vk::Format::eR32Sfloat, swizzle("RRR1") } };

	case ColorFormat::Y32fA32f:			return { std::tuple{ vk::Format::eR32G32Sfloat, swizzle("RRRG") } };
	case ColorFormat::A32fY32f:			return { std::tuple{ vk::Format::eR32G32Sfloat, swizzle("GGGR") } };
		
	case ColorFormat::R32fG32fB32f:		return { std::tuple{ vk::Format::eR32G32B32Sfloat, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B32fG32fR32f:		return { std::tuple{ vk::Format::eR32G32B32Sfloat, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G32fR32fB32f:		return { std::tuple{ vk::Format::eR32G32B32Sfloat, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G32fB32fR32f:		return { std::tuple{ vk::Format::eR32G32B32Sfloat, swizzle("RGBA", "GBRA") } };

	case ColorFormat::A32fR32fG32fB32f:	return { std::tuple{ vk::Format::eR32G32B32A32Sfloat, swizzle("RGBA", "ARGB") } };
	case ColorFormat::A32fB32fG32fR32f:	return { std::tuple{ vk::Format::eR32G32B32A32Sfloat, swizzle("RGBA", "ABGR") } };
	case ColorFormat::A32fG32fR32fB32f:	return { std::tuple{ vk::Format::eR32G32B32A32Sfloat, swizzle("RGBA", "AGRB") } };
	case ColorFormat::A32fG32fB32fR32f:	return { std::tuple{ vk::Format::eR32G32B32A32Sfloat, swizzle("RGBA", "AGBR") } };
	case ColorFormat::R32fG32fB32fA32f:	return { std::tuple{ vk::Format::eR32G32B32A32Sfloat, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B32fG32fR32fA32f:	return { std::tuple{ vk::Format::eR32G32B32A32Sfloat, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G32fR32fB32fA32f:	return { std::tuple{ vk::Format::eR32G32B32A32Sfloat, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G32fB32fR32fA32f:	return { std::tuple{ vk::Format::eR32G32B32A32Sfloat, swizzle("RGBA", "GBRA") } };

	case ColorFormat::G32f_B32f_R32f_A32f: return { 
		std::tuple{ vk::Format::eR32Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR32Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR32Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR32Sfloat, swizzle("") } };
	case ColorFormat::G32f_B32f_R32f: return { 
		std::tuple{ vk::Format::eR32Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR32Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR32Sfloat, swizzle("") } };


	//64 bits per component
	case ColorFormat::Y64f:				return { std::tuple{ vk::Format::eR64Sfloat, swizzle("RRR1") } };

	case ColorFormat::Y64fA64f:			return { std::tuple{ vk::Format::eR64G64Sfloat, swizzle("RRRG") } };
	case ColorFormat::A64fY64f:			return { std::tuple{ vk::Format::eR64G64Sfloat, swizzle("GGGR") } };
		
	case ColorFormat::R64fG64fB64f:		return { std::tuple{ vk::Format::eR64G64B64Sfloat, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B64fG64fR64f:		return { std::tuple{ vk::Format::eR64G64B64Sfloat, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G64fR64fB64f:		return { std::tuple{ vk::Format::eR64G64B64Sfloat, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G64fB64fR64f:		return { std::tuple{ vk::Format::eR64G64B64Sfloat, swizzle("RGBA", "GBRA") } };

	case ColorFormat::A64fR64fG64fB64f:	return { std::tuple{ vk::Format::eR64G64B64A64Sfloat, swizzle("RGBA", "ARGB") } };
	case ColorFormat::A64fB64fG64fR64f:	return { std::tuple{ vk::Format::eR64G64B64A64Sfloat, swizzle("RGBA", "ABGR") } };
	case ColorFormat::A64fG64fR64fB64f:	return { std::tuple{ vk::Format::eR64G64B64A64Sfloat, swizzle("RGBA", "AGRB") } };
	case ColorFormat::A64fG64fB64fR64f:	return { std::tuple{ vk::Format::eR64G64B64A64Sfloat, swizzle("RGBA", "AGBR") } };
	case ColorFormat::R64fG64fB64fA64f:	return { std::tuple{ vk::Format::eR64G64B64A64Sfloat, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B64fG64fR64fA64f:	return { std::tuple{ vk::Format::eR64G64B64A64Sfloat, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G64fR64fB64fA64f:	return { std::tuple{ vk::Format::eR64G64B64A64Sfloat, swizzle("RGBA", "GRBA") } };
	case ColorFormat::G64fB64fR64fA64f:	return { std::tuple{ vk::Format::eR64G64B64A64Sfloat, swizzle("RGBA", "GBRA") } };

	case ColorFormat::G64f_B64f_R64f_A64f: return { 
		std::tuple{ vk::Format::eR64Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR64Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR64Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR64Sfloat, swizzle("") } };
	case ColorFormat::G64f_B64f_R64f: return { 
		std::tuple{ vk::Format::eR64Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR64Sfloat, swizzle("") },
		std::tuple{ vk::Format::eR64Sfloat, swizzle("") } };


	default: return {};
	}
}



constexpr vk::Format toVulkan(DepthStencilFormat fmt) noexcept {
	switch(fmt) {
	case DepthStencilFormat::D16:		return vk::Format::eD16Unorm;
	case DepthStencilFormat::X8D24_32:	return vk::Format::eX8D24UnormPack32;
	case DepthStencilFormat::D32f:		return vk::Format::eD32Sfloat;
	case DepthStencilFormat::S8:		return vk::Format::eS8Uint;
	case DepthStencilFormat::D16S8:		return vk::Format::eD16UnormS8Uint;
	case DepthStencilFormat::D24S8:		return vk::Format::eD24UnormS8Uint;
	case DepthStencilFormat::D32fS8:	return vk::Format::eD32SfloatS8Uint;
	default:							return vk::Format::eUndefined;
	}
}

constexpr DepthStencilFormat fromVulkanDepthStencil(vk::Format fmt) noexcept {
	switch(fmt) {
	case vk::Format::eD16Unorm:			return DepthStencilFormat::D16;
	case vk::Format::eX8D24UnormPack32:	return DepthStencilFormat::X8D24_32;
	case vk::Format::eD32Sfloat:		return DepthStencilFormat::D32f;
	case vk::Format::eS8Uint:			return DepthStencilFormat::S8;
	case vk::Format::eD16UnormS8Uint:	return DepthStencilFormat::D16S8;
	case vk::Format::eD24UnormS8Uint:	return DepthStencilFormat::D24S8;
	case vk::Format::eD32SfloatS8Uint:	return DepthStencilFormat::D32fS8;
	default:							return DepthStencilFormat::NONE;
	}
}



constexpr std::tuple<ColorFormat, ColorTransferFunction> fromVulkan(vk::Format fmt) noexcept {
	switch(fmt){
	//4 bit formats
	case vk::Format::eR4G4B4A4UnormPack16: 	return { ColorFormat::R4G4B4A4_16, ColorTransferFunction::LINEAR };
	case vk::Format::eB4G4R4A4UnormPack16: 	return { ColorFormat::B4G4R4A4_16, ColorTransferFunction::LINEAR };

	//5 bits formats
	case vk::Format::eR5G5B5A1UnormPack16: 	return { ColorFormat::R5G5B5A1_16, ColorTransferFunction::LINEAR };
	case vk::Format::eB5G5R5A1UnormPack16: 	return { ColorFormat::B5G5R5A1_16, ColorTransferFunction::LINEAR };
	case vk::Format::eA1R5G5B5UnormPack16: 	return { ColorFormat::A1R5G5B5_16, ColorTransferFunction::LINEAR };

	//6 bit formats
	case vk::Format::eR5G6B5UnormPack16:	return { ColorFormat::R5G6B5_16, ColorTransferFunction::LINEAR };
	case vk::Format::eB5G6R5UnormPack16:	return { ColorFormat::B5G6R5_16, ColorTransferFunction::LINEAR };

	//8bit formats
	case vk::Format::eR8G8B8Unorm:			return { ColorFormat::R8G8B8, ColorTransferFunction::LINEAR };
	case vk::Format::eR8G8B8Srgb:			return { ColorFormat::R8G8B8, ColorTransferFunction::IEC61966_2_1 };
	case vk::Format::eB8G8R8Unorm:			return { ColorFormat::B8G8R8, ColorTransferFunction::LINEAR };
	case vk::Format::eB8G8R8Srgb:			return { ColorFormat::B8G8R8, ColorTransferFunction::IEC61966_2_1 };
	case vk::Format::eR8G8B8A8Unorm:		return { ColorFormat::R8G8B8A8, ColorTransferFunction::LINEAR };
	case vk::Format::eR8G8B8A8Srgb:			return { ColorFormat::R8G8B8A8, ColorTransferFunction::IEC61966_2_1 };
	case vk::Format::eB8G8R8A8Unorm:		return { ColorFormat::B8G8R8A8, ColorTransferFunction::LINEAR };
	case vk::Format::eB8G8R8A8Srgb:			return { ColorFormat::B8G8R8A8, ColorTransferFunction::IEC61966_2_1 };
	case vk::Format::eG8B8G8R8422Unorm:		return { ColorFormat::G8B8G8R8, ColorTransferFunction::LINEAR };
	case vk::Format::eB8G8R8G8422Unorm:		return { ColorFormat::B8G8R8G8, ColorTransferFunction::LINEAR };

	//10 bit formats
	case vk::Format::eA2R10G10B10UnormPack32:	return { ColorFormat::A2R10G10B10_32, ColorTransferFunction::LINEAR };
	case vk::Format::eA2B10G10R10UnormPack32:	return { ColorFormat::A2B10G10R10_32, ColorTransferFunction::LINEAR };
	case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16:		return { ColorFormat::R10X6G10X6B10X6A10X6_16, ColorTransferFunction::LINEAR };
	case vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16:	return { ColorFormat::G10X6B10X6G10X6R10X6_16, ColorTransferFunction::LINEAR };
	case vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16:	return { ColorFormat::B10X6G10X6R10X6G10X6_16, ColorTransferFunction::LINEAR };

	//12 bit formats
	case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16:		return { ColorFormat::R12X4G12X4B12X4A12X4_16, ColorTransferFunction::LINEAR };
	case vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16:	return { ColorFormat::G12X4B12X4G12X4R12X4_16, ColorTransferFunction::LINEAR };
	case vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16:	return { ColorFormat::B12X4G12X4R12X4G12X4_16, ColorTransferFunction::LINEAR };

	//16 bit formats
	case vk::Format::eR16G16B16Unorm:			return { ColorFormat::R16G16B16, ColorTransferFunction::LINEAR };
	case vk::Format::eR16G16B16Sfloat:			return { ColorFormat::R16fG16fB16f, ColorTransferFunction::LINEAR };
	case vk::Format::eR16G16B16A16Unorm:		return { ColorFormat::R16G16B16A16, ColorTransferFunction::LINEAR };
	case vk::Format::eR16G16B16A16Sfloat:		return { ColorFormat::R16fG16fB16fA16f, ColorTransferFunction::LINEAR };
	case vk::Format::eG16B16G16R16422Unorm:		return { ColorFormat::G16B16G16R16, ColorTransferFunction::LINEAR };
	case vk::Format::eB16G16R16G16422Unorm:		return { ColorFormat::B16G16R16G16, ColorTransferFunction::LINEAR };

	//32 bit formats
	case vk::Format::eR32G32B32Sfloat:		return { ColorFormat::R32fG32fB32f, ColorTransferFunction::LINEAR };
	case vk::Format::eR32G32B32A32Sfloat:	return { ColorFormat::R32fG32fB32fA32f, ColorTransferFunction::LINEAR };

	//64 bit formats
	case vk::Format::eR64G64B64Sfloat:		return { ColorFormat::R64fG64fB64f, ColorTransferFunction::LINEAR };
	case vk::Format::eR64G64B64A64Sfloat:	return { ColorFormat::R64fG64fB64fA64f, ColorTransferFunction::LINEAR };

	default: return {};
	}
}



constexpr std::tuple<vk::Format, vk::ComponentMapping> optimizeFormat(const std::tuple<vk::Format, vk::ComponentMapping>& fmt) noexcept {
	const auto& format = std::get<vk::Format>(fmt);
	const auto& swizzle = std::get<vk::ComponentMapping>(fmt);

	const auto identitySwizzle = vk::ComponentMapping(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		(swizzle.a == vk::ComponentSwizzle::eA) ? vk::ComponentSwizzle::eIdentity : swizzle.a
	);

	//Check if it is identity already
	if(	(swizzle.r == vk::ComponentSwizzle::eIdentity || swizzle.r == vk::ComponentSwizzle::eR) &&
		(swizzle.g == vk::ComponentSwizzle::eIdentity || swizzle.g == vk::ComponentSwizzle::eG) &&
		(swizzle.b == vk::ComponentSwizzle::eIdentity || swizzle.b == vk::ComponentSwizzle::eB) )
	{
		return { format, identitySwizzle };
	}
	
	//Check if it has reversed color
	if(	swizzle.r == vk::ComponentSwizzle::eB &&
		(swizzle.g == vk::ComponentSwizzle::eIdentity || swizzle.g == vk::ComponentSwizzle::eG) &&
		swizzle.b == vk::ComponentSwizzle::eR )
	{
		switch(format){
		case vk::Format::eR4G4B4A4UnormPack16: 	return { vk::Format::eB4G4R4A4UnormPack16, identitySwizzle };
		case vk::Format::eB4G4R4A4UnormPack16: 	return { vk::Format::eR4G4B4A4UnormPack16, identitySwizzle };

		case vk::Format::eR5G6B5UnormPack16: 	return { vk::Format::eB5G6R5UnormPack16, identitySwizzle };
		case vk::Format::eB5G6R5UnormPack16: 	return { vk::Format::eR5G6B5UnormPack16, identitySwizzle };

		case vk::Format::eR5G5B5A1UnormPack16: 	return { vk::Format::eB5G5R5A1UnormPack16, identitySwizzle };
		case vk::Format::eB5G5R5A1UnormPack16: 	return { vk::Format::eR5G5B5A1UnormPack16, identitySwizzle };

		case vk::Format::eR8G8B8Unorm: 			return { vk::Format::eB8G8R8Unorm, identitySwizzle }; 
		case vk::Format::eB8G8R8Unorm: 			return { vk::Format::eR8G8B8Unorm, identitySwizzle };
		case vk::Format::eR8G8B8Snorm: 			return { vk::Format::eB8G8R8Snorm, identitySwizzle }; 
		case vk::Format::eB8G8R8Snorm: 			return { vk::Format::eR8G8B8Snorm, identitySwizzle };
		case vk::Format::eR8G8B8Uscaled:		return { vk::Format::eB8G8R8Uscaled, identitySwizzle }; 
		case vk::Format::eB8G8R8Uscaled:		return { vk::Format::eR8G8B8Uscaled, identitySwizzle };
		case vk::Format::eR8G8B8Sscaled:		return { vk::Format::eB8G8R8Sscaled, identitySwizzle }; 
		case vk::Format::eB8G8R8Sscaled:		return { vk::Format::eR8G8B8Sscaled, identitySwizzle }; 
		case vk::Format::eR8G8B8Uint:			return { vk::Format::eB8G8R8Uint, identitySwizzle }; 
		case vk::Format::eB8G8R8Uint:			return { vk::Format::eR8G8B8Uint, identitySwizzle }; 
		case vk::Format::eR8G8B8Sint:			return { vk::Format::eB8G8R8Sint, identitySwizzle }; 
		case vk::Format::eB8G8R8Sint:			return { vk::Format::eR8G8B8Sint, identitySwizzle }; 
		case vk::Format::eR8G8B8Srgb:			return { vk::Format::eB8G8R8Srgb, identitySwizzle }; 
		case vk::Format::eB8G8R8Srgb:			return { vk::Format::eR8G8B8Srgb, identitySwizzle }; 

		case vk::Format::eR8G8B8A8Unorm: 		return { vk::Format::eB8G8R8A8Unorm, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Unorm: 		return { vk::Format::eR8G8B8A8Unorm, identitySwizzle };
		case vk::Format::eR8G8B8A8Snorm: 		return { vk::Format::eB8G8R8A8Snorm, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Snorm: 		return { vk::Format::eR8G8B8A8Snorm, identitySwizzle };
		case vk::Format::eR8G8B8A8Uscaled:		return { vk::Format::eB8G8R8A8Uscaled, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Uscaled:		return { vk::Format::eR8G8B8A8Uscaled, identitySwizzle };
		case vk::Format::eR8G8B8A8Sscaled:		return { vk::Format::eB8G8R8A8Sscaled, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Sscaled:		return { vk::Format::eR8G8B8A8Sscaled, identitySwizzle }; 
		case vk::Format::eR8G8B8A8Uint:			return { vk::Format::eB8G8R8A8Uint, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Uint:			return { vk::Format::eR8G8B8A8Uint, identitySwizzle }; 
		case vk::Format::eR8G8B8A8Sint:			return { vk::Format::eB8G8R8A8Sint, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Sint:			return { vk::Format::eR8G8B8A8Sint, identitySwizzle }; 
		case vk::Format::eR8G8B8A8Srgb:			return { vk::Format::eB8G8R8A8Srgb, identitySwizzle }; 
		case vk::Format::eB8G8R8A8Srgb:			return { vk::Format::eR8G8B8A8Srgb, identitySwizzle }; 

		case vk::Format::eA2R10G10B10UnormPack32:	return  { vk::Format::eA2B10G10R10UnormPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10UnormPack32:	return  { vk::Format::eA2R10G10B10UnormPack32, identitySwizzle };
		case vk::Format::eA2R10G10B10SnormPack32:	return  { vk::Format::eA2B10G10R10SnormPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10SnormPack32:	return  { vk::Format::eA2R10G10B10SnormPack32, identitySwizzle };
		case vk::Format::eA2R10G10B10UscaledPack32:	return  { vk::Format::eA2B10G10R10UscaledPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10UscaledPack32:	return  { vk::Format::eA2R10G10B10UscaledPack32, identitySwizzle };
		case vk::Format::eA2R10G10B10SscaledPack32:	return  { vk::Format::eA2B10G10R10SscaledPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10SscaledPack32:	return  { vk::Format::eA2R10G10B10SscaledPack32, identitySwizzle };
		case vk::Format::eA2R10G10B10UintPack32:	return  { vk::Format::eA2B10G10R10UintPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10UintPack32:	return  { vk::Format::eA2R10G10B10UintPack32, identitySwizzle };
		case vk::Format::eA2R10G10B10SintPack32:	return  { vk::Format::eA2B10G10R10SintPack32, identitySwizzle };
		case vk::Format::eA2B10G10R10SintPack32:	return  { vk::Format::eA2R10G10B10SintPack32, identitySwizzle };

		default: break;
		}
	}

	return fmt; //No conversion was found
}

inline std::tuple<vk::Format, vk::ComponentMapping> optimizeFormat(	const std::tuple<vk::Format, vk::ComponentMapping>& fmt,
																	Utils::BufferView<const vk::Format> supportedFormats ) noexcept
{
	//For binary search:
	assert(std::is_sorted(supportedFormats.cbegin(), supportedFormats.cend()));

	const auto optimized = optimizeFormat(fmt);
	if(optimized != fmt) {
		if(	std::binary_search(	supportedFormats.cbegin(), 
								supportedFormats.cend(), 
								std::get<vk::Format>(optimized) ))
		{
			return optimized;
		}
	}

	return fmt;
}



constexpr vk::Format toSrgb(vk::Format format) noexcept {
	switch(format){
	case vk::Format::eR8Unorm: 			return vk::Format::eR8Srgb;
	case vk::Format::eR8G8Unorm:		return vk::Format::eR8G8Srgb;
	case vk::Format::eR8G8B8Unorm:		return vk::Format::eR8G8B8Srgb;
	case vk::Format::eB8G8R8Unorm:		return vk::Format::eB8G8R8Srgb;
	case vk::Format::eR8G8B8A8Unorm:	return vk::Format::eR8G8B8A8Srgb;
	case vk::Format::eB8G8R8A8Unorm:	return vk::Format::eB8G8R8A8Srgb;
	default: return format;
	}
}

constexpr vk::Format fromSrgb(vk::Format format) noexcept {
	switch(format){
	case vk::Format::eR8Srgb: 			return vk::Format::eR8Unorm;
	case vk::Format::eR8G8Srgb:			return vk::Format::eR8G8Unorm;
	case vk::Format::eR8G8B8Srgb:		return vk::Format::eR8G8B8Unorm;
	case vk::Format::eB8G8R8Srgb:		return vk::Format::eB8G8R8Unorm;
	case vk::Format::eR8G8B8A8Srgb:		return vk::Format::eR8G8B8A8Unorm;
	case vk::Format::eB8G8R8A8Srgb:		return vk::Format::eB8G8R8A8Unorm;
	default: return format;
	}
}



constexpr bool hasDepth(vk::Format format) noexcept {
	switch(format) {
	case vk::Format::eD16Unorm:
	case vk::Format::eX8D24UnormPack32:
	case vk::Format::eD32Sfloat:
	case vk::Format::eD16UnormS8Uint:
	case vk::Format::eD24UnormS8Uint:
	case vk::Format::eD32SfloatS8Uint:
		return true;
	
	default:
		return false;
	}
}

constexpr bool hasStencil(vk::Format format) noexcept {
	switch(format) {
	case vk::Format::eS8Uint:
	case vk::Format::eD16UnormS8Uint:
	case vk::Format::eD24UnormS8Uint:
	case vk::Format::eD32SfloatS8Uint:
		return true;
	
	default:
		return false;
	}
}



constexpr size_t getPlaneCount(vk::Format format) noexcept {
	//Obtained from:
	//https://www.khronos.org/registry/vulkan/specs/1.1/html/chap34.html#formats-requiring-sampler-ycbcr-conversion
	switch(format) {
	case vk::Format::eUndefined: 
		return 0;

	//2 plane
	case vk::Format::eG8B8R82Plane420Unorm:
	case vk::Format::eG8B8R82Plane422Unorm:
	case vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16:
	case vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16:
	case vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16:
	case vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16:
	case vk::Format::eG16B16R162Plane420Unorm:
	case vk::Format::eG16B16R162Plane422Unorm:
		return 2;

	//3 plane
	case vk::Format::eG8B8R83Plane420Unorm:
	case vk::Format::eG8B8R83Plane422Unorm:
	case vk::Format::eG8B8R83Plane444Unorm:
	case vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16:
	case vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16:
	case vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16:
	case vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16:
	case vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16:
	case vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16:
	case vk::Format::eG16B16R163Plane420Unorm:
	case vk::Format::eG16B16R163Plane422Unorm:
	case vk::Format::eG16B16R163Plane444Unorm:
		return 3;

	default:
		return 1;
	}
}

constexpr bool requiresYCbCrSamplerConversion(vk::Format format) noexcept {
	//Obtained from:
	//https://www.khronos.org/registry/vulkan/specs/1.1/html/chap34.html#formats-requiring-sampler-ycbcr-conversion
	switch (format) {
	//8bit
	case vk::Format::eG8B8G8R8422Unorm:
	case vk::Format::eB8G8R8G8422Unorm:
	case vk::Format::eG8B8R83Plane420Unorm:
	case vk::Format::eG8B8R82Plane420Unorm:
	case vk::Format::eG8B8R83Plane422Unorm:
	case vk::Format::eG8B8R82Plane422Unorm:
	case vk::Format::eG8B8R83Plane444Unorm:

	//10bit
	case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16:
	case vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16:
	case vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16:
	case vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16:
	case vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16:
	case vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16:
	case vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16:
	case vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16:

	//12bit
	case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16:
	case vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16:
	case vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16:
	case vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16:
	case vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16:
	case vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16:
	case vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16:
	case vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16:

	//16bit
	case vk::Format::eG16B16G16R16422Unorm:
	case vk::Format::eB16G16R16G16422Unorm:
	case vk::Format::eG16B16R163Plane420Unorm:
	case vk::Format::eG16B16R162Plane420Unorm:
	case vk::Format::eG16B16R163Plane422Unorm:
	case vk::Format::eG16B16R162Plane422Unorm:
	case vk::Format::eG16B16R163Plane444Unorm:
		return true;

	default:
		return false;
	}
}



constexpr vk::ImageAspectFlagBits getPlaneAspectBit(size_t plane) noexcept {
	switch(plane) {
	case 0: 	return vk::ImageAspectFlagBits::ePlane0;
	case 1: 	return vk::ImageAspectFlagBits::ePlane1;
	case 2: 	return vk::ImageAspectFlagBits::ePlane2;
	default: 	return static_cast<vk::ImageAspectFlagBits>(0);
	}
}



constexpr vk::ComponentSwizzle getComponentDefaultValue(char component) noexcept {
	switch (component) {
	case 'a': case 'A': return vk::ComponentSwizzle::eOne;
	default: return vk::ComponentSwizzle::eZero;
	}
}

constexpr vk::ComponentSwizzle getComponentSwizzle(char swizzle) noexcept {
	switch (swizzle) {
	case 'r': case 'R': return vk::ComponentSwizzle::eR;
	case 'g': case 'G': return vk::ComponentSwizzle::eG;
	case 'b': case 'B': return vk::ComponentSwizzle::eB;
	case 'a': case 'A': return vk::ComponentSwizzle::eA;
	case '0': return vk::ComponentSwizzle::eOne;
	case '1': return vk::ComponentSwizzle::eZero;
	default: return vk::ComponentSwizzle::eIdentity;
	}
}

constexpr vk::ComponentSwizzle getComponentSwizzle(char component, std::string_view vulkanFromat, std::string_view memoryFormat) noexcept {
	//Find its position
	size_t i = 0;
	while(i < memoryFormat.size() && memoryFormat[i] != component) ++i;

	if(i >= memoryFormat.size()) return getComponentDefaultValue(component); //Not defined
	if(memoryFormat[i] == vulkanFromat[i]) return vk::ComponentSwizzle::eIdentity; //Not modified
	return getComponentSwizzle(vulkanFromat[i]); //Neeeds swizzle
}

constexpr vk::ComponentMapping swizzle(std::string_view swizzle) noexcept {
	const auto n = swizzle.size();

	return vk::ComponentMapping(
		(n > 0) ? getComponentSwizzle(swizzle[0]) : vk::ComponentSwizzle::eIdentity,
		(n > 1) ? getComponentSwizzle(swizzle[1]) : vk::ComponentSwizzle::eIdentity,
		(n > 2) ? getComponentSwizzle(swizzle[2]) : vk::ComponentSwizzle::eIdentity,
		(n > 3) ? getComponentSwizzle(swizzle[3]) : vk::ComponentSwizzle::eIdentity
	);
}

constexpr vk::ComponentMapping swizzle(std::string_view vulkanFromat, std::string_view memoryFormat) noexcept {
	return vk::ComponentMapping(
		getComponentSwizzle('R', vulkanFromat, memoryFormat),
		getComponentSwizzle('G', vulkanFromat, memoryFormat),
		getComponentSwizzle('B', vulkanFromat, memoryFormat),
		getComponentSwizzle('A', vulkanFromat, memoryFormat)
	);
}



constexpr vk::ColorSpaceKHR toVulkan(ColorPrimaries prim, ColorTransferFunction enc) noexcept {
	switch(enc){
	case ColorTransferFunction::LINEAR:
		//Linear encoding
		switch(prim){
		case ColorPrimaries::BT709: 		return vk::ColorSpaceKHR::eBt709LinearEXT;
		case ColorPrimaries::BT2020:		return vk::ColorSpaceKHR::eBt2020LinearEXT;
		case ColorPrimaries::SMPTE432:		return vk::ColorSpaceKHR::eDisplayP3LinearEXT;
		case ColorPrimaries::ADOBE_RGB:		return vk::ColorSpaceKHR::eAdobergbLinearEXT;
		default: break;
		}
		break;
	case ColorTransferFunction::BT709:
		//IEC61966_2_1 non linear encoding
		switch(prim){
		case ColorPrimaries::BT709: 		return vk::ColorSpaceKHR::eBt709NonlinearEXT;
		default: break;
		}
		break;
	case ColorTransferFunction::IEC61966_2_1:
		//IEC61966_2_1 non linear encoding
		switch(prim){
		case ColorPrimaries::IEC61966_2_1: 	return vk::ColorSpaceKHR::eSrgbNonlinear;
		case ColorPrimaries::SMPTE432:		return vk::ColorSpaceKHR::eDisplayP3NonlinearEXT;
		default: break;
		}
		break;
	case ColorTransferFunction::SMPTE2084:
		//IEC61966_2_1 non linear encoding
		switch(prim){
		case ColorPrimaries::BT2020: 		return vk::ColorSpaceKHR::eHdr10St2084EXT;
		default: break;
		}
		break;
	case ColorTransferFunction::ARIB_STD_B67:
		//IEC61966_2_1 non linear encoding
		switch(prim){
		case ColorPrimaries::BT2020: 		return vk::ColorSpaceKHR::eHdr10HlgEXT;
		default: break;
		}
		break;

	default: break;
	}

	return static_cast<vk::ColorSpaceKHR>(-1);
}

constexpr std::tuple<ColorPrimaries, ColorTransferFunction> fromVulkan(vk::ColorSpaceKHR space) noexcept {
	switch(space){

	case vk::ColorSpaceKHR::eSrgbNonlinear:				return { ColorPrimaries::IEC61966_2_1, 	ColorTransferFunction::IEC61966_2_1 };
	case vk::ColorSpaceKHR::eDisplayP3NonlinearEXT:		return { ColorPrimaries::SMPTE432, 		ColorTransferFunction::IEC61966_2_1 };
	//case vk::ColorSpaceKHR::eExtendedSrgbLinearEXT:	return {}; /*NOT SUPPORTED*/
	case vk::ColorSpaceKHR::eDisplayP3LinearEXT:		return { ColorPrimaries::SMPTE432, 		ColorTransferFunction::LINEAR };
	//case vk::ColorSpaceKHR::eDciP3NonlinearEXT:		return {}; /*NOT SUPPORTED: XYZ model expected*/
	case vk::ColorSpaceKHR::eBt709LinearEXT:			return { ColorPrimaries::BT709, 		ColorTransferFunction::LINEAR };
	case vk::ColorSpaceKHR::eBt709NonlinearEXT:			return { ColorPrimaries::BT709, 		ColorTransferFunction::BT709 };
	case vk::ColorSpaceKHR::eBt2020LinearEXT:			return { ColorPrimaries::BT2020, 		ColorTransferFunction::LINEAR };
	case vk::ColorSpaceKHR::eHdr10St2084EXT:			return { ColorPrimaries::BT2020, 		ColorTransferFunction::SMPTE2084 };
	//case vk::ColorSpaceKHR::eDolbyvisionEXT:			return {}; /*NOT SUPPORTED*/
	case vk::ColorSpaceKHR::eHdr10HlgEXT:				return { ColorPrimaries::BT2020, 		ColorTransferFunction::ARIB_STD_B67 };
	case vk::ColorSpaceKHR::eAdobergbLinearEXT:			return { ColorPrimaries::ADOBE_RGB, 	ColorTransferFunction::LINEAR };
	case vk::ColorSpaceKHR::eAdobergbNonlinearEXT:		return { ColorPrimaries::ADOBE_RGB, 	ColorTransferFunction::GAMMA22 };
	//case vk::ColorSpaceKHR::ePassThroughEXT:			return {}; /*NOT SUPPORTED*/
	//case vk::ColorSpaceKHR::eExtendedSrgbNonlinearEXT:return {}; /*NOT SUPPORTED*/
	//case vk::ColorSpaceKHR::eDisplayNativeAMD:		return {}; /*NOT SUPPORTED*/
	//case vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear:return { ColorPrimaries::BT709, 	ColorTransferFunction::IEC61966_2_1 };	
	//case vk::ColorSpaceKHR::eDciP3LinearEXT:			return {}; /*NOT SUPPORTED: wrongly named as DisplayP3*/
	default: 											return {};
	}
}



constexpr vk::Filter toVulkan(ScalingFilter filt) noexcept {
	switch(filt){
	case ScalingFilter::NEAREST: return vk::Filter::eNearest;
	case ScalingFilter::LINEAR: return vk::Filter::eLinear;
	case ScalingFilter::CUBIC: return vk::Filter::eCubicEXT;
	default: return static_cast<vk::Filter>(-1);
	}
}

constexpr ScalingFilter fromVulkan(vk::Filter filt) noexcept {
	switch(filt){
	case vk::Filter::eNearest: return ScalingFilter::NEAREST;
	case vk::Filter::eLinear: return ScalingFilter::LINEAR;
	case vk::Filter::eCubicEXT: return ScalingFilter::CUBIC;
	default: return ScalingFilter::NONE;
	}
}



constexpr vk::PipelineStageFlags toPipelineStageFlags(vk::ShaderStageFlags flags) noexcept {
	vk::PipelineStageFlags result;

	if(flags & vk::ShaderStageFlagBits::eVertex)					result |= vk::PipelineStageFlagBits::eVertexShader;
	if(flags & vk::ShaderStageFlagBits::eTessellationControl)		result |= vk::PipelineStageFlagBits::eTessellationControlShader;
	if(flags & vk::ShaderStageFlagBits::eTessellationEvaluation)	result |= vk::PipelineStageFlagBits::eTessellationEvaluationShader;
	if(flags & vk::ShaderStageFlagBits::eGeometry)					result |= vk::PipelineStageFlagBits::eGeometryShader;
	if(flags & vk::ShaderStageFlagBits::eFragment)					result |= vk::PipelineStageFlagBits::eFragmentShader;
	if(flags & vk::ShaderStageFlagBits::eCompute)					result |= vk::PipelineStageFlagBits::eComputeShader;

	return result;
}

constexpr vk::ShaderStageFlags toShaderStageFlags(vk::PipelineStageFlags flags) noexcept {
	vk::ShaderStageFlags result;

	if(flags & vk::PipelineStageFlagBits::eVertexShader)					result |= vk::ShaderStageFlagBits::eVertex;
	if(flags & vk::PipelineStageFlagBits::eTessellationControlShader)		result |= vk::ShaderStageFlagBits::eTessellationControl;
	if(flags & vk::PipelineStageFlagBits::eTessellationEvaluationShader)	result |= vk::ShaderStageFlagBits::eTessellationEvaluation;
	if(flags & vk::PipelineStageFlagBits::eGeometryShader)					result |= vk::ShaderStageFlagBits::eGeometry;
	if(flags & vk::PipelineStageFlagBits::eFragmentShader)					result |= vk::ShaderStageFlagBits::eFragment;
	if(flags & vk::PipelineStageFlagBits::eComputeShader)					result |= vk::ShaderStageFlagBits::eCompute;

	return result;
}



constexpr vk::PipelineColorBlendAttachmentState getBlendingConfiguration(BlendingMode mode) noexcept {
	constexpr vk::ColorComponentFlags colorWriteMask = 
		vk::ColorComponentFlagBits::eR |
		vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eB |
		vk::ColorComponentFlagBits::eA ;

	//This blending modes have been configured considering that src color is alpha pre-multiplied


	switch(mode) {
	case BlendingMode::WRITE:
		return vk::PipelineColorBlendAttachmentState(
			false, //Don't blend
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			colorWriteMask
		);
	case BlendingMode::OPACITY:
		return vk::PipelineColorBlendAttachmentState(
			true,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOneMinusSrcAlpha,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOneMinusSrcAlpha,
			vk::BlendOp::eAdd,
			colorWriteMask
		);
	case BlendingMode::ADD:
		return vk::PipelineColorBlendAttachmentState(
			true,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
			vk::BlendOp::eAdd,
			colorWriteMask
		);
	case BlendingMode::DIFFERENCE:
		return vk::PipelineColorBlendAttachmentState(
			true,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
			vk::BlendOp::eSubtract,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
			vk::BlendOp::eSubtract,
			colorWriteMask
		);
	case BlendingMode::DIFFERENCE_1:
		return vk::PipelineColorBlendAttachmentState(
			true,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
			vk::BlendOp::eReverseSubtract,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
			vk::BlendOp::eReverseSubtract,
			colorWriteMask
		);
	case BlendingMode::DARKEN:
		return vk::PipelineColorBlendAttachmentState(
			true,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
			vk::BlendOp::eMin,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
			vk::BlendOp::eMin,
			colorWriteMask
		);
	case BlendingMode::LIGHTEN:
		return vk::PipelineColorBlendAttachmentState(
			true,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
			vk::BlendOp::eMax,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
			vk::BlendOp::eMax,
			colorWriteMask
		);
	case BlendingMode::MULTIPLY:
		return vk::PipelineColorBlendAttachmentState(
			true,
			vk::BlendFactor::eDstColor,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eDstColor,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			colorWriteMask
		);
	case BlendingMode::SCREEN:
		return vk::PipelineColorBlendAttachmentState(
			true,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOneMinusSrcColor,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOneMinusSrcColor,
			vk::BlendOp::eAdd,
			colorWriteMask
		);
	default:
		return vk::PipelineColorBlendAttachmentState();
	}
}

constexpr vk::PipelineDepthStencilStateCreateInfo getDepthStencilConfiguration(RenderingLayer layer) noexcept {
	const auto enableDepthTesting = (layer == RenderingLayer::SCENE);
	return vk::PipelineDepthStencilStateCreateInfo(
		{},													//Flags
		enableDepthTesting, enableDepthTesting, 			//Depth test enable, write
		vk::CompareOp::eLess,								//Depth compare op
		false,												//Depth bounds test
		false, 												//Stencil enabled
		{}, {},												//Stencil operation state front, back
		0.0f, 0.0f											//min, max depth bounds
	);
}



template<typename T>
constexpr vk::ArrayProxy<T> toVulkan(Utils::BufferView<T> bv) noexcept {
	return vk::ArrayProxy<T>(bv.size(), bv.data());
}

template<typename T>
constexpr Utils::BufferView<T> fromVulkan(vk::ArrayProxy<T> ap) noexcept {
	return Utils::BufferView<T>(ap.data(), ap.size());
}

}

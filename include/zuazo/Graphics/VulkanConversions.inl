#include "VulkanConversions.h"

#include "../Macros.h"

#include <utility>
#include <cassert>
#include <algorithm>

namespace Zuazo::Graphics {

constexpr uint32_t toVulkan(Version version){
	return VK_MAKE_VERSION(
		version.getMajor(),
		version.getMinor(),
		version.getPatch()
	);
}

constexpr vk::DebugUtilsMessageSeverityFlagsEXT toVulkan(Verbosity ver) {
	vk::DebugUtilsMessageSeverityFlagsEXT result = {};

	if((ver & Verbosity::ERROR) != Verbosity::SILENT) 	result |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	if((ver & Verbosity::WARNING) != Verbosity::SILENT)	result |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
	if((ver & Verbosity::INFO) != Verbosity::SILENT) 	result |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
	if((ver & Verbosity::VERBOSE) != Verbosity::SILENT)	result |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;

	return result;
}

constexpr Verbosity fromVulkan(vk::DebugUtilsMessageSeverityFlagsEXT sev) {
	constexpr auto silent = vk::DebugUtilsMessageSeverityFlagsEXT();
	Verbosity result = Verbosity::SILENT;

	if((sev & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) != silent)		result |= Verbosity::ERROR;
	if((sev & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) != silent)	result |= Verbosity::WARNING;
	if((sev & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) != silent)		result |= Verbosity::INFO;
	if((sev & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) != silent)	result |= Verbosity::VERBOSE;

	return result;
}	


constexpr vk::Extent2D toVulkan(Resolution res){
	return vk::Extent2D(
		res.width,
		res.height
	);
}

constexpr Resolution fromVulkan(vk::Extent2D res){
	return Resolution(
		res.width,
		res.height
	);
}

constexpr std::array<std::tuple<vk::Format, vk::ComponentMapping>, MAX_PLANE_COUNT> toVulkan(ColorFormat fmt){
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

	#if ZUAZO_IS_BIG_ENDIAN
	case ColorFormat::X8R8G8B8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "XRGB") } };
	case ColorFormat::X8B8G8R8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "XBGR") } };
	case ColorFormat::R8G8B8X8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "RGBX") } };
	case ColorFormat::B8G8R8X8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "BGRX") } };

	case ColorFormat::A8R8G8B8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "ARGB") } };
	case ColorFormat::A8B8G8R8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "ABGR") } };
	case ColorFormat::R8G8B8A8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::B8G8R8A8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "BGRA") } };
	#elif ZUAZO_IS_LITTLE_ENDIAN
	case ColorFormat::X8R8G8B8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "BGRX") } };
	case ColorFormat::X8B8G8R8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "RGBX") } };
	case ColorFormat::R8G8B8X8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "XBGR") } };
	case ColorFormat::B8G8R8X8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "XRGB") } };

	case ColorFormat::A8R8G8B8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "BGRA") } };
	case ColorFormat::A8B8G8R8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::R8G8B8A8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "ABGR") } };
	case ColorFormat::B8G8R8A8_32:		return { std::tuple{ vk::Format::eR8G8B8A8Unorm, swizzle("RGBA", "ARGB") } };
	#endif

	case ColorFormat::R8G8B8G8:			return { std::tuple{ vk::Format::eB8G8R8G8422Unorm, swizzle("RGBA", "BGRA") } };
	case ColorFormat::B8G8R8G8:			return { std::tuple{ vk::Format::eB8G8R8G8422Unorm, swizzle("RGBA", "RGBA")  } };
	case ColorFormat::G8R8G8B8:			return { std::tuple{ vk::Format::eG8B8G8R8422Unorm, swizzle("RGBA", "BGRA") } };
	case ColorFormat::G8B8G8R8:			return { std::tuple{ vk::Format::eG8B8G8R8422Unorm, swizzle("RGBA", "RGBA")  } };


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

	default: return {};
	}
}

constexpr std::tuple<ColorFormat, ColorTransferFunction> fromVulkan(vk::Format fmt){
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

inline std::vector<ColorFormat> getFramebufferFormats(Utils::BufferView<const vk::Format> supported) {
	std::vector<ColorFormat> result;

	//To use binary search:
	assert(std::is_sorted(supported.cbegin(), supported.cend()));

	for(ColorFormat format = ColorFormat::NONE; format < ColorFormat::COUNT; format++) {
		if(getPlaneCount(format) != 1) continue;

		auto planes = toVulkan(format);
		assert(std::get<vk::Format>(planes[1]) == vk::Format::eUndefined);

		auto& plane = planes[0];
		plane = optimizeFormat(plane);

		if(std::get<vk::ComponentMapping>(plane) != vk::ComponentMapping()) continue;
		if(!std::binary_search(supported.cbegin(), supported.cend(), std::get<vk::Format>(plane))) continue;

		result.push_back(format);
	}

	return result;
}

inline std::vector<ColorFormat> getSamplerFormats(Utils::BufferView<const vk::Format> supported) {
	std::vector<ColorFormat> result;

	//To use binary search:
	assert(std::is_sorted(supported.cbegin(), supported.cend()));

	for(ColorFormat format = ColorFormat::NONE; format < ColorFormat::COUNT; format++) {
		const auto planes = toVulkan(format);
		if(std::get<vk::Format>(planes[0]) == vk::Format::eUndefined) continue;

		bool support = true;
		for(const auto& plane : planes) {
			if(std::get<vk::Format>(plane) != vk::Format::eUndefined) {
				if(!std::binary_search(supported.cbegin(), supported.cend(), std::get<vk::Format>(plane))){
					support = false;
					break;
				}
			}
		}
		if(!support) continue;

		result.push_back(format);
	}

	return result;
}


constexpr std::tuple<vk::Format, vk::ComponentMapping> optimizeFormat(const std::tuple<vk::Format, vk::ComponentMapping>& fmt){
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
																	Utils::BufferView<const vk::Format> supportedFormats )
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



constexpr vk::ColorSpaceKHR toVulkan(ColorPrimaries prim, ColorTransferFunction enc){
	constexpr vk::ColorSpaceKHR ERROR = static_cast<vk::ColorSpaceKHR>(-1);

	switch(enc){
	case ColorTransferFunction::LINEAR:
		//Linear encoding
		switch(prim){
		case ColorPrimaries::BT709: 	return vk::ColorSpaceKHR::eBt709LinearEXT;
		case ColorPrimaries::BT2020:	return vk::ColorSpaceKHR::eBt2020LinearEXT;
		case ColorPrimaries::SMPTE432:	return vk::ColorSpaceKHR::eDisplayP3LinearEXT;
		case ColorPrimaries::ADOBE_RGB:	return vk::ColorSpaceKHR::eAdobergbLinearEXT;
		default: return ERROR;
		}
	case ColorTransferFunction::IEC61966_2_1:
		//IEC61966_2_1 non linear encoding
		switch(prim){
		case ColorPrimaries::BT709: 	return vk::ColorSpaceKHR::eSrgbNonlinear;
		case ColorPrimaries::SMPTE432:	return vk::ColorSpaceKHR::eDisplayP3NonlinearEXT;
		default: return ERROR;
		}
	default: return ERROR;
	}
}

constexpr std::tuple<ColorPrimaries, ColorTransferFunction> fromVulkan(vk::ColorSpaceKHR space){
	switch(space){
	case vk::ColorSpaceKHR::eBt709LinearEXT: return { ColorPrimaries::BT709, ColorTransferFunction::LINEAR };
	case vk::ColorSpaceKHR::eBt2020LinearEXT: return { ColorPrimaries::BT2020, ColorTransferFunction::LINEAR};
	case vk::ColorSpaceKHR::eDisplayP3LinearEXT: return { ColorPrimaries::SMPTE432, ColorTransferFunction::LINEAR };
	case vk::ColorSpaceKHR::eAdobergbLinearEXT: return { ColorPrimaries::ADOBE_RGB, ColorTransferFunction::LINEAR };
	case vk::ColorSpaceKHR::eSrgbNonlinear: return { ColorPrimaries::BT709, ColorTransferFunction::IEC61966_2_1 };
	case vk::ColorSpaceKHR::eDisplayP3NonlinearEXT: return { ColorPrimaries::SMPTE432, ColorTransferFunction::IEC61966_2_1 };
	default: return {};
	}
}


constexpr vk::ComponentSwizzle getComponentDefaultValue(char component) {
	switch (component) {
	case 'a': case 'A': return vk::ComponentSwizzle::eOne;
	default: return vk::ComponentSwizzle::eZero;
	}
}

constexpr vk::ComponentSwizzle getComponentSwizzle(char swizzle) {
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

constexpr vk::ComponentSwizzle getComponentSwizzle(char component, std::string_view vulkanFromat, std::string_view memoryFormat) {
	//Find its position
	size_t i = 0;
	while(i < memoryFormat.size() && memoryFormat[i] != component) ++i;

	if(i >= memoryFormat.size()) return getComponentDefaultValue(component); //Not defined
	if(memoryFormat[i] == vulkanFromat[i]) return vk::ComponentSwizzle::eIdentity; //Not modified
	return getComponentSwizzle(vulkanFromat[i]); //Neeeds swizzle
}

constexpr vk::ComponentMapping swizzle(std::string_view swizzle) {
	const auto n = swizzle.size();

	return vk::ComponentMapping(
		(n > 0) ? getComponentSwizzle(swizzle[0]) : vk::ComponentSwizzle::eIdentity,
		(n > 1) ? getComponentSwizzle(swizzle[1]) : vk::ComponentSwizzle::eIdentity,
		(n > 2) ? getComponentSwizzle(swizzle[2]) : vk::ComponentSwizzle::eIdentity,
		(n > 3) ? getComponentSwizzle(swizzle[3]) : vk::ComponentSwizzle::eIdentity
	);
}

constexpr vk::ComponentMapping swizzle(std::string_view vulkanFromat, std::string_view memoryFormat) {
	return vk::ComponentMapping(
		getComponentSwizzle('R', vulkanFromat, memoryFormat),
		getComponentSwizzle('G', vulkanFromat, memoryFormat),
		getComponentSwizzle('B', vulkanFromat, memoryFormat),
		getComponentSwizzle('A', vulkanFromat, memoryFormat)
	);
}




constexpr vk::Format toSrgb(vk::Format format) {
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

constexpr vk::Format fromSrgb(vk::Format format) {
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


constexpr vk::Filter toVulkan(ScalingFilter filt) {
	switch(filt){
	case ScalingFilter::NEAREST: return vk::Filter::eNearest;
	case ScalingFilter::LINEAR: return vk::Filter::eLinear;
	default: return static_cast<vk::Filter>(-1);
	}
}

constexpr ScalingFilter fromVulkan(vk::Filter filt) {
	switch(filt){
	case vk::Filter::eNearest: return ScalingFilter::NEAREST;
	case vk::Filter::eLinear: return ScalingFilter::LINEAR;
	default: return ScalingFilter::NONE;
	}
}


template<typename T>
constexpr vk::ArrayProxy<T> toVulkan(Utils::BufferView<T> bv) {
	return vk::ArrayProxy<T>(bv.size(), bv.data());
}

template<typename T>
constexpr Utils::BufferView<T> fromVulkan(vk::ArrayProxy<T> ap) {
	return Utils::BufferView<T>(ap.data(), ap.size());
}

}

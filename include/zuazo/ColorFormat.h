#pragma once

#include "Macros.h"

#include <type_traits>
#include <string_view>
#include <ostream>

namespace Zuazo {

/**
 * ColorFormat defines how pixel data is layed out in memory. 
 * Please, note that although pixel formats are defined for RGBA 
 * color spaces (or monochrome), they are also apply for YCbCr color spaces.
 * 
 * The following notation is used:
 * * Compoent name (R, G, B, A, Y or X) followed by its bit allocation
 * * Component name meanings:
 *   - R = Red (or Cr for YCbCr color models)
 *   - G = Green (or Y for YCbCr color models)
 *   - B = Blue (or Cb for YCbCr color models)
 *   - A = Alpha (transparency)
 *   - Y = Intensity for monochrome images (grayscale images)
 *   - X = Represents unused space. It's content will be ignored
 * * Components separated by an underscore "_" means the begining of a new plane.
 * * A _## in the end (for instance _32) represents that it is packed in ## bit CPU word(s).
 * 		This means that it will be endianess dependant. For example A8R8G8B8_32 will be stored as 
 * 		a 32 bit interger. Therefore, the actual memory layout in a little endian CPU will be 
 * 		B8G8R8A8, whilst on a big endian CPU it will be layed out as A8R8G8B8.
 */
enum class ColorFormat {
	none = -1,

	//4 BIT COMPONENTS
	Y4X4_8,
	X4Y4_8,

	Y4A4_8,
	A4Y4_8,

	X4R4G4B4_16,
	X4B4G4R4_16,
	X4G4R4B4_16,
	X4G4B4R4_16,
	R4G4B4X4_16,
	B4G4R4X4_16,
	G4R4B4X4_16,
	G4B4R4X4_16,

	A4R4G4B4_16,
	A4B4G4R4_16,
	A4G4R4B4_16,
	A4G4B4R4_16,
	R4G4B4A4_16,
	B4G4R4A4_16,
	G4R4B4A4_16,
	G4B4R4A4_16,


	//5 BIT COMPONENTS
	X1R5G5B5_16,
	X1B5G5R5_16,
	X1G5R5B5_16,
	X1G5B5R5_16,
	R5G5B5X1_16,
	B5G5R5X1_16,
	G5R5B5X1_16,
	G5B5R5X1_16,

	A1R5G5B5_16,
	A1B5G5R5_16,
	A1G5R5B5_16,
	A1G5B5R5_16,
	R5G5B5A1_16,
	B5G5R5A1_16,
	G5R5B5A1_16,
	G5B5R5A1_16,


	//6 BIT COMPONENTS
	R5G6B5_16,
	B5G6R5_16,


	//8 BIT COMPONENTS
	Y8,

	Y8A8,
	A8Y8,

	R8G8B8,
	B8G8R8,
	G8R8B8,
	G8B8R8,

	A8R8G8B8,
	A8B8G8R8,
	A8G8R8B8,
	A8G8B8R8,
	R8G8B8A8,
	B8G8R8A8,
	G8R8B8A8,
	G8B8R8A8,

	A8R8G8B8_32,
	A8B8G8R8_32,
	R8G8B8A8_32,
	B8G8R8A8_32,

	X8R8G8B8_32,
	X8B8G8R8_32,
	R8G8B8X8_32,
	B8G8R8X8_32,
	
	R8G8B8G8, ///<\note This format must have 4:2:2 color subsampling
	B8G8R8G8, ///<\note This format must have 4:2:2 color subsampling
	G8R8G8B8, ///<\note This format must have 4:2:2 color subsampling
	G8B8G8R8, ///<\note This format must have 4:2:2 color subsampling

	G8_B8_R8_A8,
	G8_B8_R8,
	G8_B8R8_A8,
	G8_R8B8_A8,
	G8_B8R8,
	G8_R8B8,


	//10 BIT COMPONENTS
	Y10X6_16,

	Y10X6A10X6_16,
	A10X6Y10X6_16,

	A10X6R10X6G10X6B10X6_16,
	A10X6B10X6G10X6R10X6_16,
	A10X6G10X6R10X6B10X6_16,
	A10X6G10X6B10X6R10X6_16,
	R10X6G10X6B10X6A10X6_16,
	B10X6G10X6R10X6A10X6_16,
	G10X6R10X6B10X6A10X6_16,
	G10X6B10X6R10X6A10X6_16,

	X2R10G10B10_32, 
	X2B10G10R10_32,
	X2G10R10B10_32, 
	X2G10B10R10_32,
	
	A2R10G10B10_32, 
	A2B10G10R10_32,
	A2G10R10B10_32, 
	A2G10B10R10_32,

	R10X6G10X6B10X6G10X6_16, ///<\note This format must have 4:2:2 color subsampling
	B10X6G10X6R10X6G10X6_16, ///<\note This format must have 4:2:2 color subsampling
	G10X6R10X6G10X6B10X6_16, ///<\note This format must have 4:2:2 color subsampling
	G10X6B10X6G10X6R10X6_16, ///<\note This format must have 4:2:2 color subsampling

	G10X6_B10X6_R10X6_A10X6_16,
	G10X6_B10X6_R10X6_16,
	G10X6_B10X6R10X6_A10X6_16,
	G10X6_R10X6B10X6_A10X6_16,
	G10X6_B10X6R10X6_16,
	G10X6_R10X6B10X6_16,


	//12 BIT COMPONENTS
	Y12X4_16,

	Y12X4A12X4_16,
	A12X4Y12X4_16,

	A12X4R12X4G12X4B12X4_16,
	A12X4B12X4G12X4R12X4_16,
	A12X4G12X4R12X4B12X4_16,
	A12X4G12X4B12X4R12X4_16,
	R12X4G12X4B12X4A12X4_16,
	B12X4G12X4R12X4A12X4_16,
	G12X4R12X4B12X4A12X4_16,
	G12X4B12X4R12X4A12X4_16,

	R12X4G12X4B12X4G12X4_16, ///<\note This format must have 4:2:2 color subsampling
	B12X4G12X4R12X4G12X4_16, ///<\note This format must have 4:2:2 color subsampling
	G12X4R12X4G12X4B12X4_16, ///<\note This format must have 4:2:2 color subsampling
	G12X4B12X4G12X4R12X4_16, ///<\note This format must have 4:2:2 color subsampling

	G12X4_B12X4_R12X4_A12X4_16,
	G12X4_B12X4_R12X4_16,
	G12X4_B12X4R12X4_A12X4_16,
	G12X4_R12X4B12X4_A12X4_16,
	G12X4_B12X4R12X4_16,
	G12X4_R12X4B12X4_16,


	//16 BIT COMPONENTS
	Y16,

	Y16f,

	Y16A16,
	A16Y16,

	Y16fA16f,
	A16fY16f,

	R16G16B16,
	B16G16R16,
	G16R16B16,
	G16B16R16,

	R16fG16fB16f,
	B16fG16fR16f,
	G16fR16fB16f,
	G16fB16fR16f,

	A16R16G16B16,
	A16B16G16R16,
	A16G16R16B16,
	A16G16B16R16,
	R16G16B16A16,
	B16G16R16A16,
	G16R16B16A16,
	G16B16R16A16,

	A16fR16fG16fB16f,
	A16fB16fG16fR16f,
	A16fG16fR16fB16f,
	A16fG16fB16fR16f,
	R16fG16fB16fA16f,
	B16fG16fR16fA16f,
	G16fR16fB16fA16f,
	G16fB16fR16fA16f,

	R16G16B16G16, 	///<\note This format must have 4:2:2 color subsampling
	B16G16R16G16, 	///<\note This format must have 4:2:2 color subsampling
	G16R16G16B16, 	///<\note This format must have 4:2:2 color subsampling
	G16B16G16R16, 	///<\note This format must have 4:2:2 color subsampling

	G16_B16_R16_A16,
	G16_B16_R16,
	G16_B16R16_A16,
	G16_R16B16_A16,
	G16_B16R16,
	G16_R16B16,

	G16f_B16f_R16f_A16f,
	G16f_B16f_R16f,

	//32 BIT COMPONENTS
	Y32f,

	Y32fA32f,
	A32fY32f,

	R32fG32fB32f,
	B32fG32fR32f,
	G32fR32fB32f,
	G32fB32fR32f,

	A32fR32fG32fB32f,
	A32fB32fG32fR32f,
	A32fG32fR32fB32f,
	A32fG32fB32fR32f,
	R32fG32fB32fA32f,
	B32fG32fR32fA32f,
	G32fR32fB32fA32f,
	G32fB32fR32fA32f,

	G32f_B32f_R32f_A32f,
	G32f_B32f_R32f,


	//64 BIT COMPONENTS
	Y64f,

	Y64fA64f,
	A64fY64f,

	R64fG64fB64f,
	B64fG64fR64f,
	G64fR64fB64f,
	G64fB64fR64f,

	A64fR64fG64fB64f,
	A64fB64fG64fR64f,
	A64fG64fR64fB64f,
	A64fG64fB64fR64f,
	R64fG64fB64fA64f,
	B64fG64fR64fA64f,
	G64fR64fB64fA64f,
	G64fB64fR64fA64f,

	G64f_B64f_R64f_A64f,
	G64f_B64f_R64f,


	//Add here

	count
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorFormat)
ZUAZO_ENUM_COMP_OPERATORS(ColorFormat)	

constexpr size_t getPlaneCount(ColorFormat format) noexcept;
constexpr bool hasAlpha(ColorFormat format) noexcept;

constexpr std::string_view toString(ColorFormat format) noexcept;
size_t fromString(std::string_view str, ColorFormat& format);
std::ostream& operator<<(std::ostream& os, ColorFormat format);

namespace Utils {

template<typename T>
struct EnumTraits;

template<>
struct EnumTraits<ColorFormat> {
	static constexpr ColorFormat first() noexcept;
	static constexpr ColorFormat last() noexcept;
};

}

}

#include "ColorFormat.inl"

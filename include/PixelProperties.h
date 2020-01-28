#pragma once

#include "Macros.h"

namespace Zuazo {

/**
 * PixelFormat defines how pixel data is layed out in memory. 
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
 * * A _## at the end (for istance _32) represents that it is packed in ## bit CPU word(s).
 * 		This means that it will be endianess dependant
 * * A _S422 at the end means that it will have a 4:2:2 chroma subsampling scheme. 
 * * A _S420 at the end means that it will have a 4:2:0 chroma subsampling scheme. 
 */
enum class PixelFormat {
	NONE = 0,

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
	
	R8G8B8G8_S422,
	B8G8R8G8_S422,
	G8R8G8B8_S422,
	G8B8G8R8_S422,

	G8_B8_R8,
	G8_B8_R8_S422,
	G8_B8_R8_S420,

	G8_R8B8_S422,
	G8_B8R8_S422,
	G8_R8B8_S420,
	G8_B8R8_S420,


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

	R10X6G10X6B10X6G10X6_S422_16,
	B10X6G10X6R10X6G10X6_S422_16,
	G10X6R10X6G10X6B10X6_S422_16,
	G10X6B10X6G10X6R10X6_S422_16,

	G10X6_B10X6_R10X6_16,
	G10X6_B10X6_R10X6_S422_16,
	G10X6_B10X6_R10X6_S420_16,

	G10X6_R10X6B10X6_S422_16,
	G10X6_B10X6R10X6_S422_16,
	G10X6_R10X6B10X6_S420_16,
	G10X6_B10X6R10X6_S420_16,


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

	R12X4G12X4B12X4G12X4_S422_16,
	B12X4G12X4R12X4G12X4_S422_16,
	G12X4R12X4G12X4B12X4_S422_16,
	G12X4B12X4G12X4R12X4_S422_16,

	G12X4_B12X4_R12X4_16,
	G12X4_B12X4_R12X4_S422_16,
	G12X4_B12X4_R12X4_S420_16,

	G12X4_R12X4B12X4_S422_16,
	G12X4_B12X4R12X4_S422_16,
	G12X4_R12X4B12X4_S420_16,
	G12X4_B12X4R12X4_S420_16,


	//16 BIT COMPONENTS
	Y16,

	Y16A16,
	A16Y16,

	R16G16B16,
	B16G16R16,
	G16R16B16,
	G16B16R16,

	A16R16G16B16,
	A16B16G16R16,
	A16G16R16B16,
	A16G16B16R16,
	R16G16B16A16,
	B16G16R16A16,
	G16R16B16A16,
	G16B16R16A16,

	R16G16B16G16_S422,
	B16G16R16G16_S422,
	G16R16G16B16_S422,
	G16B16G16R16_S422,

	G16_B16_R16,
	G16_B16_R16_S422,
	G16_B16_R16_S420,

	G16_R16B16_S422,
	G16_B16R16_S422,
	G16_R16B16_S420,
	G16_B16R16_S420,


	//32 BIT COMPONENTS
	Y32,

	Y32A32,
	A32Y32,

	R32G32B32,
	B32G32R32,
	G32R32B32,
	G32B32R32,

	A32R32G32B32,
	A32B32G32R32,
	A32G32R32B32,
	A32G32B32R32,
	R32G32B32A32,
	B32G32R32A32,
	G32R32B32A32,
	G32B32R32A32,


	//64 BIT COMPONENTS
	Y64,

	Y64A64,
	A64Y64,

	R64G64B64,
	B64G64R64,
	G64R64B64,
	G64B64R64,

	A64R64G64B64,
	A64B64G64R64,
	A64G64R64B64,
	A64G64B64R64,
	R64G64B64A64,
	B64G64R64A64,
	G64R64B64A64,
	G64B64R64A64,

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(PixelFormat)
ZUAZO_ENUM_COMP_OPERATORS(PixelFormat)	


/**
 * ColorPrimaries represents which red, green and blue color primaries are used to 
 * sythetise color. This influences the range of representable range of colors
 */
enum class ColorPrimaries {
	NONE = 0,					///<Not supported

	BT709,						///<Used in HDTV and sRGB color space\see https://www.itu.int/rec/R-REC-BT.709/en
	BT2020,						///<Used in UHDTV \see https://www.itu.int/rec/R-REC-BT.2020/en
	SMPTE432,					///<Used in Display P3 color space
	ADOBE_RGB,					///<Used in Adobe RGB color space

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorPrimaries)
ZUAZO_ENUM_COMP_OPERATORS(ColorPrimaries)	


/**
 * ColorEncoding represents how color components are encoded in it's allocated space
 */
enum class ColorEncoding {
	NONE = 0,					///<Not supported

	UINT,						///<Linearly encoded on a unsigned integer
  	INT,						///<Linearly encoded on a integer (2's complement)
	IEEE754,					///<Linearly encoded on a floating point number
	IEC61966_2_1,				///<Non-linearly encoded on a unsigned integer according to the IEC 61966-2-1 standard. Used in sRGB and Display P3 color spaces.

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorEncoding)
ZUAZO_ENUM_COMP_OPERATORS(ColorEncoding)	


/**
 * ColorRange refers to wether all the available range is used or foot-room and head-room
 * is given to the color components
 */
enum class ColorRange {
	NONE = 0,					///<Not supported

	FULL,						///<Pixel components encompass all the available range
	NARROW,						///<Pixel components encompass a 219*2^(n-8) range

	//Add here

	COUNT 
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorRange)
ZUAZO_ENUM_COMP_OPERATORS(ColorRange)	



/**
 * ColorModel represents how color gets encoded on a tuple of values or components
 */
enum class ColorModel {
	NONE = 0,					///<Not supported

	MONOCHROME,					///<No color.
	RGB,						///<RGB color model
	BT601,						///<YCbCr color model used in SDTV \see https://www.itu.int/rec/R-REC-BT.601/en
	BT709,						///<YCbCr color model used in HDTV \see https://www.itu.int/rec/R-REC-BT.709/en
	BT2020,						///<YCbCr color model used in UHDTV \see https://www.itu.int/rec/R-REC-BT.2020/en

	//Add here

	COUNT
};

ZUAZO_ENUM_ARITHMETIC_OPERATORS(ColorModel)
ZUAZO_ENUM_COMP_OPERATORS(ColorModel)	

}

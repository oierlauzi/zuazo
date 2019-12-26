#pragma once

#include "Macros.h"

namespace Zuazo {

/**
 * The PixelFormat defines how pixel data is layed out in memory. 
 * Please, note that although pixel formats are defined for RGBA 
 * color spaces (or monochrome), they are also used for YCbCr color spaces
 */
enum class PixelFormat {
	NONE = 0,

	/*
	 * PACKED FORMATS
	 */	

	//8 bpp
	Y8,

	Y4X4_8,
	X4Y4_8,

	Y4A4_8,
	A4Y4_8,

	//16 bpp
	Y16,

	Y16f,

	Y8A8,
	A8Y8,

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

	X1R5G5B5_16,
	X1B5G5R5_16,
	X1G5R5B5_16,
	X1G5B5R5_16,
	R5G5B5X1_16,
	B5G5R5X1_16,
	G5R5B5X1_16,
	G5B5R5X1_16,

	A1G5R5B5_16,
	A1G5B5R5_16,
	A1R5G5B5_16,
	A1B5G5R5_16,
	R5G5B5A1_16,
	G5B5R5A1_16,
	G5R5B5A1_16,
	B5G5R5A1_16,

	R5G6B5_16,
	B5G6R5_16,

	R8G8B8G8_S422,
	G8R8G8B8_S422,
	B8G8R8G8_S422,
	G8B8G8R8_S422,

	//24 bpp
	R8G8B8,
	B8G8R8,

	//32 bpp
	Y32f,

	Y16A16,
	A16Y16,

	Y16fA16f,
	A16fY16f,

	X8R8G8B8,
	X8B8G8R8,
	R8G8B8X8,
	B8G8R8X8,
	X8R8G8B8_32,
	X8B8G8R8_32,
	R8G8B8X8_32,
	B8G8R8X8_32,
	A8R8G8B8,
	A8B8G8R8,
	R8G8B8A8,
	B8G8R8A8,
	A8R8G8B8_32,
	A8B8G8R8_32,
	R8G8B8A8_32,
	B8G8R8A8_32,

	X2R10G10B10_32, 
	X2B10G10R10_32,
	R10G10B10X2_32,
	B10G10R10X2_32,
	A2R10G10B10_32, 
	A2B10G10R10_32,
	R10G10B10A2_32,
	B10G10R10A2_32,

	//48 bpp
	R16G16B16,
	B16G16R16,

	R16fG16fB16f,
	B16fG16fR16f,

	//64 bpp
	Y64f,

	Y32fA32f,
	A32fY32f,

	X16R16G16B16,
	X16B16G16R16,
	R16G16B16X16,
	B16G16R16X16,
	A16R16G16B16,
	A16B16G16R16,
	R16G16B16A16,
	B16G16R16A16,

	X16fR16fG16fB16f,
	X16fB16fG16fR16f,
	R16fG16fB16fX16f,
	B16fG16fR16fX16f,
	A16fR16fG16fB16f,
	A16fB16fG16fR16f,
	R16fG16fB16fA16f,
	B16fG16fR16fA16f,

	//96 bpp
	R32fG32fB32f,
	B32fG32fR32f,

	//128 bpp
	A32fR32fG32fB32f,
	A32fB32fG32fR32f,
	R32fG32fB32fA32f,
	B32fG32fR32fA32f,

	//256 bpp
	A64fR64fG64fB64f,
	A64fB64fG64fR64f,
	R64fG64fB64fA64f,
	B64fG64fR64fA64f,


	/*
	 * PLANAR FORMATS
	 */

	//TODO

	//Add here

	COUNT
};



enum class ColorPrimaries {
	NONE = 0,					///<Not supported

	BT709,						///<HDTV and sRGB \see https://www.itu.int/rec/R-REC-BT.709/en
	BT2020,						///<UHDTV \see https://www.itu.int/rec/R-REC-BT.2020/en
	SMPTE432,					///<Display P3 color primaries
	ADOBE_RGB,					///<Adobe RGB color primaries

	//Add here

	COUNT
};



enum class ColorTransferFunction {
	NONE = 0,					///<Not supported

  	LINEAR,						///<Linear transfer characteristic
	IEC61966_2_1,				///<sRGB and Display P3

	//Add here

	COUNT
};



enum class ColorModel {
	NONE = 0,					///<Not supported

	MONOCHROME,					///<No color.
	RGB,						///<RGB color model
	BT601,						///<SDTV YCbCr color model \see https://www.itu.int/rec/R-REC-BT.601/en
	BT709,						///<HDTV YCbCr color model \see https://www.itu.int/rec/R-REC-BT.709/en
	BT2020,						///<UHDTV YCbCr color model \see https://www.itu.int/rec/R-REC-BT.2020/en

	//Add here

	COUNT
};

enum class ColorRange {
	NONE = 0,					///<Not supported

	FULL,						///<Pixel components encompass all the available range
	NARROW,						///<Pixel components encompass a 219*2^(n-8) range

	//Add here

	COUNT 
};



}

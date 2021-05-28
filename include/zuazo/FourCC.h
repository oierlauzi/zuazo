#pragma once

#include "ColorFormat.h"
#include "ColorSubsampling.h"
#include "ColorModel.h"

#include <tuple>
#include <cstdint>

namespace Zuazo {

//Required to be implemented here
constexpr uint32_t makeFourCC(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3) noexcept {
	return 	static_cast<uint32_t>(c0) << 0*Utils::getByteSize() |
			static_cast<uint32_t>(c1) << 1*Utils::getByteSize() |
			static_cast<uint32_t>(c2) << 2*Utils::getByteSize() |
			static_cast<uint32_t>(c3) << 3*Utils::getByteSize() ;
}

constexpr uint32_t makeFourCC(const char fcc[4]) noexcept {
	return makeFourCC(fcc[0], fcc[1], fcc[2], fcc[3]);
}

enum class FourCC : uint32_t {
	//RGB(A)-like
	RGBX	= makeFourCC("RGBX"),
	RGBA	= makeFourCC("RGBA"),
	XRGB	= makeFourCC("XRGB"),
	ARGB	= makeFourCC("ARGB"),
	BGRX	= makeFourCC("BGRX"),
	BGRA	= makeFourCC("BGRA"),
	XBGR	= makeFourCC("XBGR"),
	ABGR	= makeFourCC("ABGR"),

	//Grayscale
	Y800	= makeFourCC("Y800"),	///<Simple, single Y plane for monochrome images
	GREY	= makeFourCC("GREY"),	///<Apparently a duplicate of Y800

	//Packed YUV
	UYVY	= makeFourCC("UYVY"),	///<A macropixel contains 2 pixels in 1 uint32.
	HDYC	= makeFourCC("HDYC"),	///<UYVY except that the color components use the BT709
	YVYU	= makeFourCC("YVYU"),	///<Same as UYVY but with different component ordering within uint32
	YUYV	= makeFourCC("YUYV"),	///<Same as UYVY but with different component ordering within uint32
	YUY2	= makeFourCC("YUY2"),	///<Same as YUYV

	Y216 	= makeFourCC("Y216"),	///<Packed, 4:2:2, 16-bit.
	Y210 	= makeFourCC("Y210"),	///<Packed, 4:2:2, 10-bit.
	Y416 	= makeFourCC("Y416"),	///<Packed, 4:4:4, 16-bit
	Y410 	= makeFourCC("Y410"),	///<Packed, 4:4:4, 10-bit.

	AYUV	= makeFourCC("AYUV"),	///<Combined YUV and alpha 32bpp
	UYVA	= makeFourCC("UYVA"),	///<Combined YUV and alpha 32bpp
		
	//Planar YUV
	NV12	= makeFourCC("NV12"),	///<8-bit Y plane followed by an interleaved U/V plane with 2x2 subsampling
	NV21	= makeFourCC("NV21"),	///<As NV12 with U and V reversed in the interleaved plane
	NV16	= makeFourCC("NV16"),	///<8-bit Y plane followed by an interleaved U/V plane with 2x1 subsampling
	NV61	= makeFourCC("NV61"),	///<As NV16 with U and V reversed in the interleaved plane
	NV24	= makeFourCC("NV24"),	///<8-bit Y plane followed by an interleaved U/V plane with no subsampling
	NV42	= makeFourCC("NV42"),	///<As NV16 with U and V reversed in the interleaved plane

	P016 	= makeFourCC("P016"), 	///<Planar, 4:2:0, 16-bit.
	P010 	= makeFourCC("P010"), 	///<Planar, 4:2:0, 10-bit.
	P216 	= makeFourCC("P216"), 	///<Planar, 4:2:2, 16-bit.
	P210 	= makeFourCC("P210"), 	///<Planar, 4:2:2, 10-bit.

	PA16	= makeFourCC("PA16"),	///<Like P216 but with a third alpha plane

	I420	= makeFourCC("I420"),	///<8 bit Y plane followed by 8 bit 2x2 subsampled U and V planes.
	I422	= makeFourCC("I422"),	///<8 bit Y plane followed by 8 bit 2x1 subsampled U and V planes.
	I444	= makeFourCC("I444"),	///<8 bit Y plane followed by 8 bit U and V planes (no subsampling).

	YV12	= makeFourCC("YV12"),	///<8 bit Y plane followed by 8 bit 2x2 subsampled V and U planes.
	YV16	= makeFourCC("YV16"),	///<8 bit Y plane followed by 8 bit 2x1 subsampled V and U planes.

};

constexpr std::tuple<ColorFormat, ColorSubsampling, ColorModel> fromFourCC(	FourCC fcc, 
																			ColorModel ycbcrModel = ColorModel::yuv ) noexcept;

std::string_view toString(const FourCC& fcc) noexcept;
std::ostream& operator<<(std::ostream& os, FourCC fcc);

}

#include "FourCC.inl"

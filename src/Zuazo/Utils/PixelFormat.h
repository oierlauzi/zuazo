#pragma once

extern "C"{
	#include <libavutil/pixfmt.h>
}
#include <string>

namespace Zuazo::Utils{

/**
 * @brief Pixel types supported by Zuazo
 *
 * @note This pixel formats have been borrowed (Ctrl + C, Ctrl + V) from libavutil/pixfmt.h
 * so they are equivalent to the respective AV_PIX_FMT_xxxxxxxx
 */
class PixelFormat{
public:
	constexpr PixelFormat();
	constexpr PixelFormat(AVPixelFormat fmt);
	PixelFormat(const std::string& name);
	PixelFormat(const PixelFormat& other)=default;
	~PixelFormat()=default;

	constexpr int 			operator==(const PixelFormat& other) const;
	constexpr int 			operator!=(const PixelFormat& other) const;
	constexpr int 			operator<(const PixelFormat& other) const;
	constexpr int 			operator>(const PixelFormat& other) const;
	constexpr int 			operator<=(const PixelFormat& other) const;
	constexpr int 			operator>=(const PixelFormat& other) const;

	constexpr AVPixelFormat toAVPixelFormat() const;
	std::string 			getPixelFormatName() const;

	bool 					hasAlpha() const;
	bool 					isBigEndian() const;
	int 					getBitsPerPixel() const;
	int 					getPlanesCount();
	PixelFormat 			swapEndianness() const;
private:
	AVPixelFormat 			fmt;
};

constexpr PixelFormat::PixelFormat() :
		fmt(AV_PIX_FMT_NONE)
{
}

constexpr PixelFormat::PixelFormat(AVPixelFormat fmt) :
		fmt(fmt)
{
}

constexpr int PixelFormat::operator==(const PixelFormat& other) const{
	return fmt==other.fmt;
}
constexpr int PixelFormat::operator!=(const PixelFormat& other) const{
	return fmt!=other.fmt;
}
constexpr int PixelFormat::operator<(const PixelFormat& other) const{
	return fmt<other.fmt;
}
constexpr int PixelFormat::operator>(const PixelFormat& other) const{
	return fmt>other.fmt;
}
constexpr int PixelFormat::operator<=(const PixelFormat& other) const{
	return fmt<=other.fmt;
}
constexpr int PixelFormat::operator>=(const PixelFormat& other) const{
	return fmt>=other.fmt;
}

constexpr AVPixelFormat PixelFormat::toAVPixelFormat() const{
	return fmt;
}

namespace PixelFormats{

constexpr PixelFormat NONE=AV_PIX_FMT_NONE;
constexpr PixelFormat YUV420P=AV_PIX_FMT_YUV420P;   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
constexpr PixelFormat YUYV422=AV_PIX_FMT_YUYV422;   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
constexpr PixelFormat RGB24=AV_PIX_FMT_RGB24;     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
constexpr PixelFormat BGR24=AV_PIX_FMT_BGR24;     ///< packed RGB 8:8:8, 24bpp, BGRBGR...
constexpr PixelFormat YUV422P=AV_PIX_FMT_YUV422P;   ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
constexpr PixelFormat YUV444P=AV_PIX_FMT_YUV444P;   ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
constexpr PixelFormat YUV410P=AV_PIX_FMT_YUV410P;   ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
constexpr PixelFormat YUV411P=AV_PIX_FMT_YUV411P;   ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
constexpr PixelFormat GRAY8=AV_PIX_FMT_GRAY8;     ///<        Y        ,  8bpp
constexpr PixelFormat MONOWHITE=AV_PIX_FMT_MONOWHITE; ///<        Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
constexpr PixelFormat MONOBLACK=AV_PIX_FMT_MONOBLACK; ///<        Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
constexpr PixelFormat PAL8=AV_PIX_FMT_PAL8;      ///< 8 bits with AV_PIX_FMT_RGB32 palette
constexpr PixelFormat YUVJ420P=AV_PIX_FMT_YUVJ420P;  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV420P and setting color_range
constexpr PixelFormat YUVJ422P=AV_PIX_FMT_YUVJ422P;  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV422P and setting color_range
constexpr PixelFormat YUVJ444P=AV_PIX_FMT_YUVJ444P;  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV444P and setting color_range
constexpr PixelFormat UYVY422=AV_PIX_FMT_UYVY422;   ///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
constexpr PixelFormat UYYVYY411=AV_PIX_FMT_UYYVYY411; ///< packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
constexpr PixelFormat BGR8=AV_PIX_FMT_BGR8;      ///< packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
constexpr PixelFormat BGR4=AV_PIX_FMT_BGR4;      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
constexpr PixelFormat BGR4_BYTE=AV_PIX_FMT_BGR4_BYTE; ///< packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
constexpr PixelFormat RGB8=AV_PIX_FMT_RGB8;      ///< packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
constexpr PixelFormat RGB4=AV_PIX_FMT_RGB4;      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
constexpr PixelFormat RGB4_BYTE=AV_PIX_FMT_RGB4_BYTE; ///< packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
constexpr PixelFormat NV12=AV_PIX_FMT_NV12;      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
constexpr PixelFormat NV21=AV_PIX_FMT_NV21;      ///< as above, but U and V bytes are swapped

constexpr PixelFormat ARGB=AV_PIX_FMT_ARGB;      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
constexpr PixelFormat RGBA=AV_PIX_FMT_RGBA;      ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
constexpr PixelFormat ABGR=AV_PIX_FMT_ABGR;      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
constexpr PixelFormat BGRA=AV_PIX_FMT_BGRA;      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...

constexpr PixelFormat GRAY16BE=AV_PIX_FMT_GRAY16BE;  ///<        Y        , 16bpp, big-endian
constexpr PixelFormat GRAY16LE=AV_PIX_FMT_GRAY16LE;  ///<        Y        , 16bpp, little-endian
constexpr PixelFormat YUV440P=AV_PIX_FMT_YUV440P;   ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
constexpr PixelFormat YUVJ440P=AV_PIX_FMT_YUVJ440P;  ///< planar YUV 4:4:0 full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV440P and setting color_range
constexpr PixelFormat YUVA420P=AV_PIX_FMT_YUVA420P;  ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
constexpr PixelFormat RGB48BE=AV_PIX_FMT_RGB48BE;   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian
constexpr PixelFormat RGB48LE=AV_PIX_FMT_RGB48LE;   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian

constexpr PixelFormat RGB565BE=AV_PIX_FMT_RGB565BE;  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
constexpr PixelFormat RGB565LE=AV_PIX_FMT_RGB565LE;  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
constexpr PixelFormat RGB555BE=AV_PIX_FMT_RGB555BE;  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), big-endian   , X=unused/undefined
constexpr PixelFormat RGB555LE=AV_PIX_FMT_RGB555LE;  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), little-endian, X=unused/undefined

constexpr PixelFormat BGR565BE=AV_PIX_FMT_BGR565BE;  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
constexpr PixelFormat BGR565LE=AV_PIX_FMT_BGR565LE;  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
constexpr PixelFormat BGR555BE=AV_PIX_FMT_BGR555BE;  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), big-endian   , X=unused/undefined
constexpr PixelFormat BGR555LE=AV_PIX_FMT_BGR555LE;  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), little-endian, X=unused/undefined

/**
    *  Hardware acceleration through VA-API, data[3] contains a
    *  VASurfaceID.
    */
constexpr PixelFormat VAAPI=AV_PIX_FMT_VAAPI;

constexpr PixelFormat YUV420P16LE=AV_PIX_FMT_YUV420P16LE;  	///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
constexpr PixelFormat YUV420P16BE=AV_PIX_FMT_YUV420P16BE;  	///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
constexpr PixelFormat YUV422P16LE=AV_PIX_FMT_YUV422P16LE;  	///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
constexpr PixelFormat YUV422P16BE=AV_PIX_FMT_YUV422P16BE; 	///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
constexpr PixelFormat YUV444P16LE=AV_PIX_FMT_YUV444P16LE;  	///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
constexpr PixelFormat YUV444P16BE=AV_PIX_FMT_YUV444P16BE;  	///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
constexpr PixelFormat DXVA2_VLD=AV_PIX_FMT_DXVA2_VLD;    	///< HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer

constexpr PixelFormat RGB444LE=AV_PIX_FMT_RGB444LE;  		///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), little-endian, X=unused/undefined
constexpr PixelFormat RGB444BE=AV_PIX_FMT_RGB444BE;  		///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), big-endian,    X=unused/undefined
constexpr PixelFormat BGR444LE=AV_PIX_FMT_BGR444LE;  		///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), little-endian, X=unused/undefined
constexpr PixelFormat BGR444BE=AV_PIX_FMT_BGR444BE;  		///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), big-endian,    X=unused/undefined
constexpr PixelFormat YA8=AV_PIX_FMT_YA8;       		///< 8 bits gray, 8 bits alpha

constexpr PixelFormat Y400A=AV_PIX_FMT_Y400A; 			///< alias for AV_PIX_FMT_YA8
constexpr PixelFormat GRAY8A=AV_PIX_FMT_GRAY8A; 		///< alias for AV_PIX_FMT_YA8

constexpr PixelFormat BGR48BE=AV_PIX_FMT_BGR48BE;  		///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
constexpr PixelFormat BGR48LE=AV_PIX_FMT_BGR48LE;   		///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian

/**
    * The following 12 formats have the disadvantage of needing 1 format for each bit depth.
    * Notice that each 9/10 bits sample is stored in 16 bits with extra padding.
    * If you want to support multiple bit depths, then using AV_PIX_FMT_YUV420P16* with the bpp stored separately is better.
    */
constexpr PixelFormat YUV420P9BE=AV_PIX_FMT_YUV420P9BE; 	///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
constexpr PixelFormat YUV420P9LE=AV_PIX_FMT_YUV420P9LE; 	///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
constexpr PixelFormat YUV420P10BE=AV_PIX_FMT_YUV420P10BE;	///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
constexpr PixelFormat YUV420P10LE=AV_PIX_FMT_YUV420P10LE;	///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
constexpr PixelFormat YUV422P10BE=AV_PIX_FMT_YUV422P10BE;	///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
constexpr PixelFormat YUV422P10LE=AV_PIX_FMT_YUV422P10LE;	///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
constexpr PixelFormat YUV444P9BE=AV_PIX_FMT_YUV444P9BE; 	///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
constexpr PixelFormat YUV444P9LE=AV_PIX_FMT_YUV444P9LE; 	///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
constexpr PixelFormat YUV444P10BE=AV_PIX_FMT_YUV444P10BE;	///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
constexpr PixelFormat YUV444P10LE=AV_PIX_FMT_YUV444P10LE;	///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
constexpr PixelFormat YUV422P9BE=AV_PIX_FMT_YUV422P9BE; 	///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
constexpr PixelFormat YUV422P9LE=AV_PIX_FMT_YUV422P9LE; 	///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
constexpr PixelFormat GBRP=AV_PIX_FMT_GBRP;      	///< planar GBR 4:4:4 24bpp
constexpr PixelFormat GBR24P=AV_PIX_FMT_GBR24P; 		/// alias for #AV_PIX_FMT_GBRP
constexpr PixelFormat GBRP9BE=AV_PIX_FMT_GBRP9BE;   	///< planar GBR 4:4:4 27bpp, big-endian
constexpr PixelFormat GBRP9LE=AV_PIX_FMT_GBRP9LE;   	///< planar GBR 4:4:4 27bpp, little-endian
constexpr PixelFormat GBRP10BE=AV_PIX_FMT_GBRP10BE; 		///< planar GBR 4:4:4 30bpp, big-endian
constexpr PixelFormat GBRP10LE=AV_PIX_FMT_GBRP10LE;  	///< planar GBR 4:4:4 30bpp, little-endian
constexpr PixelFormat GBRP16BE=AV_PIX_FMT_GBRP16BE;  	///< planar GBR 4:4:4 48bpp, big-endian
constexpr PixelFormat GBRP16LE=AV_PIX_FMT_GBRP16LE;  	///< planar GBR 4:4:4 48bpp, little-endian
constexpr PixelFormat YUVA422P=AV_PIX_FMT_YUVA422P;  	///< planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
constexpr PixelFormat YUVA444P=AV_PIX_FMT_YUVA444P;  	///< planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)
constexpr PixelFormat YUVA420P9BE=AV_PIX_FMT_YUVA420P9BE;  	///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian
constexpr PixelFormat YUVA420P9LE=AV_PIX_FMT_YUVA420P9LE;  	///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian
constexpr PixelFormat YUVA422P9BE=AV_PIX_FMT_YUVA422P9BE;  	///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian
constexpr PixelFormat YUVA422P9LE=AV_PIX_FMT_YUVA422P9LE;  	///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian
constexpr PixelFormat YUVA444P9BE=AV_PIX_FMT_YUVA444P9BE;  	///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian
constexpr PixelFormat YUVA444P9LE=AV_PIX_FMT_YUVA444P9LE;  	///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
constexpr PixelFormat YUVA420P10BE=AV_PIX_FMT_YUVA420P10BE;	///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
constexpr PixelFormat YUVA420P10LE=AV_PIX_FMT_YUVA420P10LE;	///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
constexpr PixelFormat YUVA422P10BE=AV_PIX_FMT_YUVA422P10BE; 	///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
constexpr PixelFormat YUVA422P10LE=AV_PIX_FMT_YUVA422P10LE; 	///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
constexpr PixelFormat YUVA444P10BE=AV_PIX_FMT_YUVA444P10BE; 	///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
constexpr PixelFormat YUVA444P10LE=AV_PIX_FMT_YUVA444P10LE;	///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)
constexpr PixelFormat YUVA420P16BE=AV_PIX_FMT_YUVA420P16BE; 	///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
constexpr PixelFormat YUVA420P16LE=AV_PIX_FMT_YUVA420P16LE; 	///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
constexpr PixelFormat YUVA422P16BE=AV_PIX_FMT_YUVA422P16BE; 	///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
constexpr PixelFormat YUVA422P16LE=AV_PIX_FMT_YUVA422P16LE; 	///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
constexpr PixelFormat YUVA444P16BE=AV_PIX_FMT_YUVA444P16BE; 	///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
constexpr PixelFormat YUVA444P16LE=AV_PIX_FMT_YUVA444P16LE; 	///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)

constexpr PixelFormat VDPAU=AV_PIX_FMT_VDPAU;    		///< HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface

constexpr PixelFormat XYZ12LE=AV_PIX_FMT_XYZ12LE;      	///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0
constexpr PixelFormat XYZ12BE=AV_PIX_FMT_XYZ12BE;     	///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0
constexpr PixelFormat NV16=AV_PIX_FMT_NV16;         	///< interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
constexpr PixelFormat NV20LE=AV_PIX_FMT_NV20LE;       	///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
constexpr PixelFormat NV20BE=AV_PIX_FMT_NV20BE;       	///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian

constexpr PixelFormat RGBA64BE=AV_PIX_FMT_RGBA64BE;     	///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
constexpr PixelFormat RGBA64LE=AV_PIX_FMT_RGBA64LE;     	///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
constexpr PixelFormat BGRA64BE=AV_PIX_FMT_BGRA64BE;     	///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
constexpr PixelFormat BGRA64LE=AV_PIX_FMT_BGRA64LE;     	///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian

constexpr PixelFormat YVYU422=AV_PIX_FMT_YVYU422;   		///< packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb

constexpr PixelFormat YA16BE=AV_PIX_FMT_YA16BE;       	///< 16 bits gray, 16 bits alpha (big-endian)
constexpr PixelFormat YA16LE=AV_PIX_FMT_YA16LE;       	///< 16 bits gray, 16 bits alpha (little-endian)

constexpr PixelFormat GBRAP=AV_PIX_FMT_GBRAP;        	///< planar GBRA 4:4:4:4 32bpp
constexpr PixelFormat GBRAP16BE=AV_PIX_FMT_GBRAP16BE;   	///< planar GBRA 4:4:4:4 64bpp, big-endian
constexpr PixelFormat GBRAP16LE=AV_PIX_FMT_GBRAP16LE;    	///< planar GBRA 4:4:4:4 64bpp, little-endian
/**
    *  HW acceleration through QSV, data[3] contains a pointer to the
    *  mfxFrameSurface1 structure.
    */
constexpr PixelFormat QSV=AV_PIX_FMT_QSV;
/**
    * HW acceleration though MMAL, data[3] contains a pointer to the
    * MMAL_BUFFER_HEADER_T structure.
    */
constexpr PixelFormat MMAL=AV_PIX_FMT_MMAL;

constexpr PixelFormat D3D11VA_VLD=AV_PIX_FMT_D3D11VA_VLD;  	///< HW decoding through Direct3D11 via old API, Picture.data[3] contains a ID3D11VideoDecoderOutputView pointer

/**
    * HW acceleration through CUDA. data[i] contain CUdeviceptr pointers
    * exactly as for system memory frames.
    */
constexpr PixelFormat CUDA=AV_PIX_FMT_CUDA;

constexpr PixelFormat XRGB=AV_PIX_FMT_0RGB;			///< packed RGB 8:8:8, 32bpp, XRGBXRGB...   X=unused/undefined
constexpr PixelFormat RGBX=AV_PIX_FMT_RGB0;       	///< packed RGB 8:8:8, 32bpp, RGBXRGBX...   X=unused/undefined
constexpr PixelFormat XBGR=AV_PIX_FMT_0BGR;        	///< packed BGR 8:8:8, 32bpp, XBGRXBGR...   X=unused/undefined
constexpr PixelFormat BGRX=AV_PIX_FMT_BGR0; 			///< packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined

constexpr PixelFormat YUV420P12BE=AV_PIX_FMT_YUV420P12BE; 	///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
constexpr PixelFormat YUV420P12LE=AV_PIX_FMT_YUV420P12LE; 	///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
constexpr PixelFormat YUV420P14BE=AV_PIX_FMT_YUV420P14BE; 	///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
constexpr PixelFormat YUV420P14LE=AV_PIX_FMT_YUV420P14LE; 	///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
constexpr PixelFormat YUV422P12BE=AV_PIX_FMT_YUV422P12BE; ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
constexpr PixelFormat YUV422P12LE=AV_PIX_FMT_YUV422P12LE; ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
constexpr PixelFormat YUV422P14BE=AV_PIX_FMT_YUV422P14BE; ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
constexpr PixelFormat YUV422P14LE=AV_PIX_FMT_YUV422P14LE; ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
constexpr PixelFormat YUV444P12BE=AV_PIX_FMT_YUV444P12BE; ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
constexpr PixelFormat YUV444P12LE=AV_PIX_FMT_YUV444P12LE; ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
constexpr PixelFormat YUV444P14BE=AV_PIX_FMT_YUV444P14BE; ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
constexpr PixelFormat YUV444P14LE=AV_PIX_FMT_YUV444P14LE; ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
constexpr PixelFormat GBRP12BE=AV_PIX_FMT_GBRP12BE; ///< planar GBR 4:4:4 36bpp, big-endian
constexpr PixelFormat GBRP12LE=AV_PIX_FMT_GBRP12LE; ///< planar GBR 4:4:4 36bpp, little-endian
constexpr PixelFormat GBRP14BE=AV_PIX_FMT_GBRP14BE; ///< planar GBR 4:4:4 42bpp, big-endian
constexpr PixelFormat GBRP14LE=AV_PIX_FMT_GBRP14LE; ///< planar GBR 4:4:4 42bpp, little-endian
constexpr PixelFormat YUVJ411P=AV_PIX_FMT_YUVJ411P; ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV411P and setting color_range

constexpr PixelFormat BAYER_BGGR8=AV_PIX_FMT_BAYER_BGGR8; ///< bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples */
constexpr PixelFormat BAYER_RGGB8=AV_PIX_FMT_BAYER_RGGB8; ///< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples */
constexpr PixelFormat BAYER_GBRG8=AV_PIX_FMT_BAYER_GBRG8; ///< bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples */
constexpr PixelFormat BAYER_GRBG8=AV_PIX_FMT_BAYER_GRBG8; ///< bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples */
constexpr PixelFormat BAYER_BGGR16LE=AV_PIX_FMT_BAYER_BGGR16LE; ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, little-endian */
constexpr PixelFormat BAYER_BGGR16BE=AV_PIX_FMT_BAYER_BGGR16BE; ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, big-endian */
constexpr PixelFormat BAYER_RGGB16LE=AV_PIX_FMT_BAYER_RGGB16LE; ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, little-endian */
constexpr PixelFormat BAYER_RGGB16BE=AV_PIX_FMT_BAYER_RGGB16BE; ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, big-endian */
constexpr PixelFormat BAYER_GBRG16LE=AV_PIX_FMT_BAYER_GBRG16LE; ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, little-endian */
constexpr PixelFormat BAYER_GBRG16BE=AV_PIX_FMT_BAYER_GBRG16BE; ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, big-endian */
constexpr PixelFormat BAYER_GRBG16LE=AV_PIX_FMT_BAYER_GRBG16LE; ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, little-endian */
constexpr PixelFormat BAYER_GRBG16BE=AV_PIX_FMT_BAYER_GRBG16BE; ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, big-endian */

constexpr PixelFormat XVMC=AV_PIX_FMT_XVMC; ///< XVideo Motion Acceleration via common packet passing

constexpr PixelFormat YUV440P10LE=AV_PIX_FMT_YUV440P10LE; ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
constexpr PixelFormat YUV440P10BE=AV_PIX_FMT_YUV440P10BE; ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
constexpr PixelFormat YUV440P12LE=AV_PIX_FMT_YUV440P12LE; ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
constexpr PixelFormat YUV440P12BE=AV_PIX_FMT_YUV440P12BE; ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
constexpr PixelFormat AYUV64LE=AV_PIX_FMT_AYUV64LE; ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
constexpr PixelFormat AYUV64BE=AV_PIX_FMT_AYUV64BE; ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), big-endian

constexpr PixelFormat VIDEOTOOLBOX=AV_PIX_FMT_VIDEOTOOLBOX; ///< hardware decoding through Videotoolbox

constexpr PixelFormat P010LE=AV_PIX_FMT_P010LE; ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, little-endian
constexpr PixelFormat P010BE=AV_PIX_FMT_P010BE; ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, big-endian

constexpr PixelFormat GBRAP12BE=AV_PIX_FMT_GBRAP12BE; ///< planar GBR 4:4:4:4 48bpp, big-endian
constexpr PixelFormat GBRAP12LE=AV_PIX_FMT_GBRAP12LE; ///< planar GBR 4:4:4:4 48bpp, little-endian

constexpr PixelFormat GBRAP10BE=AV_PIX_FMT_GBRAP10BE; ///< planar GBR 4:4:4:4 40bpp, big-endian
constexpr PixelFormat GBRAP10LE=AV_PIX_FMT_GBRAP10LE; ///< planar GBR 4:4:4:4 40bpp, little-endian

constexpr PixelFormat MEDIACODEC=AV_PIX_FMT_MEDIACODEC; ///< hardware decoding through MediaCodec

constexpr PixelFormat GRAY12BE=AV_PIX_FMT_GRAY12BE; ///<        Y        , 12bpp, big-endian
constexpr PixelFormat GRAY12LE=AV_PIX_FMT_GRAY12LE; ///<        Y        , 12bpp, little-endian
constexpr PixelFormat GRAY10BE=AV_PIX_FMT_GRAY10BE; ///<        Y        , 10bpp, big-endian
constexpr PixelFormat GRAY10LE=AV_PIX_FMT_GRAY10LE; ///<        Y        , 10bpp, little-endian

constexpr PixelFormat P016LE=AV_PIX_FMT_P016LE; ///< like NV12, with 16bpp per component, little-endian
constexpr PixelFormat P016BE=AV_PIX_FMT_P016BE; ///< like NV12, with 16bpp per component, big-endian

/**
    * Hardware surfaces for Direct3D11.
    *
    * This is preferred over the legacy AV_PIX_FMT_D3D11VA_VLD. The new D3D11
    * hwaccel API and filtering support AV_PIX_FMT_D3D11 only.
    *
    * data[0] contains a ID3D11Texture2D pointer, and data[1] contains the
    * texture array index of the frame as intptr_t if the ID3D11Texture2D is
    * an array texture (or always 0 if it's a normal texture).
    */
constexpr PixelFormat D3D11=AV_PIX_FMT_D3D11;

constexpr PixelFormat GRAY9BE=AV_PIX_FMT_GRAY9BE; ///<        Y        , 9bpp, big-endian
constexpr PixelFormat GRAY9LE=AV_PIX_FMT_GRAY9LE; ///<        Y        , 9bpp, little-endian

constexpr PixelFormat GBRPF32BE=AV_PIX_FMT_GBRPF32BE; ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, big-endian
constexpr PixelFormat GBRPF32LE=AV_PIX_FMT_GBRPF32LE; ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, little-endian
constexpr PixelFormat GBRAPF32BE=AV_PIX_FMT_GBRAPF32BE; ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, big-endian
constexpr PixelFormat GBRAPF32LE=AV_PIX_FMT_GBRAPF32LE; ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, little-endian

/**
    * DRM-managed buffers exposed through PRIME buffer sharing.
    *
    * data[0] points to an AVDRMFrameDescriptor.
    */
constexpr PixelFormat DRM_PRIME=AV_PIX_FMT_DRM_PRIME;
/**
    * Hardware surfaces for OpenCL.
    *
    * data[i] contain 2D image objects (typed in C as cl_mem, used
    * in OpenCL as image2d_t) for each plane of the surface.
    */
constexpr PixelFormat OPENCL=AV_PIX_FMT_OPENCL;

constexpr PixelFormat GRAY14BE=AV_PIX_FMT_GRAY14BE; ///<        Y        , 14bpp, big-endian
constexpr PixelFormat GRAY14LE=AV_PIX_FMT_GRAY14LE; ///<        Y        , 14bpp, little-endian

constexpr PixelFormat GRAYF32BE=AV_PIX_FMT_GRAYF32BE; ///< IEEE-754 single precision Y, 32bpp, big-endian
constexpr PixelFormat GRAYF32LE=AV_PIX_FMT_GRAYF32LE; ///< IEEE-754 single precision Y, 32bpp, little-endian

constexpr PixelFormat RGB32=AV_PIX_FMT_RGB32;
constexpr PixelFormat RGB32_1=AV_PIX_FMT_RGB32_1;
constexpr PixelFormat BGR32=AV_PIX_FMT_BGR32;
constexpr PixelFormat BGR32_1=AV_PIX_FMT_BGR32_1;
constexpr PixelFormat XRGB32=AV_PIX_FMT_RGB32;
constexpr PixelFormat XBGR32=AV_PIX_FMT_BGR32;

constexpr PixelFormat GRAY9=AV_PIX_FMT_GRAY9;
constexpr PixelFormat GRAY10=AV_PIX_FMT_GRAY10;
constexpr PixelFormat GRAY12=AV_PIX_FMT_GRAY12;
constexpr PixelFormat GRAY14=AV_PIX_FMT_GRAY14;
constexpr PixelFormat GRAY16=AV_PIX_FMT_GRAY16;
constexpr PixelFormat YA16=AV_PIX_FMT_YA16;
constexpr PixelFormat RGB48=AV_PIX_FMT_RGB48;
constexpr PixelFormat RGB565=AV_PIX_FMT_RGB565;
constexpr PixelFormat RGB555=AV_PIX_FMT_RGB555;
constexpr PixelFormat RGB444=AV_PIX_FMT_RGB444;
constexpr PixelFormat RGBA64=AV_PIX_FMT_RGBA64;
constexpr PixelFormat BGR48=AV_PIX_FMT_BGR48;
constexpr PixelFormat BGR565=AV_PIX_FMT_BGR565;
constexpr PixelFormat BGR555=AV_PIX_FMT_BGR555;
constexpr PixelFormat BGR444=AV_PIX_FMT_BGR444;
constexpr PixelFormat BGRA64=AV_PIX_FMT_BGRA64;

constexpr PixelFormat YUV420P9=AV_PIX_FMT_YUV420P9;
constexpr PixelFormat YUV422P9=AV_PIX_FMT_YUV422P9;
constexpr PixelFormat YUV444P9=AV_PIX_FMT_YUV444P9;
constexpr PixelFormat YUV420P10=AV_PIX_FMT_YUV420P10;
constexpr PixelFormat YUV422P10=AV_PIX_FMT_YUV422P10;
constexpr PixelFormat YUV440P10=AV_PIX_FMT_YUV440P10;
constexpr PixelFormat YUV444P10=AV_PIX_FMT_YUV444P10;
constexpr PixelFormat YUV420P12=AV_PIX_FMT_YUV420P12;
constexpr PixelFormat YUV422P12=AV_PIX_FMT_YUV422P12;
constexpr PixelFormat YUV440P12=AV_PIX_FMT_YUV440P12;
constexpr PixelFormat YUV444P12=AV_PIX_FMT_YUV444P12;
constexpr PixelFormat YUV420P14=AV_PIX_FMT_YUV420P14;
constexpr PixelFormat YUV422P14=AV_PIX_FMT_YUV422P14;
constexpr PixelFormat YUV444P14=AV_PIX_FMT_YUV444P14;
constexpr PixelFormat YUV420P16=AV_PIX_FMT_YUV420P16;
constexpr PixelFormat YUV422P16=AV_PIX_FMT_YUV422P16;
constexpr PixelFormat YUV444P16=AV_PIX_FMT_YUV444P16;

constexpr PixelFormat GBRP9=AV_PIX_FMT_GBRP9;
constexpr PixelFormat GBRP10=AV_PIX_FMT_GBRP10;
constexpr PixelFormat GBRP12=AV_PIX_FMT_GBRP12;
constexpr PixelFormat GBRP14=AV_PIX_FMT_GBRP14;
constexpr PixelFormat GBRP16=AV_PIX_FMT_GBRP16;
constexpr PixelFormat GBRAP10=AV_PIX_FMT_GBRAP10;
constexpr PixelFormat GBRAP12=AV_PIX_FMT_GBRAP12;
constexpr PixelFormat GBRAP16=AV_PIX_FMT_GBRAP16;

constexpr PixelFormat BAYER_BGGR16=AV_PIX_FMT_BAYER_BGGR16;
constexpr PixelFormat BAYER_RGGB16=AV_PIX_FMT_BAYER_RGGB16;
constexpr PixelFormat BAYER_GBRG16=AV_PIX_FMT_BAYER_GBRG16;
constexpr PixelFormat BAYER_GRBG16=AV_PIX_FMT_BAYER_GRBG16;

constexpr PixelFormat GBRPF32=AV_PIX_FMT_GBRPF32;
constexpr PixelFormat GBRAPF32=AV_PIX_FMT_GBRAPF32;

constexpr PixelFormat GRAYF32=AV_PIX_FMT_GRAYF32;

constexpr PixelFormat YUVA420P9=AV_PIX_FMT_YUVA420P9;
constexpr PixelFormat YUVA422P9=AV_PIX_FMT_YUVA422P9;
constexpr PixelFormat YUVA444P9=AV_PIX_FMT_YUVA444P9;
constexpr PixelFormat YUVA420P10=AV_PIX_FMT_YUVA420P10;
constexpr PixelFormat YUVA422P10=AV_PIX_FMT_YUVA422P10;
constexpr PixelFormat YUVA444P10=AV_PIX_FMT_YUVA444P10;
constexpr PixelFormat YUVA420P16=AV_PIX_FMT_YUVA420P16;
constexpr PixelFormat YUVA422P16=AV_PIX_FMT_YUVA422P16;
constexpr PixelFormat YUVA444P16=AV_PIX_FMT_YUVA444P16;

constexpr PixelFormat XYZ12=AV_PIX_FMT_XYZ12;
constexpr PixelFormat NV20=AV_PIX_FMT_NV20;
constexpr PixelFormat AYUV64=AV_PIX_FMT_AYUV64;
constexpr PixelFormat P010=AV_PIX_FMT_P010;
constexpr PixelFormat P016=AV_PIX_FMT_P016;

}
}

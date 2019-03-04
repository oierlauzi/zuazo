#pragma once

#include <string>

extern "C"{
	#include <libavutil/pixdesc.h>
	#include <libavutil/pixfmt.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/version.h>
}

namespace Zuazo::Utils{

/**
 * @brief Pixel types supported by Zuazo
 *
 * @note This pixel formats have been borrowed (Ctrl + C, Ctrl + V) from libavutil/pixfmt.h
 * so they are equivalent to the respective AV_PIX_FMT_xxxxxxxx
 */
enum class PixelFormats{
	NONE 				=AV_PIX_FMT_NONE,
	YUV420P 			=AV_PIX_FMT_YUV420P,   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
	YUYV422 			=AV_PIX_FMT_YUYV422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
	RGB24 				=AV_PIX_FMT_RGB24,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
	BGR24 				=AV_PIX_FMT_BGR24,     ///< packed RGB 8:8:8, 24bpp, BGRBGR...
	YUV422P 			=AV_PIX_FMT_YUV422P,   ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
	YUV444P 			=AV_PIX_FMT_YUV444P,   ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
	YUV410P 			=AV_PIX_FMT_YUV410P,   ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
	YUV411P 			=AV_PIX_FMT_YUV411P,   ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
	GRAY8 				=AV_PIX_FMT_GRAY8,     ///<        Y        ,  8bpp
	MONOWHITE 			=AV_PIX_FMT_MONOWHITE, ///<        Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
	MONOBLACK 			=AV_PIX_FMT_MONOBLACK, ///<        Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
	PAL8 				=AV_PIX_FMT_PAL8,      ///< 8 bits with AV_PIX_FMT_RGB32 palette
	YUVJ420P 			=AV_PIX_FMT_YUVJ420P,  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV420P and setting color_range
	YUVJ422P 			=AV_PIX_FMT_YUVJ422P,  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV422P and setting color_range
	YUVJ444P 			=AV_PIX_FMT_YUVJ444P,  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV444P and setting color_range
	UYVY422 			=AV_PIX_FMT_UYVY422,   ///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
	UYYVYY411 			=AV_PIX_FMT_UYYVYY411, ///< packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
	BGR8 				=AV_PIX_FMT_BGR8,      ///< packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
	BGR4 				=AV_PIX_FMT_BGR4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
	BGR4_BYTE 			=AV_PIX_FMT_BGR4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
	RGB8 				=AV_PIX_FMT_RGB8,      ///< packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
	RGB4 				=AV_PIX_FMT_RGB4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
	RGB4_BYTE 			=AV_PIX_FMT_RGB4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
	NV12 				=AV_PIX_FMT_NV12,      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
	NV21 				=AV_PIX_FMT_NV21,      ///< as above, but U and V bytes are swapped

	ARGB 				=AV_PIX_FMT_ARGB,      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
	RGBA 				=AV_PIX_FMT_RGBA,      ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
	ABGR 				=AV_PIX_FMT_ABGR,      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
	BGRA 				=AV_PIX_FMT_BGRA,      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...

	GRAY16BE 			=AV_PIX_FMT_GRAY16BE,  ///<        Y        , 16bpp, big-endian
	GRAY16LE 			=AV_PIX_FMT_GRAY16LE,  ///<        Y        , 16bpp, little-endian
	YUV440P 			=AV_PIX_FMT_YUV440P,   ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
	YUVJ440P 			=AV_PIX_FMT_YUVJ440P,  ///< planar YUV 4:4:0 full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV440P and setting color_range
	YUVA420P 			=AV_PIX_FMT_YUVA420P,  ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
	RGB48BE 			=AV_PIX_FMT_RGB48BE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian
	RGB48LE 			=AV_PIX_FMT_RGB48LE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian

	RGB565BE 			=AV_PIX_FMT_RGB565BE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
	RGB565LE 			=AV_PIX_FMT_RGB565LE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
	RGB555BE 			=AV_PIX_FMT_RGB555BE,  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), big-endian   , X=unused/undefined
	RGB555LE 			=AV_PIX_FMT_RGB555LE,  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), little-endian, X=unused/undefined

	BGR565BE 			=AV_PIX_FMT_BGR565BE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
	BGR565LE 			=AV_PIX_FMT_BGR565LE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
	BGR555BE 			=AV_PIX_FMT_BGR555BE,  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), big-endian   , X=unused/undefined
	BGR555LE 			=AV_PIX_FMT_BGR555LE,  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), little-endian, X=unused/undefined

	#if FF_API_VAAPI
	/** @name Deprecated pixel formats */
	/**@{*/
	VAAPI_MOCO 			=AV_PIX_FMT_VAAPI_MOCO, ///< HW acceleration through VA API at motion compensation entry-point, Picture.data[3] contains a vaapi_render_state struct which contains macroblocks as well as various fields extracted from headers
	VAAPI_IDCT 			=AV_PIX_FMT_VAAPI_IDCT, ///< HW acceleration through VA API at IDCT entry-point, Picture.data[3] contains a vaapi_render_state struct which contains fields extracted from headers
	VAAPI_VLD 			=AV_PIX_FMT_VAAPI_VLD,  ///< HW decoding through VA API, Picture.data[3] contains a VASurfaceID
	/**@}*/
	VAAPI 				= AV_PIX_FMT_VAAPI_VLD,
	#else
	/**
	    *  Hardware acceleration through VA-API, data[3] contains a
	    *  VASurfaceID.
	    */
	VAAPI 	=AV_PIX_FMT_VAAPI,
	#endif

	YUV420P16LE 		=AV_PIX_FMT_YUV420P16LE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	YUV420P16BE 		=AV_PIX_FMT_YUV420P16BE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	YUV422P16LE 		=AV_PIX_FMT_YUV422P16LE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	YUV422P16BE 		=AV_PIX_FMT_YUV422P16BE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	YUV444P16LE 		=AV_PIX_FMT_YUV444P16LE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	YUV444P16BE 		=AV_PIX_FMT_YUV444P16BE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	DXVA2_VLD 			=AV_PIX_FMT_DXVA2_VLD,    ///< HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer

	RGB444LE 			=AV_PIX_FMT_RGB444LE,  ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), little-endian, X=unused/undefined
	RGB444BE 			=AV_PIX_FMT_RGB444BE,  ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), big-endian,    X=unused/undefined
	BGR444LE 			=AV_PIX_FMT_BGR444LE,  ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), little-endian, X=unused/undefined
	BGR444BE 			=AV_PIX_FMT_BGR444BE,  ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), big-endian,    X=unused/undefined
	YA8 				=AV_PIX_FMT_YA8,       ///< 8 bits gray, 8 bits alpha

	Y400A 				=AV_PIX_FMT_Y400A, ///< alias for AV_PIX_FMT_YA8
	GRAY8A				=AV_PIX_FMT_GRAY8A, ///< alias for AV_PIX_FMT_YA8

	BGR48BE 			=AV_PIX_FMT_BGR48BE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
	BGR48LE 			=AV_PIX_FMT_BGR48LE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian

	/**
	    * The following 12 formats have the disadvantage of needing 1 format for each bit depth.
	    * Notice that each 9/10 bits sample is stored in 16 bits with extra padding.
	    * If you want to support multiple bit depths, then using AV_PIX_FMT_YUV420P16* with the bpp stored separately is better.
	    */
	YUV420P9BE 			=AV_PIX_FMT_YUV420P9BE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	YUV420P9LE 			=AV_PIX_FMT_YUV420P9LE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	YUV420P10BE 		=AV_PIX_FMT_YUV420P10BE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	YUV420P10LE 		=AV_PIX_FMT_YUV420P10LE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	YUV422P10BE 		=AV_PIX_FMT_YUV422P10BE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	YUV422P10LE 		=AV_PIX_FMT_YUV422P10LE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	YUV444P9BE 			=AV_PIX_FMT_YUV444P9BE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	YUV444P9LE 			=AV_PIX_FMT_YUV444P9LE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	YUV444P10BE 		=AV_PIX_FMT_YUV444P10BE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	YUV444P10LE 		=AV_PIX_FMT_YUV444P10LE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	YUV422P9BE 			=AV_PIX_FMT_YUV422P9BE, ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	YUV422P9LE 			=AV_PIX_FMT_YUV422P9LE, ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	GBRP 				=AV_PIX_FMT_GBRP,      ///< planar GBR 4:4:4 24bpp
	GBR24P 				=AV_PIX_FMT_GBRP, 	// alias for #AV_PIX_FMT_GBRP
	GBRP9BE 			=AV_PIX_FMT_GBRP9BE,   ///< planar GBR 4:4:4 27bpp, big-endian
	GBRP9LE 			=AV_PIX_FMT_GBRP9LE,   ///< planar GBR 4:4:4 27bpp, little-endian
	GBRP10BE 			=AV_PIX_FMT_GBRP10BE,  ///< planar GBR 4:4:4 30bpp, big-endian
	GBRP10LE 			=AV_PIX_FMT_GBRP10LE,  ///< planar GBR 4:4:4 30bpp, little-endian
	GBRP16BE 			=AV_PIX_FMT_GBRP16BE,  ///< planar GBR 4:4:4 48bpp, big-endian
	GBRP16LE 			=AV_PIX_FMT_GBRP16LE,  ///< planar GBR 4:4:4 48bpp, little-endian
	YUVA422P 			=AV_PIX_FMT_YUVA422P,  ///< planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
	YUVA444P 			=AV_PIX_FMT_YUVA444P,  ///< planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)
	YUVA420P9BE 		=AV_PIX_FMT_YUVA420P9BE,  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian
	YUVA420P9LE 		=AV_PIX_FMT_YUVA420P9LE,  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian
	YUVA422P9BE 		=AV_PIX_FMT_YUVA422P9BE,  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian
	YUVA422P9LE 		=AV_PIX_FMT_YUVA422P9LE,  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian
	YUVA444P9BE 		=AV_PIX_FMT_YUVA444P9BE,  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian
	YUVA444P9LE 		=AV_PIX_FMT_YUVA444P9LE,  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
	YUVA420P10BE 		=AV_PIX_FMT_YUVA420P10BE, ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
	YUVA420P10LE 		=AV_PIX_FMT_YUVA420P10LE, ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
	YUVA422P10BE 		=AV_PIX_FMT_YUVA422P10BE, ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
	YUVA422P10LE 		=AV_PIX_FMT_YUVA422P10LE, ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
	YUVA444P10BE 		=AV_PIX_FMT_YUVA444P10BE, ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
	YUVA444P10LE 		=AV_PIX_FMT_YUVA444P10LE, ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)
	YUVA420P16BE 		=AV_PIX_FMT_YUVA420P16BE, ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
	YUVA420P16LE 		=AV_PIX_FMT_YUVA420P16LE, ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
	YUVA422P16BE 		=AV_PIX_FMT_YUVA422P16BE, ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
	YUVA422P16LE 		=AV_PIX_FMT_YUVA422P16LE, ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
	YUVA444P16BE 		=AV_PIX_FMT_YUVA444P16BE, ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
	YUVA444P16LE 		=AV_PIX_FMT_YUVA444P16LE, ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)

	VDPAU 				=AV_PIX_FMT_VDPAU,     ///< HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface

	XYZ12LE 			=AV_PIX_FMT_XYZ12LE,      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0
	XYZ12BE 			=AV_PIX_FMT_XYZ12BE,      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0
	NV16 				=AV_PIX_FMT_NV16,         ///< interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
	NV20LE 				=AV_PIX_FMT_NV20LE,       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	NV20BE 				=AV_PIX_FMT_NV20BE,       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian

	RGBA64BE 			=AV_PIX_FMT_RGBA64BE,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
	RGBA64LE 			=AV_PIX_FMT_RGBA64LE,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
	BGRA64BE 			=AV_PIX_FMT_BGRA64BE,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
	BGRA64LE 			=AV_PIX_FMT_BGRA64LE,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian

	YVYU422 			=AV_PIX_FMT_YVYU422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb

	YA16BE 				=AV_PIX_FMT_YA16BE,       ///< 16 bits gray, 16 bits alpha (big-endian)
	YA16LE 				=AV_PIX_FMT_YA16LE,       ///< 16 bits gray, 16 bits alpha (little-endian)

	GBRAP 				=AV_PIX_FMT_GBRAP,        ///< planar GBRA 4:4:4:4 32bpp
	GBRAP16BE 			=AV_PIX_FMT_GBRAP16BE,    ///< planar GBRA 4:4:4:4 64bpp, big-endian
	GBRAP16LE 			=AV_PIX_FMT_GBRAP16LE,    ///< planar GBRA 4:4:4:4 64bpp, little-endian
	/**
	    *  HW acceleration through QSV, data[3] contains a pointer to the
	    *  mfxFrameSurface1 structure.
	    */
	QSV 				=AV_PIX_FMT_QSV,
	/**
	    * HW acceleration though MMAL, data[3] contains a pointer to the
	    * MMAL_BUFFER_HEADER_T structure.
	    */
	MMAL 				=AV_PIX_FMT_MMAL,

	D3D11VA_VLD 		=AV_PIX_FMT_D3D11VA_VLD,  ///< HW decoding through Direct3D11 via old API, Picture.data[3] contains a ID3D11VideoDecoderOutputView pointer

	/**
	    * HW acceleration through CUDA. data[i] contain CUdeviceptr pointers
	    * exactly as for system memory frames.
	    */
	CUDA 				=AV_PIX_FMT_CUDA,

	XRGB 				=AV_PIX_FMT_0RGB,        ///< packed RGB 8:8:8, 32bpp, XRGBXRGB...   X=unused/undefined
	RGBX 				=AV_PIX_FMT_RGB0,        ///< packed RGB 8:8:8, 32bpp, RGBXRGBX...   X=unused/undefined
	XBGR 				=AV_PIX_FMT_0BGR,        ///< packed BGR 8:8:8, 32bpp, XBGRXBGR...   X=unused/undefined
	BGRX 				=AV_PIX_FMT_BGR0,        ///< packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined

	YUV420P12BE 		=AV_PIX_FMT_YUV420P12BE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	YUV420P12LE 		=AV_PIX_FMT_YUV420P12LE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	YUV420P14BE 		=AV_PIX_FMT_YUV420P14BE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	YUV420P14LE 		=AV_PIX_FMT_YUV420P14LE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	YUV422P12BE 		=AV_PIX_FMT_YUV422P12BE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	YUV422P12LE 		=AV_PIX_FMT_YUV422P12LE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	YUV422P14BE 		=AV_PIX_FMT_YUV422P14BE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	YUV422P14LE 		=AV_PIX_FMT_YUV422P14LE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	YUV444P12BE 		=AV_PIX_FMT_YUV444P12BE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	YUV444P12LE 		=AV_PIX_FMT_YUV444P12LE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	YUV444P14BE 		=AV_PIX_FMT_YUV444P14BE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	YUV444P14LE 		=AV_PIX_FMT_YUV444P14LE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	GBRP12BE 			=AV_PIX_FMT_GBRP12BE,    ///< planar GBR 4:4:4 36bpp, big-endian
	GBRP12LE 			=AV_PIX_FMT_GBRP12LE,    ///< planar GBR 4:4:4 36bpp, little-endian
	GBRP14BE 			=AV_PIX_FMT_GBRP14BE,    ///< planar GBR 4:4:4 42bpp, big-endian
	GBRP14LE 			=AV_PIX_FMT_GBRP14LE,    ///< planar GBR 4:4:4 42bpp, little-endian
	YUVJ411P 			=AV_PIX_FMT_YUVJ411P,    ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV411P and setting color_range

	BAYER_BGGR8 		=AV_PIX_FMT_BAYER_BGGR8,    ///< bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples */
	BAYER_RGGB8 		=AV_PIX_FMT_BAYER_RGGB8,    ///< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples */
	BAYER_GBRG8 		=AV_PIX_FMT_BAYER_GBRG8,    ///< bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples */
	BAYER_GRBG8 		=AV_PIX_FMT_BAYER_GRBG8,    ///< bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples */
	BAYER_BGGR16LE 		=AV_PIX_FMT_BAYER_BGGR16LE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, little-endian */
	BAYER_BGGR16BE 		=AV_PIX_FMT_BAYER_BGGR16BE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, big-endian */
	BAYER_RGGB16LE 		=AV_PIX_FMT_BAYER_RGGB16LE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, little-endian */
	BAYER_RGGB16BE 		=AV_PIX_FMT_BAYER_RGGB16BE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, big-endian */
	BAYER_GBRG16LE 		=AV_PIX_FMT_BAYER_GBRG16LE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, little-endian */
	BAYER_GBRG16BE 		=AV_PIX_FMT_BAYER_GBRG16BE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, big-endian */
	BAYER_GRBG16LE 		=AV_PIX_FMT_BAYER_GRBG16LE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, little-endian */
	BAYER_GRBG16BE 		=AV_PIX_FMT_BAYER_GRBG16BE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, big-endian */

	XVMC 				=AV_PIX_FMT_XVMC,///< XVideo Motion Acceleration via common packet passing

	YUV440P10LE 		=AV_PIX_FMT_YUV440P10LE, ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
	YUV440P10BE 		=AV_PIX_FMT_YUV440P10BE, ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
	YUV440P12LE 		=AV_PIX_FMT_YUV440P12LE, ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
	YUV440P12BE 		=AV_PIX_FMT_YUV440P12BE, ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
	AYUV64LE 			=AV_PIX_FMT_AYUV64LE,    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
	AYUV64BE 			=AV_PIX_FMT_AYUV64BE,    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), big-endian

	VIDEOTOOLBOX 		=AV_PIX_FMT_VIDEOTOOLBOX, ///< hardware decoding through Videotoolbox

	P010LE 				=AV_PIX_FMT_P010LE, ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, little-endian
	P010BE 				=AV_PIX_FMT_P010BE, ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, big-endian

	GBRAP12BE 			=AV_PIX_FMT_GBRAP12BE,  ///< planar GBR 4:4:4:4 48bpp, big-endian
	GBRAP12LE 			=AV_PIX_FMT_GBRAP12LE,  ///< planar GBR 4:4:4:4 48bpp, little-endian

	GBRAP10BE 			=AV_PIX_FMT_GBRAP10BE,  ///< planar GBR 4:4:4:4 40bpp, big-endian
	GBRAP10LE 			=AV_PIX_FMT_GBRAP10LE,  ///< planar GBR 4:4:4:4 40bpp, little-endian

	MEDIACODEC 			=AV_PIX_FMT_MEDIACODEC, ///< hardware decoding through MediaCodec

	GRAY12BE 			=AV_PIX_FMT_GRAY12BE,   ///<        Y        , 12bpp, big-endian
	GRAY12LE 			=AV_PIX_FMT_GRAY12LE,   ///<        Y        , 12bpp, little-endian
	GRAY10BE 			=AV_PIX_FMT_GRAY10BE,   ///<        Y        , 10bpp, big-endian
	GRAY10LE 			=AV_PIX_FMT_GRAY10LE,   ///<        Y        , 10bpp, little-endian

	P016LE 				=AV_PIX_FMT_P016LE, ///< like NV12, with 16bpp per component, little-endian
	P016BE 				=AV_PIX_FMT_P016BE, ///< like NV12, with 16bpp per component, big-endian

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
	D3D11 				=AV_PIX_FMT_D3D11,

	GRAY9BE 			=AV_PIX_FMT_GRAY9BE,   ///<        Y        , 9bpp, big-endian
	GRAY9LE 			=AV_PIX_FMT_GRAY9LE,   ///<        Y        , 9bpp, little-endian

	GBRPF32BE 			=AV_PIX_FMT_GBRPF32BE,  ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, big-endian
	GBRPF32LE 			=AV_PIX_FMT_GBRPF32LE,  ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, little-endian
	GBRAPF32BE 			=AV_PIX_FMT_GBRAPF32BE, ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, big-endian
	GBRAPF32LE 			=AV_PIX_FMT_GBRAPF32LE, ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, little-endian

	/**
	    * DRM-managed buffers exposed through PRIME buffer sharing.
	    *
	    * data[0] points to an AVDRMFrameDescriptor.
	    */
	DRM_PRIME 			=AV_PIX_FMT_DRM_PRIME,
	/**
	    * Hardware surfaces for OpenCL.
	    *
	    * data[i] contain 2D image objects (typed in C as cl_mem, used
	    * in OpenCL as image2d_t) for each plane of the surface.
	    */
	OPENCL 				=AV_PIX_FMT_OPENCL,

	GRAY14BE 			=AV_PIX_FMT_GRAY14BE,   ///<        Y        , 14bpp, big-endian
	GRAY14LE 			=AV_PIX_FMT_GRAY14LE,   ///<        Y        , 14bpp, little-endian

	GRAYF32BE 			=AV_PIX_FMT_GRAYF32BE,  ///< IEEE-754 single precision Y, 32bpp, big-endian
	GRAYF32LE 			=AV_PIX_FMT_GRAYF32LE,  ///< IEEE-754 single precision Y, 32bpp, little-endian

	RGB32  				=AV_PIX_FMT_RGB32,
	RGB32_1 			=AV_PIX_FMT_RGB32_1,
	BGR32 				=AV_PIX_FMT_BGR32,
	BGR32_1 			=AV_PIX_FMT_BGR32_1,
	XRGB32 				=AV_PIX_FMT_0RGB32,
	XBGR32 				=AV_PIX_FMT_0BGR32,

	GRAY9  				=AV_PIX_FMT_GRAY9,
	GRAY10  			=AV_PIX_FMT_GRAY10,
	GRAY12  			=AV_PIX_FMT_GRAY12,
	GRAY14  			=AV_PIX_FMT_GRAY14,
	GRAY16  			=AV_PIX_FMT_GRAY16,
	YA16    			=AV_PIX_FMT_YA16,
	RGB48   			=AV_PIX_FMT_RGB48,
	RGB565  			=AV_PIX_FMT_RGB565,
	RGB555  			=AV_PIX_FMT_RGB555,
	RGB444  			=AV_PIX_FMT_RGB444,
	RGBA64  			=AV_PIX_FMT_RGBA64,
	BGR48   			=AV_PIX_FMT_BGR48,
	BGR565  			=AV_PIX_FMT_BGR565,
	BGR555  			=AV_PIX_FMT_BGR555,
	BGR444  			=AV_PIX_FMT_BGR444,
	BGRA64  			=AV_PIX_FMT_BGRA64,

	YUV420P9   			=AV_PIX_FMT_YUV420P9,
	YUV422P9   			=AV_PIX_FMT_YUV422P9,
	YUV444P9		   	=AV_PIX_FMT_YUV444P9,
	YUV420P10  			=AV_PIX_FMT_YUV420P10,
	YUV422P10  			=AV_PIX_FMT_YUV422P10,
	YUV440P10 		 	=AV_PIX_FMT_YUV440P10,
	YUV444P10  			=AV_PIX_FMT_YUV444P10,
	YUV420P12 		 	=AV_PIX_FMT_YUV420P12,
	YUV422P12 		 	=AV_PIX_FMT_YUV422P12,
	YUV440P12 		 	=AV_PIX_FMT_YUV440P12,
	YUV444P12 		 	=AV_PIX_FMT_YUV444P12,
	YUV420P14 		 	=AV_PIX_FMT_YUV420P14,
	YUV422P14  			=AV_PIX_FMT_YUV422P14,
	YUV444P14 		 	=AV_PIX_FMT_YUV444P14,
	YUV420P16 		 	=AV_PIX_FMT_YUV420P16,
	YUV422P16 		 	=AV_PIX_FMT_YUV422P16,
	YUV444P16		  	=AV_PIX_FMT_YUV444P16,

	GBRP9     		 	=AV_PIX_FMT_GBRP9,
	GBRP10    		 	=AV_PIX_FMT_GBRP10,
	GBRP12   		  	=AV_PIX_FMT_GBRP12,
	GBRP14    		 	=AV_PIX_FMT_GBRP14,
	GBRP16  		   	=AV_PIX_FMT_GBRP16,
	GBRAP10 		   	=AV_PIX_FMT_GBRAP10,
	GBRAP12 		   	=AV_PIX_FMT_GBRAP12,
	GBRAP16  		  	=AV_PIX_FMT_GBRAP16,

	BAYER_BGGR16  		=AV_PIX_FMT_BAYER_BGGR16,
	BAYER_RGGB16  		=AV_PIX_FMT_BAYER_RGGB16,
	BAYER_GBRG16  		=AV_PIX_FMT_BAYER_GBRG16,
	BAYER_GRBG16  		=AV_PIX_FMT_BAYER_GRBG16,

	GBRPF32     		=AV_PIX_FMT_GBRPF32,
	GBRAPF32    		=AV_PIX_FMT_GBRAPF32,

	GRAYF32     		=AV_PIX_FMT_GRAYF32,

	YUVA420P9   		=AV_PIX_FMT_YUVA420P9,
	YUVA422P9   		=AV_PIX_FMT_YUVA422P9,
	YUVA444P9   		=AV_PIX_FMT_YUVA444P9,
	YUVA420P10  		=AV_PIX_FMT_YUVA420P10,
	YUVA422P10  		=AV_PIX_FMT_YUVA422P10,
	YUVA444P10  		=AV_PIX_FMT_YUVA444P10,
	YUVA420P16  		=AV_PIX_FMT_YUVA420P16,
	YUVA422P16  		=AV_PIX_FMT_YUVA422P16,
	YUVA444P16  		=AV_PIX_FMT_YUVA444P16,

	XYZ12       		=AV_PIX_FMT_XYZ12,
	NV20        		=AV_PIX_FMT_NV20,
	AYUV64      		=AV_PIX_FMT_AYUV64,
	P010        		=AV_PIX_FMT_P010,
	P016        		=AV_PIX_FMT_P016
};

struct PixelFormat{
	typedef AVPixFmtDescriptor PixelDescriptor;

	PixelFormats fmt;

	constexpr PixelFormat(PixelFormats fmt) :
		fmt(fmt)
	{
	}

	constexpr PixelFormat(AVPixelFormat fmt) :
		fmt(static_cast<PixelFormats>(fmt))
	{
	}

	PixelFormat(const std::string& name) :
		PixelFormat( av_get_pix_fmt(name.c_str()) )
	{
	}

	PixelFormat(const PixelFormat& other)=default;
	~PixelFormat()=default;

	constexpr int operator ==(const PixelFormat& other) const{
		return fmt == other.fmt;
	}

	constexpr int operator !=(const PixelFormat& other) const{
		return fmt != other.fmt;
	}

	constexpr AVPixelFormat toAVPixelFormat() const{
		return (AVPixelFormat)fmt;
	}

	const PixelDescriptor* getPixelDescription() const{
		return av_pix_fmt_desc_get(toAVPixelFormat());
	}

	std::string getPixelFormatName() const{
		const PixelDescriptor* desc = getPixelDescription();
		if(desc){
			return std::string(desc->name);
		}else{
			return std::string(); //Should not happen, an invalid pixel format was given
		}
	}

	bool hasAlpha() const{
		const PixelDescriptor* desc = getPixelDescription();
		if(desc){
			return desc->flags & AV_PIX_FMT_FLAG_ALPHA;
		}else{
			return false; //Should not happen, an invalid pixel format was given
		}
	}

	bool isBigEndian() const{
		const PixelDescriptor* desc = getPixelDescription();
		if(desc){
			return desc->flags & AV_PIX_FMT_FLAG_BE;
		}else{
			return false; //Should not happen, an invalid pixel format was given
		}
	}

	int getBitsPerPixel() const{
		return av_get_bits_per_pixel(getPixelDescription());
	}

	int getPlanesCount() const{
		return av_pix_fmt_count_planes(toAVPixelFormat());
	}

	PixelFormat swapEndianness() const{
		return PixelFormat(
				av_pix_fmt_swap_endianness(toAVPixelFormat())
		);
	}
};
}

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
struct PixelFormat{
public:
	constexpr PixelFormat() :
		fmt(AV_PIX_FMT_NONE)
	{
	}

	constexpr PixelFormat(AVPixelFormat fmt) :
		fmt(fmt)
	{
	}

	PixelFormat(const std::string& name) :
		fmt( av_get_pix_fmt(name.c_str()) )
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
		return fmt;
	}

	const AVPixFmtDescriptor* getPixelDescription() const{
		return av_pix_fmt_desc_get(toAVPixelFormat());
	}

	std::string getPixelFormatName() const{
		const AVPixFmtDescriptor* desc = getPixelDescription();
		if(desc){
			return std::string(desc->name);
		}else{
			return std::string(); //Should not happen, an invalid pixel format was given
		}
	}

	bool hasAlpha() const{
		const AVPixFmtDescriptor* desc = getPixelDescription();
		if(desc){
			return desc->flags & AV_PIX_FMT_FLAG_ALPHA;
		}else{
			return false; //Should not happen, an invalid pixel format was given
		}
	}

	bool isBigEndian() const{
		const AVPixFmtDescriptor* desc = getPixelDescription();
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

	static const PixelFormat NONE;
	static const PixelFormat YUV420P;   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
	static const PixelFormat YUYV422;   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
	static const PixelFormat RGB24;     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
	static const PixelFormat BGR24;     ///< packed RGB 8:8:8, 24bpp, BGRBGR...
	static const PixelFormat YUV422P;   ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
	static const PixelFormat YUV444P;   ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
	static const PixelFormat YUV410P;   ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
	static const PixelFormat YUV411P;   ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
	static const PixelFormat GRAY8;     ///<        Y        ,  8bpp
	static const PixelFormat MONOWHITE; ///<        Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
	static const PixelFormat MONOBLACK; ///<        Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
	static const PixelFormat PAL8;      ///< 8 bits with AV_PIX_FMT_RGB32 palette
	static const PixelFormat YUVJ420P;  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV420P and setting color_range
	static const PixelFormat YUVJ422P;  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV422P and setting color_range
	static const PixelFormat YUVJ444P;  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV444P and setting color_range
	static const PixelFormat UYVY422;   ///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
	static const PixelFormat UYYVYY411; ///< packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
	static const PixelFormat BGR8;      ///< packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
	static const PixelFormat BGR4;      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
	static const PixelFormat BGR4_BYTE; ///< packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
	static const PixelFormat RGB8;      ///< packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
	static const PixelFormat RGB4;      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
	static const PixelFormat RGB4_BYTE; ///< packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
	static const PixelFormat NV12;      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
	static const PixelFormat NV21;      ///< as above, but U and V bytes are swapped

	static const PixelFormat ARGB;      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
	static const PixelFormat RGBA;      ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
	static const PixelFormat ABGR;      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
	static const PixelFormat BGRA;      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...

	static const PixelFormat GRAY16BE;  ///<        Y        , 16bpp, big-endian
	static const PixelFormat GRAY16LE;  ///<        Y        , 16bpp, little-endian
	static const PixelFormat YUV440P;   ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
	static const PixelFormat YUVJ440P;  ///< planar YUV 4:4:0 full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV440P and setting color_range
	static const PixelFormat YUVA420P;  ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
	static const PixelFormat RGB48BE;   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian
	static const PixelFormat RGB48LE;   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian

	static const PixelFormat RGB565BE;  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
	static const PixelFormat RGB565LE;  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
	static const PixelFormat RGB555BE;  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), big-endian   , X=unused/undefined
	static const PixelFormat RGB555LE;  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), little-endian, X=unused/undefined

	static const PixelFormat BGR565BE;  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
	static const PixelFormat BGR565LE;  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
	static const PixelFormat BGR555BE;  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), big-endian   , X=unused/undefined
	static const PixelFormat BGR555LE;  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), little-endian, X=unused/undefined

    /**
        *  Hardware acceleration through VA-API, data[3] contains a
        *  VASurfaceID.
        */
	static const PixelFormat VAAPI;

	static const PixelFormat YUV420P16LE;  	///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	static const PixelFormat YUV420P16BE;  	///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	static const PixelFormat YUV422P16LE;  	///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const PixelFormat YUV422P16BE; 	///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	static const PixelFormat YUV444P16LE;  	///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	static const PixelFormat YUV444P16BE;  	///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	static const PixelFormat DXVA2_VLD;    	///< HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer

	static const PixelFormat RGB444LE;  		///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), little-endian, X=unused/undefined
	static const PixelFormat RGB444BE;  		///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), big-endian,    X=unused/undefined
	static const PixelFormat BGR444LE;  		///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), little-endian, X=unused/undefined
	static const PixelFormat BGR444BE;  		///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), big-endian,    X=unused/undefined
	static const PixelFormat YA8;       		///< 8 bits gray, 8 bits alpha

	static const PixelFormat Y400A; 			///< alias for AV_PIX_FMT_YA8
	static const PixelFormat GRAY8A; 		///< alias for AV_PIX_FMT_YA8

	static const PixelFormat BGR48BE;  		///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
	static const PixelFormat BGR48LE;   		///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian

    /**
        * The following 12 formats have the disadvantage of needing 1 format for each bit depth.
        * Notice that each 9/10 bits sample is stored in 16 bits with extra padding.
        * If you want to support multiple bit depths, then using AV_PIX_FMT_YUV420P16* with the bpp stored separately is better.
        */
	static const PixelFormat YUV420P9BE; 	///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	static const PixelFormat YUV420P9LE; 	///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	static const PixelFormat YUV420P10BE;	///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	static const PixelFormat YUV420P10LE;	///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	static const PixelFormat YUV422P10BE;	///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	static const PixelFormat YUV422P10LE;	///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const PixelFormat YUV444P9BE; 	///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	static const PixelFormat YUV444P9LE; 	///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	static const PixelFormat YUV444P10BE;	///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	static const PixelFormat YUV444P10LE;	///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	static const PixelFormat YUV422P9BE; 	///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	static const PixelFormat YUV422P9LE; 	///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const PixelFormat GBRP;      	///< planar GBR 4:4:4 24bpp
	static const PixelFormat GBR24P; 		/// alias for #AV_PIX_FMT_GBRP
	static const PixelFormat GBRP9BE;   	///< planar GBR 4:4:4 27bpp, big-endian
	static const PixelFormat GBRP9LE;   	///< planar GBR 4:4:4 27bpp, little-endian
	static const PixelFormat GBRP10BE; 		///< planar GBR 4:4:4 30bpp, big-endian
	static const PixelFormat GBRP10LE;  	///< planar GBR 4:4:4 30bpp, little-endian
	static const PixelFormat GBRP16BE;  	///< planar GBR 4:4:4 48bpp, big-endian
	static const PixelFormat GBRP16LE;  	///< planar GBR 4:4:4 48bpp, little-endian
	static const PixelFormat YUVA422P;  	///< planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
	static const PixelFormat YUVA444P;  	///< planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)
	static const PixelFormat YUVA420P9BE;  	///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian
	static const PixelFormat YUVA420P9LE;  	///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian
	static const PixelFormat YUVA422P9BE;  	///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian
	static const PixelFormat YUVA422P9LE;  	///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian
	static const PixelFormat YUVA444P9BE;  	///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian
	static const PixelFormat YUVA444P9LE;  	///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
	static const PixelFormat YUVA420P10BE;	///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
	static const PixelFormat YUVA420P10LE;	///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
	static const PixelFormat YUVA422P10BE; 	///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
	static const PixelFormat YUVA422P10LE; 	///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
	static const PixelFormat YUVA444P10BE; 	///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
	static const PixelFormat YUVA444P10LE;	///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)
	static const PixelFormat YUVA420P16BE; 	///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
	static const PixelFormat YUVA420P16LE; 	///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
	static const PixelFormat YUVA422P16BE; 	///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
	static const PixelFormat YUVA422P16LE; 	///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
	static const PixelFormat YUVA444P16BE; 	///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
	static const PixelFormat YUVA444P16LE; 	///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)

	static const PixelFormat VDPAU;    		///< HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface

	static const PixelFormat XYZ12LE;      	///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0
	static const PixelFormat XYZ12BE;     	///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0
	static const PixelFormat NV16;         	///< interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
	static const PixelFormat NV20LE;       	///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const PixelFormat NV20BE;       	///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian

	static const PixelFormat RGBA64BE;     	///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
	static const PixelFormat RGBA64LE;     	///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
	static const PixelFormat BGRA64BE;     	///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
	static const PixelFormat BGRA64LE;     	///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian

	static const PixelFormat YVYU422;   		///< packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb

	static const PixelFormat YA16BE;       	///< 16 bits gray, 16 bits alpha (big-endian)
	static const PixelFormat YA16LE;       	///< 16 bits gray, 16 bits alpha (little-endian)

	static const PixelFormat GBRAP;        	///< planar GBRA 4:4:4:4 32bpp
	static const PixelFormat GBRAP16BE;   	///< planar GBRA 4:4:4:4 64bpp, big-endian
	static const PixelFormat GBRAP16LE;    	///< planar GBRA 4:4:4:4 64bpp, little-endian
    /**
        *  HW acceleration through QSV, data[3] contains a pointer to the
        *  mfxFrameSurface1 structure.
        */
	static const PixelFormat QSV;
    /**
        * HW acceleration though MMAL, data[3] contains a pointer to the
        * MMAL_BUFFER_HEADER_T structure.
        */
	static const PixelFormat MMAL;

	static const PixelFormat D3D11VA_VLD;  	///< HW decoding through Direct3D11 via old API, Picture.data[3] contains a ID3D11VideoDecoderOutputView pointer

    /**
        * HW acceleration through CUDA. data[i] contain CUdeviceptr pointers
        * exactly as for system memory frames.
        */
	static const PixelFormat CUDA;

	static const PixelFormat XRGB;			///< packed RGB 8:8:8, 32bpp, XRGBXRGB...   X=unused/undefined
	static const PixelFormat RGBX;       	///< packed RGB 8:8:8, 32bpp, RGBXRGBX...   X=unused/undefined
	static const PixelFormat XBGR;        	///< packed BGR 8:8:8, 32bpp, XBGRXBGR...   X=unused/undefined
	static const PixelFormat BGRX; 			///< packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined

	static const PixelFormat YUV420P12BE; 	///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	static const PixelFormat YUV420P12LE; 	///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	static const PixelFormat YUV420P14BE; 	///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
	static const PixelFormat YUV420P14LE; 	///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
	static const PixelFormat YUV422P12BE; ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	static const PixelFormat YUV422P12LE; ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const PixelFormat YUV422P14BE; ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
	static const PixelFormat YUV422P14LE; ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
	static const PixelFormat YUV444P12BE; ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	static const PixelFormat YUV444P12LE; ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	static const PixelFormat YUV444P14BE; ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
	static const PixelFormat YUV444P14LE; ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
	static const PixelFormat GBRP12BE; ///< planar GBR 4:4:4 36bpp, big-endian
	static const PixelFormat GBRP12LE; ///< planar GBR 4:4:4 36bpp, little-endian
	static const PixelFormat GBRP14BE; ///< planar GBR 4:4:4 42bpp, big-endian
	static const PixelFormat GBRP14LE; ///< planar GBR 4:4:4 42bpp, little-endian
	static const PixelFormat YUVJ411P; ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV411P and setting color_range

	static const PixelFormat BAYER_BGGR8; ///< bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples */
	static const PixelFormat BAYER_RGGB8; ///< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples */
	static const PixelFormat BAYER_GBRG8; ///< bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples */
	static const PixelFormat BAYER_GRBG8; ///< bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples */
	static const PixelFormat BAYER_BGGR16LE; ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, little-endian */
	static const PixelFormat BAYER_BGGR16BE; ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, big-endian */
	static const PixelFormat BAYER_RGGB16LE; ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, little-endian */
	static const PixelFormat BAYER_RGGB16BE; ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, big-endian */
	static const PixelFormat BAYER_GBRG16LE; ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, little-endian */
	static const PixelFormat BAYER_GBRG16BE; ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, big-endian */
	static const PixelFormat BAYER_GRBG16LE; ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, little-endian */
	static const PixelFormat BAYER_GRBG16BE; ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, big-endian */

	static const PixelFormat XVMC; ///< XVideo Motion Acceleration via common packet passing

	static const PixelFormat YUV440P10LE; ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
	static const PixelFormat YUV440P10BE; ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
	static const PixelFormat YUV440P12LE; ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
	static const PixelFormat YUV440P12BE; ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
	static const PixelFormat AYUV64LE; ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
	static const PixelFormat AYUV64BE; ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), big-endian

	static const PixelFormat VIDEOTOOLBOX; ///< hardware decoding through Videotoolbox

	static const PixelFormat P010LE; ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, little-endian
	static const PixelFormat P010BE; ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, big-endian

	static const PixelFormat GBRAP12BE; ///< planar GBR 4:4:4:4 48bpp, big-endian
	static const PixelFormat GBRAP12LE; ///< planar GBR 4:4:4:4 48bpp, little-endian

	static const PixelFormat GBRAP10BE; ///< planar GBR 4:4:4:4 40bpp, big-endian
	static const PixelFormat GBRAP10LE; ///< planar GBR 4:4:4:4 40bpp, little-endian

	static const PixelFormat MEDIACODEC; ///< hardware decoding through MediaCodec

	static const PixelFormat GRAY12BE; ///<        Y        , 12bpp, big-endian
	static const PixelFormat GRAY12LE; ///<        Y        , 12bpp, little-endian
	static const PixelFormat GRAY10BE; ///<        Y        , 10bpp, big-endian
	static const PixelFormat GRAY10LE; ///<        Y        , 10bpp, little-endian

	static const PixelFormat P016LE; ///< like NV12, with 16bpp per component, little-endian
	static const PixelFormat P016BE; ///< like NV12, with 16bpp per component, big-endian

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
	static const PixelFormat D3D11;

	static const PixelFormat GRAY9BE; ///<        Y        , 9bpp, big-endian
	static const PixelFormat GRAY9LE; ///<        Y        , 9bpp, little-endian

	static const PixelFormat GBRPF32BE; ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, big-endian
	static const PixelFormat GBRPF32LE; ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, little-endian
	static const PixelFormat GBRAPF32BE; ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, big-endian
	static const PixelFormat GBRAPF32LE; ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, little-endian

    /**
        * DRM-managed buffers exposed through PRIME buffer sharing.
        *
        * data[0] points to an AVDRMFrameDescriptor.
        */
	static const PixelFormat DRM_PRIME;
    /**
        * Hardware surfaces for OpenCL.
        *
        * data[i] contain 2D image objects (typed in C as cl_mem, used
        * in OpenCL as image2d_t) for each plane of the surface.
        */
	static const PixelFormat OPENCL;

	static const PixelFormat GRAY14BE; ///<        Y        , 14bpp, big-endian
	static const PixelFormat GRAY14LE; ///<        Y        , 14bpp, little-endian

	static const PixelFormat GRAYF32BE; ///< IEEE-754 single precision Y, 32bpp, big-endian
	static const PixelFormat GRAYF32LE; ///< IEEE-754 single precision Y, 32bpp, little-endian

	static const PixelFormat RGB32;
	static const PixelFormat RGB32_1;
	static const PixelFormat BGR32;
	static const PixelFormat BGR32_1;
	static const PixelFormat XRGB32;
	static const PixelFormat XBGR32;

	static const PixelFormat GRAY9;
	static const PixelFormat GRAY10;
	static const PixelFormat GRAY12;
	static const PixelFormat GRAY14;
	static const PixelFormat GRAY16;
	static const PixelFormat YA16;
	static const PixelFormat RGB48;
	static const PixelFormat RGB565;
	static const PixelFormat RGB555;
	static const PixelFormat RGB444;
	static const PixelFormat RGBA64;
	static const PixelFormat BGR48;
	static const PixelFormat BGR565;
	static const PixelFormat BGR555;
	static const PixelFormat BGR444;
	static const PixelFormat BGRA64;

	static const PixelFormat YUV420P9;
	static const PixelFormat YUV422P9;
	static const PixelFormat YUV444P9;
	static const PixelFormat YUV420P10;
	static const PixelFormat YUV422P10;
	static const PixelFormat YUV440P10;
	static const PixelFormat YUV444P10;
	static const PixelFormat YUV420P12;
	static const PixelFormat YUV422P12;
	static const PixelFormat YUV440P12;
	static const PixelFormat YUV444P12;
	static const PixelFormat YUV420P14;
	static const PixelFormat YUV422P14;
	static const PixelFormat YUV444P14;
	static const PixelFormat YUV420P16;
	static const PixelFormat YUV422P16;
	static const PixelFormat YUV444P16;

	static const PixelFormat GBRP9;
	static const PixelFormat GBRP10;
	static const PixelFormat GBRP12;
	static const PixelFormat GBRP14;
	static const PixelFormat GBRP16;
	static const PixelFormat GBRAP10;
	static const PixelFormat GBRAP12;
	static const PixelFormat GBRAP16;

	static const PixelFormat BAYER_BGGR16;
	static const PixelFormat BAYER_RGGB16;
	static const PixelFormat BAYER_GBRG16;
	static const PixelFormat BAYER_GRBG16;

	static const PixelFormat GBRPF32;
	static const PixelFormat GBRAPF32;

	static const PixelFormat GRAYF32;

	static const PixelFormat YUVA420P9;
	static const PixelFormat YUVA422P9;
	static const PixelFormat YUVA444P9;
	static const PixelFormat YUVA420P10;
	static const PixelFormat YUVA422P10;
	static const PixelFormat YUVA444P10;
	static const PixelFormat YUVA420P16;
	static const PixelFormat YUVA422P16;
	static const PixelFormat YUVA444P16;

	static const PixelFormat XYZ12;
	static const PixelFormat NV20;
	static const PixelFormat AYUV64;
	static const PixelFormat P010;
	static const PixelFormat P016;
private:
	AVPixelFormat fmt;
};

constexpr const PixelFormat PixelFormat::NONE 						=AV_PIX_FMT_NONE;
constexpr const PixelFormat PixelFormat::YUV420P 					=AV_PIX_FMT_YUV420P;
constexpr const PixelFormat PixelFormat::YUYV422 					=AV_PIX_FMT_YUYV422;
constexpr const PixelFormat PixelFormat::RGB24 						=AV_PIX_FMT_RGB24;
constexpr const PixelFormat PixelFormat::BGR24 						=AV_PIX_FMT_BGR24;
constexpr const PixelFormat PixelFormat::YUV422P 					=AV_PIX_FMT_YUV422P;
constexpr const PixelFormat PixelFormat::YUV444P 					=AV_PIX_FMT_YUV444P;
constexpr const PixelFormat PixelFormat::YUV410P 					=AV_PIX_FMT_YUV410P;
constexpr const PixelFormat PixelFormat::YUV411P 					=AV_PIX_FMT_YUV411P;
constexpr const PixelFormat PixelFormat::GRAY8 						=AV_PIX_FMT_GRAY8;
constexpr const PixelFormat PixelFormat::MONOWHITE 					=AV_PIX_FMT_MONOWHITE;
constexpr const PixelFormat PixelFormat::MONOBLACK 					=AV_PIX_FMT_MONOBLACK;
constexpr const PixelFormat PixelFormat::PAL8 						=AV_PIX_FMT_PAL8;
constexpr const PixelFormat PixelFormat::YUVJ420P 					=AV_PIX_FMT_YUVJ420P;
constexpr const PixelFormat PixelFormat::YUVJ422P 					=AV_PIX_FMT_YUVJ422P;
constexpr const PixelFormat PixelFormat::YUVJ444P 					=AV_PIX_FMT_YUVJ444P;
constexpr const PixelFormat PixelFormat::UYVY422 					=AV_PIX_FMT_UYVY422;
constexpr const PixelFormat PixelFormat::UYYVYY411 					=AV_PIX_FMT_UYYVYY411;
constexpr const PixelFormat PixelFormat::BGR8 						=AV_PIX_FMT_BGR8;
constexpr const PixelFormat PixelFormat::BGR4 						=AV_PIX_FMT_BGR4;
constexpr const PixelFormat PixelFormat::BGR4_BYTE 					=AV_PIX_FMT_BGR4_BYTE;
constexpr const PixelFormat PixelFormat::RGB8 						=AV_PIX_FMT_RGB8;
constexpr const PixelFormat PixelFormat::RGB4 						=AV_PIX_FMT_RGB4;
constexpr const PixelFormat PixelFormat::RGB4_BYTE 					=AV_PIX_FMT_RGB4_BYTE;
constexpr const PixelFormat PixelFormat::NV12 						=AV_PIX_FMT_NV12;
constexpr const PixelFormat PixelFormat::NV21 						=AV_PIX_FMT_NV21;
constexpr const PixelFormat PixelFormat::ARGB 						=AV_PIX_FMT_ARGB;
constexpr const PixelFormat PixelFormat::RGBA 						=AV_PIX_FMT_RGBA;
constexpr const PixelFormat PixelFormat::ABGR 						=AV_PIX_FMT_ABGR;
constexpr const PixelFormat PixelFormat::BGRA 						=AV_PIX_FMT_BGRA;
constexpr const PixelFormat PixelFormat::GRAY16BE 					=AV_PIX_FMT_GRAY16BE;
constexpr const PixelFormat PixelFormat::GRAY16LE 					=AV_PIX_FMT_GRAY16LE;
constexpr const PixelFormat PixelFormat::YUV440P 					=AV_PIX_FMT_YUV440P;
constexpr const PixelFormat PixelFormat::YUVJ440P 					=AV_PIX_FMT_YUVJ440P;
constexpr const PixelFormat PixelFormat::YUVA420P 					=AV_PIX_FMT_YUVA420P;
constexpr const PixelFormat PixelFormat::RGB48BE 					=AV_PIX_FMT_RGB48BE;
constexpr const PixelFormat PixelFormat::RGB48LE 					=AV_PIX_FMT_RGB48LE;
constexpr const PixelFormat PixelFormat::RGB565BE 					=AV_PIX_FMT_RGB565BE;
constexpr const PixelFormat PixelFormat::RGB565LE 					=AV_PIX_FMT_RGB565LE;
constexpr const PixelFormat PixelFormat::RGB555BE 					=AV_PIX_FMT_RGB555BE;
constexpr const PixelFormat PixelFormat::RGB555LE 					=AV_PIX_FMT_RGB555LE;
constexpr const PixelFormat PixelFormat::BGR565BE 					=AV_PIX_FMT_BGR565BE;
constexpr const PixelFormat PixelFormat::BGR565LE 					=AV_PIX_FMT_BGR565LE;
constexpr const PixelFormat PixelFormat::BGR555BE 					=AV_PIX_FMT_BGR555BE;
constexpr const PixelFormat PixelFormat::BGR555LE 					=AV_PIX_FMT_BGR555LE;
constexpr const PixelFormat PixelFormat::VAAPI 	       				=AV_PIX_FMT_VAAPI;
constexpr const PixelFormat PixelFormat::YUV420P16LE 				=AV_PIX_FMT_YUV420P16LE;
constexpr const PixelFormat PixelFormat::YUV420P16BE 				=AV_PIX_FMT_YUV420P16BE;
constexpr const PixelFormat PixelFormat::YUV422P16LE 				=AV_PIX_FMT_YUV422P16LE;
constexpr const PixelFormat PixelFormat::YUV422P16BE 				=AV_PIX_FMT_YUV422P16BE;
constexpr const PixelFormat PixelFormat::YUV444P16LE 				=AV_PIX_FMT_YUV444P16LE;
constexpr const PixelFormat PixelFormat::YUV444P16BE 				=AV_PIX_FMT_YUV444P16BE;
constexpr const PixelFormat PixelFormat::DXVA2_VLD 					=AV_PIX_FMT_DXVA2_VLD;
constexpr const PixelFormat PixelFormat::RGB444LE 					=AV_PIX_FMT_RGB444LE;
constexpr const PixelFormat PixelFormat::RGB444BE 					=AV_PIX_FMT_RGB444BE;
constexpr const PixelFormat PixelFormat::BGR444LE 					=AV_PIX_FMT_BGR444LE;
constexpr const PixelFormat PixelFormat::BGR444BE 					=AV_PIX_FMT_BGR444BE;
constexpr const PixelFormat PixelFormat::YA8 						=AV_PIX_FMT_YA8;
constexpr const PixelFormat PixelFormat::Y400A 						=AV_PIX_FMT_Y400A;
constexpr const PixelFormat PixelFormat::GRAY8A						=AV_PIX_FMT_GRAY8A;
constexpr const PixelFormat PixelFormat::BGR48BE 					=AV_PIX_FMT_BGR48BE;
constexpr const PixelFormat PixelFormat::BGR48LE 					=AV_PIX_FMT_BGR48LE;
constexpr const PixelFormat PixelFormat::YUV420P9BE 				=AV_PIX_FMT_YUV420P9BE;
constexpr const PixelFormat PixelFormat::YUV420P9LE 				=AV_PIX_FMT_YUV420P9LE;
constexpr const PixelFormat PixelFormat::YUV420P10BE 				=AV_PIX_FMT_YUV420P10BE;
constexpr const PixelFormat PixelFormat::YUV420P10LE 				=AV_PIX_FMT_YUV420P10LE;
constexpr const PixelFormat PixelFormat::YUV422P10BE 				=AV_PIX_FMT_YUV422P10BE;
constexpr const PixelFormat PixelFormat::YUV422P10LE 				=AV_PIX_FMT_YUV422P10LE;
constexpr const PixelFormat PixelFormat::YUV444P9BE 				=AV_PIX_FMT_YUV444P9BE;
constexpr const PixelFormat PixelFormat::YUV444P9LE 				=AV_PIX_FMT_YUV444P9LE;
constexpr const PixelFormat PixelFormat::YUV444P10BE 				=AV_PIX_FMT_YUV444P10BE;
constexpr const PixelFormat PixelFormat::YUV444P10LE 				=AV_PIX_FMT_YUV444P10LE;
constexpr const PixelFormat PixelFormat::YUV422P9BE 				=AV_PIX_FMT_YUV422P9BE;
constexpr const PixelFormat PixelFormat::YUV422P9LE 				=AV_PIX_FMT_YUV422P9LE;
constexpr const PixelFormat PixelFormat::GBRP 						=AV_PIX_FMT_GBRP;
constexpr const PixelFormat PixelFormat::GBR24P 					=AV_PIX_FMT_GBRP;
constexpr const PixelFormat PixelFormat::GBRP9BE 					=AV_PIX_FMT_GBRP9BE;
constexpr const PixelFormat PixelFormat::GBRP9LE 					=AV_PIX_FMT_GBRP9LE;
constexpr const PixelFormat PixelFormat::GBRP10BE 					=AV_PIX_FMT_GBRP10BE;
constexpr const PixelFormat PixelFormat::GBRP10LE 					=AV_PIX_FMT_GBRP10LE;
constexpr const PixelFormat PixelFormat::GBRP16BE 					=AV_PIX_FMT_GBRP16BE;
constexpr const PixelFormat PixelFormat::GBRP16LE 					=AV_PIX_FMT_GBRP16LE;
constexpr const PixelFormat PixelFormat::YUVA422P 					=AV_PIX_FMT_YUVA422P;
constexpr const PixelFormat PixelFormat::YUVA444P 					=AV_PIX_FMT_YUVA444P;
constexpr const PixelFormat PixelFormat::YUVA420P9BE 				=AV_PIX_FMT_YUVA420P9BE;
constexpr const PixelFormat PixelFormat::YUVA420P9LE 				=AV_PIX_FMT_YUVA420P9LE;
constexpr const PixelFormat PixelFormat::YUVA422P9BE 				=AV_PIX_FMT_YUVA422P9BE;
constexpr const PixelFormat PixelFormat::YUVA422P9LE 				=AV_PIX_FMT_YUVA422P9LE;
constexpr const PixelFormat PixelFormat::YUVA444P9BE 				=AV_PIX_FMT_YUVA444P9BE;
constexpr const PixelFormat PixelFormat::YUVA444P9LE 				=AV_PIX_FMT_YUVA444P9LE;
constexpr const PixelFormat PixelFormat::YUVA420P10BE 				=AV_PIX_FMT_YUVA420P10BE;
constexpr const PixelFormat PixelFormat::YUVA420P10LE 				=AV_PIX_FMT_YUVA420P10LE;
constexpr const PixelFormat PixelFormat::YUVA422P10BE 				=AV_PIX_FMT_YUVA422P10BE;
constexpr const PixelFormat PixelFormat::YUVA422P10LE 				=AV_PIX_FMT_YUVA422P10LE;
constexpr const PixelFormat PixelFormat::YUVA444P10BE 				=AV_PIX_FMT_YUVA444P10BE;
constexpr const PixelFormat PixelFormat::YUVA444P10LE 				=AV_PIX_FMT_YUVA444P10LE;
constexpr const PixelFormat PixelFormat::YUVA420P16BE 				=AV_PIX_FMT_YUVA420P16BE;
constexpr const PixelFormat PixelFormat::YUVA420P16LE 				=AV_PIX_FMT_YUVA420P16LE;
constexpr const PixelFormat PixelFormat::YUVA422P16BE				=AV_PIX_FMT_YUVA422P16BE;
constexpr const PixelFormat PixelFormat::YUVA422P16LE 				=AV_PIX_FMT_YUVA422P16LE;
constexpr const PixelFormat PixelFormat::YUVA444P16BE 				=AV_PIX_FMT_YUVA444P16BE;
constexpr const PixelFormat PixelFormat::YUVA444P16LE 				=AV_PIX_FMT_YUVA444P16LE;
constexpr const PixelFormat PixelFormat::VDPAU 						=AV_PIX_FMT_VDPAU;
constexpr const PixelFormat PixelFormat::XYZ12LE 					=AV_PIX_FMT_XYZ12LE;
constexpr const PixelFormat PixelFormat::XYZ12BE 					=AV_PIX_FMT_XYZ12BE;
constexpr const PixelFormat PixelFormat::NV16 						=AV_PIX_FMT_NV16;
constexpr const PixelFormat PixelFormat::NV20LE 					=AV_PIX_FMT_NV20LE;
constexpr const PixelFormat PixelFormat::NV20BE 					=AV_PIX_FMT_NV20BE;
constexpr const PixelFormat PixelFormat::RGBA64BE 					=AV_PIX_FMT_RGBA64BE;
constexpr const PixelFormat PixelFormat::RGBA64LE 					=AV_PIX_FMT_RGBA64LE;
constexpr const PixelFormat PixelFormat::BGRA64BE 					=AV_PIX_FMT_BGRA64BE;
constexpr const PixelFormat PixelFormat::BGRA64LE 					=AV_PIX_FMT_BGRA64LE;
constexpr const PixelFormat PixelFormat::YVYU422 					=AV_PIX_FMT_YVYU422;
constexpr const PixelFormat PixelFormat::YA16BE 					=AV_PIX_FMT_YA16BE;
constexpr const PixelFormat PixelFormat::YA16LE 					=AV_PIX_FMT_YA16LE;
constexpr const PixelFormat PixelFormat::GBRAP 						=AV_PIX_FMT_GBRAP;
constexpr const PixelFormat PixelFormat::GBRAP16BE 					=AV_PIX_FMT_GBRAP16BE;
constexpr const PixelFormat PixelFormat::GBRAP16LE 					=AV_PIX_FMT_GBRAP16LE;
constexpr const PixelFormat PixelFormat::QSV 						=AV_PIX_FMT_QSV;
constexpr const PixelFormat PixelFormat::MMAL 						=AV_PIX_FMT_MMAL;
constexpr const PixelFormat PixelFormat::D3D11VA_VLD 				=AV_PIX_FMT_D3D11VA_VLD;
constexpr const PixelFormat PixelFormat::CUDA 						=AV_PIX_FMT_CUDA;
constexpr const PixelFormat PixelFormat::XRGB 						=AV_PIX_FMT_0RGB;
constexpr const PixelFormat PixelFormat::RGBX 						=AV_PIX_FMT_RGB0;
constexpr const PixelFormat PixelFormat::XBGR 						=AV_PIX_FMT_0BGR;
constexpr const PixelFormat PixelFormat::BGRX 						=AV_PIX_FMT_BGR0;
constexpr const PixelFormat PixelFormat::YUV420P12BE 				=AV_PIX_FMT_YUV420P12BE;
constexpr const PixelFormat PixelFormat::YUV420P12LE 				=AV_PIX_FMT_YUV420P12LE;
constexpr const PixelFormat PixelFormat::YUV420P14BE 				=AV_PIX_FMT_YUV420P14BE;
constexpr const PixelFormat PixelFormat::YUV420P14LE 				=AV_PIX_FMT_YUV420P14LE;
constexpr const PixelFormat PixelFormat::YUV422P12BE 				=AV_PIX_FMT_YUV422P12BE;
constexpr const PixelFormat PixelFormat::YUV422P12LE 				=AV_PIX_FMT_YUV422P12LE;
constexpr const PixelFormat PixelFormat::YUV422P14BE 				=AV_PIX_FMT_YUV422P14BE;
constexpr const PixelFormat PixelFormat::YUV422P14LE 				=AV_PIX_FMT_YUV422P14LE;
constexpr const PixelFormat PixelFormat::YUV444P12BE 				=AV_PIX_FMT_YUV444P12BE;
constexpr const PixelFormat PixelFormat::YUV444P12LE 				=AV_PIX_FMT_YUV444P12LE;
constexpr const PixelFormat PixelFormat::YUV444P14BE 				=AV_PIX_FMT_YUV444P14BE;
constexpr const PixelFormat PixelFormat::YUV444P14LE 				=AV_PIX_FMT_YUV444P14LE;
constexpr const PixelFormat PixelFormat::GBRP12BE 					=AV_PIX_FMT_GBRP12BE;
constexpr const PixelFormat PixelFormat::GBRP12LE 					=AV_PIX_FMT_GBRP12LE;
constexpr const PixelFormat PixelFormat::GBRP14BE 					=AV_PIX_FMT_GBRP14BE;
constexpr const PixelFormat PixelFormat::GBRP14LE 					=AV_PIX_FMT_GBRP14LE;
constexpr const PixelFormat PixelFormat::YUVJ411P 					=AV_PIX_FMT_YUVJ411P;
constexpr const PixelFormat PixelFormat::BAYER_BGGR8 				=AV_PIX_FMT_BAYER_BGGR8;
constexpr const PixelFormat PixelFormat::BAYER_RGGB8 				=AV_PIX_FMT_BAYER_RGGB8;
constexpr const PixelFormat PixelFormat::BAYER_GBRG8 				=AV_PIX_FMT_BAYER_GBRG8;
constexpr const PixelFormat PixelFormat::BAYER_GRBG8 				=AV_PIX_FMT_BAYER_GRBG8;
constexpr const PixelFormat PixelFormat::BAYER_BGGR16LE 			=AV_PIX_FMT_BAYER_BGGR16LE;
constexpr const PixelFormat PixelFormat::BAYER_BGGR16BE 			=AV_PIX_FMT_BAYER_BGGR16BE;
constexpr const PixelFormat PixelFormat::BAYER_RGGB16LE 			=AV_PIX_FMT_BAYER_RGGB16LE;
constexpr const PixelFormat PixelFormat::BAYER_RGGB16BE 			=AV_PIX_FMT_BAYER_RGGB16BE;
constexpr const PixelFormat PixelFormat::BAYER_GBRG16LE 			=AV_PIX_FMT_BAYER_GBRG16LE;
constexpr const PixelFormat PixelFormat::BAYER_GBRG16BE 			=AV_PIX_FMT_BAYER_GBRG16BE;
constexpr const PixelFormat PixelFormat::BAYER_GRBG16LE 			=AV_PIX_FMT_BAYER_GRBG16LE;
constexpr const PixelFormat PixelFormat::BAYER_GRBG16BE 			=AV_PIX_FMT_BAYER_GRBG16BE;
constexpr const PixelFormat PixelFormat::XVMC 						=AV_PIX_FMT_XVMC;
constexpr const PixelFormat PixelFormat::YUV440P10LE 				=AV_PIX_FMT_YUV440P10LE;
constexpr const PixelFormat PixelFormat::YUV440P10BE 				=AV_PIX_FMT_YUV440P10BE;
constexpr const PixelFormat PixelFormat::YUV440P12LE 				=AV_PIX_FMT_YUV440P12LE;
constexpr const PixelFormat PixelFormat::YUV440P12BE 				=AV_PIX_FMT_YUV440P12BE;
constexpr const PixelFormat PixelFormat::AYUV64LE 					=AV_PIX_FMT_AYUV64LE;
constexpr const PixelFormat PixelFormat::AYUV64BE 					=AV_PIX_FMT_AYUV64BE;
constexpr const PixelFormat PixelFormat::VIDEOTOOLBOX 				=AV_PIX_FMT_VIDEOTOOLBOX;
constexpr const PixelFormat PixelFormat::P010LE 					=AV_PIX_FMT_P010LE;
constexpr const PixelFormat PixelFormat::P010BE 					=AV_PIX_FMT_P010BE;
constexpr const PixelFormat PixelFormat::GBRAP12BE 					=AV_PIX_FMT_GBRAP12BE;
constexpr const PixelFormat PixelFormat::GBRAP12LE 					=AV_PIX_FMT_GBRAP12LE;
constexpr const PixelFormat PixelFormat::GBRAP10BE 					=AV_PIX_FMT_GBRAP10BE;
constexpr const PixelFormat PixelFormat::GBRAP10LE 					=AV_PIX_FMT_GBRAP10LE;
constexpr const PixelFormat PixelFormat::MEDIACODEC 				=AV_PIX_FMT_MEDIACODEC;
constexpr const PixelFormat PixelFormat::GRAY12BE 					=AV_PIX_FMT_GRAY12BE;
constexpr const PixelFormat PixelFormat::GRAY12LE 					=AV_PIX_FMT_GRAY12LE;
constexpr const PixelFormat PixelFormat::GRAY10BE 					=AV_PIX_FMT_GRAY10BE;
constexpr const PixelFormat PixelFormat::GRAY10LE 					=AV_PIX_FMT_GRAY10LE;
constexpr const PixelFormat PixelFormat::P016LE 					=AV_PIX_FMT_P016LE;
constexpr const PixelFormat PixelFormat::P016BE 					=AV_PIX_FMT_P016BE;
constexpr const PixelFormat PixelFormat::D3D11 						=AV_PIX_FMT_D3D11;
constexpr const PixelFormat PixelFormat::GRAY9BE 					=AV_PIX_FMT_GRAY9BE;
constexpr const PixelFormat PixelFormat::GRAY9LE 					=AV_PIX_FMT_GRAY9LE;
constexpr const PixelFormat PixelFormat::GBRPF32BE 					=AV_PIX_FMT_GBRPF32BE;
constexpr const PixelFormat PixelFormat::GBRPF32LE 					=AV_PIX_FMT_GBRPF32LE;
constexpr const PixelFormat PixelFormat::GBRAPF32BE 				=AV_PIX_FMT_GBRAPF32BE;
constexpr const PixelFormat PixelFormat::GBRAPF32LE 				=AV_PIX_FMT_GBRAPF32LE;
constexpr const PixelFormat PixelFormat::DRM_PRIME 					=AV_PIX_FMT_DRM_PRIME;
constexpr const PixelFormat PixelFormat::OPENCL 					=AV_PIX_FMT_OPENCL;
constexpr const PixelFormat PixelFormat::GRAY14BE 					=AV_PIX_FMT_GRAY14BE;
constexpr const PixelFormat PixelFormat::GRAY14LE 					=AV_PIX_FMT_GRAY14LE;
constexpr const PixelFormat PixelFormat::GRAYF32BE 					=AV_PIX_FMT_GRAYF32BE;
constexpr const PixelFormat PixelFormat::GRAYF32LE 					=AV_PIX_FMT_GRAYF32LE;
constexpr const PixelFormat PixelFormat::RGB32  					=AV_PIX_FMT_RGB32;
constexpr const PixelFormat PixelFormat::RGB32_1 					=AV_PIX_FMT_RGB32_1;
constexpr const PixelFormat PixelFormat::BGR32 						=AV_PIX_FMT_BGR32;
constexpr const PixelFormat PixelFormat::BGR32_1 					=AV_PIX_FMT_BGR32_1;
constexpr const PixelFormat PixelFormat::XRGB32 					=AV_PIX_FMT_0RGB32;
constexpr const PixelFormat PixelFormat::XBGR32 					=AV_PIX_FMT_0BGR32;
constexpr const PixelFormat PixelFormat::GRAY9  					=AV_PIX_FMT_GRAY9;
constexpr const PixelFormat PixelFormat::GRAY10  					=AV_PIX_FMT_GRAY10;
constexpr const PixelFormat PixelFormat::GRAY12  					=AV_PIX_FMT_GRAY12;
constexpr const PixelFormat PixelFormat::GRAY14  					=AV_PIX_FMT_GRAY14;
constexpr const PixelFormat PixelFormat::GRAY16  					=AV_PIX_FMT_GRAY16;
constexpr const PixelFormat PixelFormat::YA16    					=AV_PIX_FMT_YA16;
constexpr const PixelFormat PixelFormat::RGB48   					=AV_PIX_FMT_RGB48;
constexpr const PixelFormat PixelFormat::RGB565  					=AV_PIX_FMT_RGB565;
constexpr const PixelFormat PixelFormat::RGB555  					=AV_PIX_FMT_RGB555;
constexpr const PixelFormat PixelFormat::RGB444  					=AV_PIX_FMT_RGB444;
constexpr const PixelFormat PixelFormat::RGBA64  					=AV_PIX_FMT_RGBA64;
constexpr const PixelFormat PixelFormat::BGR48   					=AV_PIX_FMT_BGR48;
constexpr const PixelFormat PixelFormat::BGR565  					=AV_PIX_FMT_BGR565;
constexpr const PixelFormat PixelFormat::BGR555  					=AV_PIX_FMT_BGR555;
constexpr const PixelFormat PixelFormat::BGR444  					=AV_PIX_FMT_BGR444;
constexpr const PixelFormat PixelFormat::BGRA64  					=AV_PIX_FMT_BGRA64;
constexpr const PixelFormat PixelFormat::YUV420P9   				=AV_PIX_FMT_YUV420P9;
constexpr const PixelFormat PixelFormat::YUV422P9   				=AV_PIX_FMT_YUV422P9;
constexpr const PixelFormat PixelFormat::YUV444P9					=AV_PIX_FMT_YUV444P9;
constexpr const PixelFormat PixelFormat::YUV420P10  				=AV_PIX_FMT_YUV420P10;
constexpr const PixelFormat PixelFormat::YUV422P10  				=AV_PIX_FMT_YUV422P10;
constexpr const PixelFormat PixelFormat::YUV440P10 					=AV_PIX_FMT_YUV440P10;
constexpr const PixelFormat PixelFormat::YUV444P10  				=AV_PIX_FMT_YUV444P10;
constexpr const PixelFormat PixelFormat::YUV420P12 					=AV_PIX_FMT_YUV420P12;
constexpr const PixelFormat PixelFormat::YUV422P12 					=AV_PIX_FMT_YUV422P12;
constexpr const PixelFormat PixelFormat::YUV440P12 					=AV_PIX_FMT_YUV440P12;
constexpr const PixelFormat PixelFormat::YUV444P12 					=AV_PIX_FMT_YUV444P12;
constexpr const PixelFormat PixelFormat::YUV420P14 					=AV_PIX_FMT_YUV420P14;
constexpr const PixelFormat PixelFormat::YUV422P14  				=AV_PIX_FMT_YUV422P14;
constexpr const PixelFormat PixelFormat::YUV444P14 					=AV_PIX_FMT_YUV444P14;
constexpr const PixelFormat PixelFormat::YUV420P16 					=AV_PIX_FMT_YUV420P16;
constexpr const PixelFormat PixelFormat::YUV422P16 					=AV_PIX_FMT_YUV422P16;
constexpr const PixelFormat PixelFormat::YUV444P16	       			=AV_PIX_FMT_YUV444P16;
constexpr const PixelFormat PixelFormat::GBRP9     					=AV_PIX_FMT_GBRP9;
constexpr const PixelFormat PixelFormat::GBRP10    					=AV_PIX_FMT_GBRP10;
constexpr const PixelFormat PixelFormat::GBRP12   		  			=AV_PIX_FMT_GBRP12;
constexpr const PixelFormat PixelFormat::GBRP14    					=AV_PIX_FMT_GBRP14;
constexpr const PixelFormat PixelFormat::GBRP16           			=AV_PIX_FMT_GBRP16;
constexpr const PixelFormat PixelFormat::GBRAP10 		   			=AV_PIX_FMT_GBRAP10;
constexpr const PixelFormat PixelFormat::GBRAP12 		   			=AV_PIX_FMT_GBRAP12;
constexpr const PixelFormat PixelFormat::GBRAP16  		  			=AV_PIX_FMT_GBRAP16;
constexpr const PixelFormat PixelFormat::BAYER_BGGR16     			=AV_PIX_FMT_BAYER_BGGR16;
constexpr const PixelFormat PixelFormat::BAYER_RGGB16  				=AV_PIX_FMT_BAYER_RGGB16;
constexpr const PixelFormat PixelFormat::BAYER_GBRG16  				=AV_PIX_FMT_BAYER_GBRG16;
constexpr const PixelFormat PixelFormat::BAYER_GRBG16  				=AV_PIX_FMT_BAYER_GRBG16;
constexpr const PixelFormat PixelFormat::GBRPF32     				=AV_PIX_FMT_GBRPF32;
constexpr const PixelFormat PixelFormat::GBRAPF32    				=AV_PIX_FMT_GBRAPF32;
constexpr const PixelFormat PixelFormat::GRAYF32     				=AV_PIX_FMT_GRAYF32;
constexpr const PixelFormat PixelFormat::YUVA420P9   				=AV_PIX_FMT_YUVA420P9;
constexpr const PixelFormat PixelFormat::YUVA422P9   				=AV_PIX_FMT_YUVA422P9;
constexpr const PixelFormat PixelFormat::YUVA444P9   				=AV_PIX_FMT_YUVA444P9;
constexpr const PixelFormat PixelFormat::YUVA420P10  				=AV_PIX_FMT_YUVA420P10;
constexpr const PixelFormat PixelFormat::YUVA422P10  				=AV_PIX_FMT_YUVA422P10;
constexpr const PixelFormat PixelFormat::YUVA444P10  				=AV_PIX_FMT_YUVA444P10;
constexpr const PixelFormat PixelFormat::YUVA420P16  				=AV_PIX_FMT_YUVA420P16;
constexpr const PixelFormat PixelFormat::YUVA422P16  				=AV_PIX_FMT_YUVA422P16;
constexpr const PixelFormat PixelFormat::YUVA444P16  				=AV_PIX_FMT_YUVA444P16;
constexpr const PixelFormat PixelFormat::XYZ12       				=AV_PIX_FMT_XYZ12;
constexpr const PixelFormat PixelFormat::NV20        				=AV_PIX_FMT_NV20;
constexpr const PixelFormat PixelFormat::AYUV64      				=AV_PIX_FMT_AYUV64;
constexpr const PixelFormat PixelFormat::P010        				=AV_PIX_FMT_P010;
constexpr const PixelFormat PixelFormat::P016        				=AV_PIX_FMT_P016;
}

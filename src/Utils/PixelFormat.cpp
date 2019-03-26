#include <Utils/PixelFormat.h>

extern "C"{
	#include <libavutil/pixdesc.h>
}

using namespace Zuazo::Utils;

PixelFormat::PixelFormat(const std::string& name) :
		fmt( av_get_pix_fmt(name.c_str()) )
{
}

std::string PixelFormat::getPixelFormatName() const{
	const AVPixFmtDescriptor* desc = av_pix_fmt_desc_get(fmt);

	if(desc){
		return std::string(desc->name);
	}else{
		return std::string(); //Should not happen, an invalid pixel format was given
	}
}

bool PixelFormat::hasAlpha() const{
	const AVPixFmtDescriptor* desc = av_pix_fmt_desc_get(fmt);

	if(desc){
		return desc->flags & AV_PIX_FMT_FLAG_ALPHA;
	}else{
		return false; //Should not happen, an invalid pixel format was given
	}
}

bool PixelFormat::isBigEndian() const{
	const AVPixFmtDescriptor* desc = av_pix_fmt_desc_get(fmt);

	if(desc){
		return desc->flags & AV_PIX_FMT_FLAG_BE;
	}else{
		return false; //Should not happen, an invalid pixel format was given
	}
}

int PixelFormat::getBitsPerPixel() const{
	return av_get_bits_per_pixel(av_pix_fmt_desc_get(fmt));
}

int PixelFormat::getPlanesCount(){
	return av_pix_fmt_count_planes(fmt);
}

PixelFormat PixelFormat::swapEndianness() const{
	return PixelFormat(
			av_pix_fmt_swap_endianness(fmt)
	);
}

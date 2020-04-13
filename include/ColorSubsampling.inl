#include "ColorSubsampling.h"

namespace Zuazo {

constexpr std::string_view toString(ColorSubsampling colorSubsampling){
	switch(colorSubsampling){

	case ColorSubsampling::CHROMA_444: return "4:4:4";
	case ColorSubsampling::CHROMA_422: return "4:2:2";
	case ColorSubsampling::CHROMA_420: return "4:2:0";
	case ColorSubsampling::CHROMA_411: return "4:1:1";
	case ColorSubsampling::CHROMA_410: return "4:1:0";
	case ColorSubsampling::CHROMA_311: return "3:1:1";
	case ColorSubsampling::BAYER: return "Bayer";

	default: return "";
	}
}

constexpr Resolution getSubsampledResolution(ColorSubsampling subs, const Resolution& res){
	switch(subs){

	case ColorSubsampling::CHROMA_444: 
		return res;
	case ColorSubsampling::CHROMA_422: 
	case ColorSubsampling::BAYER: 
		return Resolution(res.width / 2, res.height);
	case ColorSubsampling::CHROMA_420:
		return Resolution(res.width / 2, res.height / 2);
	case ColorSubsampling::CHROMA_411:
		return Resolution(res.width / 4, res.height);
	case ColorSubsampling::CHROMA_410:
		return Resolution(res.width / 4, res.height / 2);
	case ColorSubsampling::CHROMA_311: 
		return Resolution(res.width / 3, res.height);

	default: return Resolution(0, 0);
	}
}

}
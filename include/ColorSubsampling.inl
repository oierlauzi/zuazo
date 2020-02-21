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

	default: return "";
	}
}

}
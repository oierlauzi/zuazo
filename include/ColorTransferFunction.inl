#include "ColorTransferFunction.h"

namespace Zuazo {

constexpr std::string_view toString(ColorTransferFunction colorSubsampling) {
	switch(colorSubsampling){

	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, LINEAR )
	ZUAZO_ENUM2STR_CASE( ColorTransferFunction, IEC61966_2_1 )

	default: return "";
	}
}	
	
}
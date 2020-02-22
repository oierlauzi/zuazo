#include "ColorRange.h"

namespace Zuazo {

constexpr std::string_view toString(ColorRange colorRange){
	switch(colorRange){

	ZUAZO_ENUM2STR_CASE( ColorRange, FULL )
	ZUAZO_ENUM2STR_CASE( ColorRange, ITU_NARROW )

	default: return "";
	}
}

}
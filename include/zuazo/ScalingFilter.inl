#include "ScalingFilter.h"

namespace Zuazo {

constexpr std::string_view toString(ScalingFilter filter){
	switch(filter){

	ZUAZO_ENUM2STR_CASE( ScalingFilter, NEAREST )
	ZUAZO_ENUM2STR_CASE( ScalingFilter, LINEAR )

	default: return "";
	}
}

inline std::ostream& operator<<(std::ostream& os, ScalingFilter filter) {
	return os << toString(filter);
}

}
#include "NamingConventions.h"

#include "../StringConversions.h"

namespace Zuazo::Signal {

template<typename T>
constexpr std::string_view makeInputName() {
	return "in";
}

template<typename T>
inline std::string makeInputName(size_t index) {
	return std::string(makeInputName<T>()) + toString(index);
}



template<typename T>
constexpr std::string_view makeOutputName() {
	return "out";
}

template<typename T>
inline std::string makeOutputName(size_t index) {
	return std::string(makeOutputName<T>()) + toString(index);
}


}
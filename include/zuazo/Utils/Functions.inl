#include "Functions.h"

#include "../Macros.h"

#include <algorithm>
#include <cassert>
#include <tuple>
#include <functional>
#include <sstream>


namespace Zuazo::Utils {

template<typename... T>
constexpr void ignore(const T&...){

}

template<typename T>
constexpr T bit(T pos){
	return T(1) << pos;
}

template<typename T, typename Q>
constexpr T align(T ptr, Q alignment) {
	if(ptr % alignment) {
		return (ptr / alignment + 1) * alignment;
	} else {
		return ptr;
	}
}


template<typename T, typename... Args>
inline typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
makeUnique(Args&&... args)
{
	#if ZUAZO_IS_CPP14
		return std::make_unique<T>(std::forward<Args>(args)...);
	#else
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	#endif
}

template<typename T, typename... Args>
inline typename std::enable_if<!std::is_array<T>::value, std::shared_ptr<T>>::type
makeShared(Args&&... args)
{
	#if ZUAZO_IS_CPP14
		return std::make_shared<T>(std::forward<Args>(args)...);
	#else
		return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
	#endif
}

}
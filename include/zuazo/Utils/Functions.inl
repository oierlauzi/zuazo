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

template<typename T>
inline T* align(T* ptr, size_t alignment) {
	static_assert(sizeof(T*) == sizeof(uintptr_t));
	return reinterpret_cast<T*>(align(reinterpret_cast<uintptr_t>(ptr), alignment));
}

constexpr uintptr_t align(uintptr_t ptr, size_t alignment) {
	return ((ptr + alignment - 1) / alignment) * alignment;
}


template<typename Func, typename... Args>
inline typename std::enable_if<!std::is_same<Func, typename std::invoke_result<Func, Args...>::type>::value, void>::type
invokeIf(Func&& f, Args&&... args) {
	if(f) {
		std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
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
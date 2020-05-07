#include "Configuration.h"

#include <functional>

namespace Zuazo::Utils {

template<typename Names, typename... Types> 
template <typename... T>
constexpr Configuration<Names, Types...>::Configuration(T&&... params)
	: m_parameters(std::forward<T>(params)...)
{
}


template<typename Names, typename... Types>  
constexpr int Configuration<Names, Types...>::operator==(const Configuration& other) const {
	return m_parameters == other.m_parameters;
}

template<typename Names, typename... Types>  
constexpr int Configuration<Names, Types...>::operator!=(const Configuration& other) const {
	return m_parameters != other.m_parameters;
}

template<typename Names, typename... Types>  
constexpr int Configuration<Names, Types...>::operator<(const Configuration& other) const {
	return m_parameters < other.m_parameters;
}

template<typename Names, typename... Types>  
constexpr int Configuration<Names, Types...>::operator<=(const Configuration& other) const {
	return m_parameters <= other.m_parameters;
}

template<typename Names, typename... Types>  
constexpr int Configuration<Names, Types...>::operator>(const Configuration& other) const {
	return m_parameters > other.m_parameters;
}

template<typename Names, typename... Types>  
constexpr int Configuration<Names, Types...>::operator>=(const Configuration& other) const {
	return m_parameters >= other.m_parameters;
}



template<typename Names, typename... Types>  
template <typename Configuration<Names, Types...>::Parameters param, typename T>
constexpr void Configuration<Names, Types...>::set(T&& val) {
	std::get<static_cast<size_t>(param)>(m_parameters) = std::forward<T>(val);
}

template<typename Names, typename... Types>  
template <typename Configuration<Names, Types...>::Parameters param>
constexpr const auto& Configuration<Names, Types...>::get() const {
	return std::get<static_cast<size_t>(param)>(m_parameters);
}


template<typename Names, typename... Types>  
constexpr typename Configuration<Names, Types...>::Compatibility 
Configuration<Names, Types...>::intersection(	const Compatibility& a, 
												const Compatibility& b )
{
	return elementwiseOperation(
		[] (const auto& x, const auto y) -> auto {
			using T = typename std::remove_reference<decltype(x)>::type;
			return Limitation<T>::intersection(x, y);
		},
		a, b
	);
}

template<typename Names, typename... Types>  
constexpr Configuration<Names, Types...> 
Configuration<Names, Types...>::selectWorst(const Compatibility& a) 
{
	const auto selection = elementwiseOperation(
		[] (const auto& x) -> auto {
			using T = typename std::remove_reference<decltype(x)>::type;
			return Limitation<T>::worst(x);
		},
		a
	);

	return std::make_from_tuple<Configuration>(selection);
}

template<typename Names, typename... Types>  
constexpr Configuration<Names, Types...> 
Configuration<Names, Types...>::selectBest(const Compatibility& a) 
{
	const auto selection = elementwiseOperation(
		[] (const auto& x) -> auto {
			using T = typename std::remove_reference<decltype(x)>::type;
			return Limitation<T>::best(x);
		},
		a
	);

	return std::make_from_tuple<Configuration>(selection);
}



}
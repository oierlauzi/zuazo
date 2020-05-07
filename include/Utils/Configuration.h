#pragma once

#include "Limitation.h"

#include <tuple>
#include <vector>
#include <variant>
#include <type_traits>

namespace Zuazo::Utils {

template<typename Names, typename... Types> 
class Configuration {
	static_assert(std::is_enum_v<Names>, "Names must be a enumeration");
	using Tuple = std::tuple<Types...>;
public:
	using Parameters = Names;
	template<Parameters param>
	using ParameterType = typename std::tuple_element<static_cast<size_t>(param), Tuple>::type;

	using Compatibility = std::tuple<typename Limitation<Types>::Limit...>;
	using Compatibilities = std::vector<Compatibility>;

	constexpr Configuration() = default;
	template <typename... T>
	constexpr Configuration(T&&... params);
	constexpr Configuration(const Configuration& other) = default;
	constexpr Configuration(Configuration&& other) = default;
	~Configuration() = default;

	constexpr Configuration&		operator=(const Configuration& other) = default;
	constexpr Configuration&		operator=(Configuration&& other) = default;

	constexpr int					operator==(const Configuration& other) const;
	constexpr int					operator!=(const Configuration& other) const;
	constexpr int					operator<(const Configuration& other) const;
	constexpr int					operator<=(const Configuration& other) const;
	constexpr int					operator>(const Configuration& other) const;
	constexpr int					operator>=(const Configuration& other) const;

	template <Parameters param, typename T>
	constexpr void					set(T&& val);
	template <Parameters param>
	constexpr const auto& 			get() const;


	static constexpr Compatibility	intersection(	const Compatibility& a, 
													const Compatibility& b );

	static constexpr Configuration	selectWorst(const Compatibility& a);
	static constexpr Configuration	selectBest(const Compatibility& a);

private:
	Tuple							m_parameters;
};

}

#include "Configuration.inl"
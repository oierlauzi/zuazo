#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <variant>
#include <type_traits>

namespace Zuazo::Utils {

template<typename Names, typename... Types> 
class Configuration {
	static_assert(std::is_enum<Names>::value, "Names must be a enumeration");
public:
	template <typename T>
	struct Any {};

	template <typename T>
	struct Range { T min, max; };

	template <typename T>
	using Discrete = std::vector<T>;

	template <typename T>
	struct MustBe { T value; };

	template <typename T>
	struct None {};

	template <typename T>
	using Limit = std::variant<	Any<T>,
								Range<T>,
								Discrete<T>,
								MustBe<T>,
								None<T> >;

	using Parameters = Names;
	using Tuple = std::tuple<Types...>;
	template<Parameters param>
	using ParameterType = typename std::tuple_element<static_cast<size_t>(param), Tuple>::type;

	using Compatibility = std::tuple<Limit<Types>...>;
	using Compatibilities = std::vector<Compatibility>;

	template<typename T>
	using ResultArray = std::array<T, sizeof...(Types)>;

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

	constexpr const Tuple&			asTuple() const;


	template <typename T>
	static constexpr bool 			validate(const T& value, Any<T> limit);
	template <typename T>
	static constexpr bool 			validate(const T& value, const Range<T>& limit);
	template <typename T>
	static constexpr bool 			validate(const T& value, const Discrete<T>& limit);
	template <typename T>
	static constexpr bool 			validate(const T& value, const MustBe<T>& limit);
	template <typename T>
	static constexpr bool 			validate(const T& value, None<T> limit);
	template <typename T>
	static constexpr bool 			validate(const T& value, const Limit<T>& limit);
	static constexpr ResultArray<bool>	validate(	const Configuration& conf,
													const Compatibility& comp);


	template <typename T>
	static constexpr Limit<T> 		simplify(Any<T> limit);
	template <typename T>
	static constexpr Limit<T> 		simplify(const Range<T>& limit);
	template <typename T>
	static constexpr Limit<T> 		simplify(const Discrete<T>& limit);
	template <typename T>
	static constexpr Limit<T> 		simplify(const MustBe<T>& limit);
	template <typename T>
	static constexpr Limit<T> 		simplify(None<T> limit);
	template <typename T>
	static constexpr Limit<T> 		simplify(const Limit<T>& limit);
	static constexpr Compatibility	simplify(const Compatibility& comp);


	template <typename T>
	static constexpr Limit<T> 		intersection(Any<T> a, Any<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(Any<T> a, const Range<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(Any<T> a, const Discrete<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(Any<T> a, const MustBe<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(Any<T> a, None<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Range<T>& a, Any<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Range<T>& a, const Range<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Range<T>& a, const Discrete<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Range<T>& a, const MustBe<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Range<T>& a, None<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Discrete<T>& a, Any<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Discrete<T>& a, const Range<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Discrete<T>& a, const Discrete<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Discrete<T>& a, const MustBe<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Discrete<T>& a, None<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const MustBe<T>& a, Any<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const MustBe<T>& a, const Range<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const MustBe<T>& a, const Discrete<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const MustBe<T>& a, const MustBe<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const MustBe<T>& a, None<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(None<T> a, Any<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(None<T> a, const Range<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(None<T> a, const Discrete<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(None<T> a, const MustBe<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(None<T> a, None<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Limit<T>& a, const Limit<T>& b);
	static constexpr Compatibility	intersection(	const Compatibility& a, 
													const Compatibility& b );


	template <typename T>
	static constexpr T 				lowest(Any<T> limit);
	template <typename T>
	static constexpr T 				lowest(const Range<T>& limit);
	template <typename T>
	static constexpr T 				lowest(const Discrete<T>& limit);
	template <typename T>
	static constexpr T 				lowest(const MustBe<T>& limit);
	template <typename T>
	static constexpr T 				lowest(None<T> limit);
	template <typename T>
	static constexpr T 				lowest(const Limit<T>& limit);
	static constexpr Configuration	lowest(const Compatibility& a);


	template <typename T>
	static constexpr T 				highest(Any<T> limit);
	template <typename T>
	static constexpr T 				highest(const Range<T>& limit);
	template <typename T>
	static constexpr T 				highest(const Discrete<T>& limit);
	template <typename T>
	static constexpr T 				highest(const MustBe<T>& limit);
	template <typename T>
	static constexpr T 				highest(None<T> limit);
	template <typename T>
	static constexpr T 				highest(const Limit<T>& limit);
	static constexpr Configuration	highest(const Compatibility& a);

private:
	Tuple							m_parameters;
};

}

namespace Zuazo {

template<typename Names, typename... Types> 
std::string toString(const Utils::Configuration<Names, Types...>& conf);

}

#include "Configuration.inl"
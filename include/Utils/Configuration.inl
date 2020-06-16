#include "Configuration.h"

#include "Functions.h"
#include "../Math/Functions.h"

#include <functional>
#include <array>

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
constexpr Configuration<Names, Types...>::operator bool() const {
	return std::apply(
		[] (const auto&... args) -> bool {
			return (static_cast<bool>(args) && ...);
		}, 
		m_parameters
	);
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
constexpr const typename Configuration<Names, Types...>::Tuple&
Configuration<Names, Types...>::asTuple() const {
	return m_parameters;
}



template<typename Names, typename... Types>  
template <typename T>
constexpr bool Configuration<Names, Types...>::validate(const T&, Any<T>) {
	return true;
}

template<typename Names, typename... Types>  
template <typename T>
constexpr bool Configuration<Names, Types...>::validate(const T& value, const Range<T>& limit) {
	return Math::isInRange(value, limit.min, limit.max);
}

template<typename Names, typename... Types>  
template <typename T>
constexpr bool Configuration<Names, Types...>::validate(const T& value, const Discrete<T>& limit) {
	return std::find(limit.cbegin(), limit.cend(), value) != limit.cend();
}

template<typename Names, typename... Types>  
template <typename T>
constexpr bool Configuration<Names, Types...>::validate(const T& value, const MustBe<T>& limit) {
	return limit.value == value;
}

template<typename Names, typename... Types>  
template <typename T>
constexpr bool Configuration<Names, Types...>::validate(const T&, None<T>) {
	return false;
}

template<typename Names, typename... Types>  
template <typename T>
constexpr bool Configuration<Names, Types...>::validate(const T& value, const Limit<T>& limit) {
	return std::visit(
		[&] (const auto& x) -> auto{
			return Configuration::validate(value, x);
		},
		limit
	);
}

template<typename Names, typename... Types>  
constexpr typename Configuration<Names, Types...>::Validation
Configuration<Names, Types...>::validate(	const Configuration& conf,
											const Compatibility& comp)
{
	return std::apply(
		[] (auto&&... x) -> auto {
			constexpr char FALSE = '0',  TRUE = '1';

			const std::array str {(x ? TRUE : FALSE) ...};
			return Validation(str.data(), str.size(), FALSE, TRUE);
		},
		elementwiseOperation(
			[] (const auto& x, const auto& y) {
				return Configuration::validate(x, y);
			}, 
			conf.m_parameters, comp
		)
	);
}

template<typename Names, typename... Types>  
constexpr typename Configuration<Names, Types...>::Validation
Configuration<Names, Types...>::validate(	const Configuration& conf,
											const Compatibilities& comp)
{
	Validation best;

	for(const auto& c : comp) {
		//Validate this compatibility
		const auto r = validate(conf, c); 

		//Evaluate if it is better suited
		if(r.count() > best.count()) best = r;
	}

	return best;
}



template<typename Names, typename... Types>  
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::simplify(Any<T>) 
{
	return Any<T>();
}

template<typename Names, typename... Types>  
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T> 
Configuration<Names, Types...>::simplify(const Range<T>& limit) 
{
	if(limit.min < limit.max) {
		return limit;
	} else if(limit.min == limit.max) {
		return MustBe<T>{limit.min};
	} else { // limit.min > limit.max
		return None<T>();
	}
}

template<typename Names, typename... Types>  
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::simplify(const Discrete<T>& limit) 
{
	if(limit.size() > 1) {
		return limit;
	} else if(limit.size() == 1) {
		return MustBe<T>{limit[0]};
	} else { // limit.size() == 0
		return None<T>();
	}
}

template<typename Names, typename... Types>  
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::simplify(const MustBe<T>& limit) 
{
	return limit;
}

template<typename Names, typename... Types>  
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::simplify(None<T>) 
{
	return None<T>();
}

template<typename Names, typename... Types>  
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::simplify(const Limit<T>& limit) 
{
	return std::visit(
		[] (const auto& x) -> auto{
			return Configuration::simplify(x);
		},
		limit
	);
}

template<typename Names, typename... Types>  
constexpr typename Configuration<Names, Types...>::Compatibility
Configuration<Names, Types...>::simplify(const Compatibility& comp) 
{
	return elementwiseOperation(
		[] (const auto& x) -> auto {
			return Configuration::simplify(x);
		},		
		comp
	);
}



template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(Any<T>, Any<T>) 
{
	return Any<T>();
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(Any<T>, const Range<T>& b) 
{
	return b;
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(Any<T>, const Discrete<T>& b) 
{
	return b;
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(Any<T>, const MustBe<T>& b) 
{
	return b;
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(Any<T>, None<T>) 
{
	return None<T>();
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const Range<T>& a, Any<T>)
{
	return a;
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const Range<T>& a, const Range<T>& b) 
{
	return simplify(
		Range<T>{
			max(a.min, b.min),
			min(a.max, b.max)
		}
	);
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const Range<T>& a, const Discrete<T>& b)
{
	Discrete<T> result;

	for(const auto& val : b) {
		if(validate(val, a)){
			result.push_back(val);
		}
	}

	return simplify(result);
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const Range<T>& a, const MustBe<T>& b) 
{
	if(validate(b.value, a)) {
		return b;
	} else {
		return None<T>();
	}
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const Range<T>&, None<T>) 
{
	return None<T>();
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const Discrete<T>& a, Any<T>) 
{
	return a;
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const Discrete<T>& a, const Range<T>& b) 
{
	return intersection(b, a);
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const Discrete<T>& a, const Discrete<T>& b) 
{
	Discrete<T> result;

	for(const auto& val : b) {
		if(validate(val, a)){
			result.push_back(val);
		}
	}

	return simplify(result);
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const Discrete<T>& a, const MustBe<T>& b) 
{
	if(validate(b.value, a)) {
		return b;
	} else {
		return None<T>();
	}
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const Discrete<T>&, None<T>) 
{
	return None<T>();
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const MustBe<T>& a, Any<T>)
{
	return a;
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const MustBe<T>& a, const Range<T>& b) 
{
	return intersection(b, a);
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const MustBe<T>& a, const Discrete<T>& b) 
{
	return intersection(b, a);
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const MustBe<T>& a, const MustBe<T>& b) 
{
	if(a.value == b.value) {
		return b;
	} else {
		return None<T>();
	}
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const MustBe<T>&, None<T>) 
{
	return None<T>();
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(None<T>, Any<T>) 
{
	return None<T>();
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(None<T>, const Range<T>&) 
{
	return None<T>();
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(None<T>, const Discrete<T>&) 
{
	return None<T>();
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(None<T>, const MustBe<T>&) 
{
	return None<T>();
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(None<T>, None<T>) 
{
	return None<T>();
}

template<typename Names, typename... Types>
template <typename T>
constexpr typename Configuration<Names, Types...>::template Limit<T>
Configuration<Names, Types...>::intersection(const Limit<T>& a, const Limit<T>& b) 
{
	return std::visit(
		[&] (const auto& x, const auto& y) -> auto{
			return Configuration::intersection(x, y);
		},
		a, b
	);
}

template<typename Names, typename... Types>  
constexpr typename Configuration<Names, Types...>::Compatibility 
Configuration<Names, Types...>::intersection(	const Compatibility& a, 
												const Compatibility& b )
{
	return elementwiseOperation(
		[] (const auto& x, const auto& y) -> auto {
			return Configuration::intersection(x, y);
		},		
		a, b
	);
}



template<typename Names, typename... Types>  
template <typename T>
constexpr T Configuration<Names, Types...>::lowest(Any<T>) {
	assert(false); //TODO
}

template<typename Names, typename... Types>  
template <typename T>
constexpr T Configuration<Names, Types...>::lowest(const Range<T>& limit) {
	return limit.min;
}

template<typename Names, typename... Types>  
template <typename T>
constexpr T Configuration<Names, Types...>::lowest(const Discrete<T>& limit) {
	assert(std::is_sorted(limit.cbegin(), limit.cend()));
	return limit.front();
}

template<typename Names, typename... Types>  
template <typename T>
constexpr T Configuration<Names, Types...>::lowest(const MustBe<T>& limit) {
	return limit.value;
}

template<typename Names, typename... Types>  
template <typename T>
constexpr T Configuration<Names, Types...>::lowest(None<T>) {
	assert(false);
}

template<typename Names, typename... Types>  
template <typename T>
constexpr T Configuration<Names, Types...>::lowest(const Limit<T>& limit) {
	return std::visit(
		[&] (const auto& x) -> auto{
			return Configuration::lowest(x);
		},
		limit
	);
}

template<typename Names, typename... Types>  
constexpr Configuration<Names, Types...> 
Configuration<Names, Types...>::lowest(const Compatibility& a) 
{
	return std::make_from_tuple<Configuration>(
		elementwiseOperation(
			[] (const auto& x) -> auto {
				return Configuration::lowest(x);
			},		
			a
		)
	);
}



template<typename Names, typename... Types>
template <typename T>
constexpr T Configuration<Names, Types...>::highest(Any<T>) {
	assert(false); //TODO
}

template<typename Names, typename... Types>
template <typename T>
constexpr T Configuration<Names, Types...>::highest(const Range<T>& limit) {
	return limit.max;
}

template<typename Names, typename... Types>
template <typename T>
constexpr T Configuration<Names, Types...>::highest(const Discrete<T>& limit) {
	assert(std::is_sorted(limit.cbegin(), limit.cend()));
	return limit.back();
}

template<typename Names, typename... Types>
template <typename T>
constexpr T Configuration<Names, Types...>::highest(const MustBe<T>& limit) {
	return limit.value;
}

template<typename Names, typename... Types>
template <typename T>
constexpr T Configuration<Names, Types...>::highest(None<T>) {
	assert(false);
}

template<typename Names, typename... Types>
template <typename T>
constexpr T Configuration<Names, Types...>::highest(const Limit<T>& limit) {
	return std::visit(
		[&] (const auto& x) -> auto{
			return Configuration::highest(x);
		},
		limit
	);
}

template<typename Names, typename... Types>  
constexpr Configuration<Names, Types...> 
Configuration<Names, Types...>::highest(const Compatibility& a) 
{
	return std::make_from_tuple<Configuration>(
		elementwiseOperation(
			[] (const auto& x) -> auto {
				return Configuration::highest(x);
			},		
			a
		)
	);
}

}

namespace Zuazo {

template<typename Names, typename... Types> 
inline std::string toString(const Utils::Configuration<Names, Types...>& conf) {
	using Parameters = typename Utils::Configuration<Names, Types...>::Parameters;

	const auto arr = std::apply(
		[] (const auto&... x) -> auto {
			return std::array{std::string(toString(x))...};
		},
		conf.asTuple()
	);

	std::string result = "{";

	for(size_t i = 0; i < arr.size(); i++) {
		//Append the parameter as pseudo json
		result += "\"" + std::string(toString(static_cast<Parameters>(i))) + "\": \"" + arr[i] + "\"";

		//Evaluate if it is the last so that it gets a comma or not
		if(i < arr.size() - 1) result += ", ";
	}

	result += "}";
	return result;
}

}
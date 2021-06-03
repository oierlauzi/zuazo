#pragma once

#include "../StringConversions.h"

#include <vector>
#include <variant>
#include <string_view>

namespace Zuazo::Utils {

enum class LimitType {
	none,
	mustBe,
	discrete,
	discreteRange,
	range,
	any
};



template <typename T>
class Any;

template <typename T>
class Range;

template <typename T>
class DiscreteRange;

template <typename T>
class Discrete;

template <typename T>
class MustBe;

template <typename T>
class None;

template <typename T>
class Limit;



template <typename T>
class Any {
public:
	constexpr Any() = default;
	constexpr Any(const Any& other) = default;
	constexpr Any(Any&& other) = default;
	~Any() = default;

	constexpr Any&						operator=(const Any& other) = default;
	constexpr Any&						operator=(Any&& other) = default;

	constexpr bool						operator==(const Any& other) const;
	constexpr bool						operator!=(const Any& other) const;
	constexpr bool						operator<(const Any& other) const;
	constexpr bool						operator<=(const Any& other) const;
	constexpr bool						operator>(const Any& other) const;
	constexpr bool						operator>=(const Any& other) const;

	constexpr T							lowest() const;
	constexpr T							highest() const;
	constexpr T							value() const;

	constexpr bool						hasValue() const;
	constexpr bool						validate(const T& value) const;
	constexpr const Any<T>&				simplify() const;

	constexpr const Any<T>& 			intersect(const Any<T>& other) const;
	constexpr const Range<T>& 			intersect(const Range<T>& other) const;
	constexpr const DiscreteRange<T>&	intersect(const DiscreteRange<T>& other) const;
	constexpr const Discrete<T>& 		intersect(const Discrete<T>& other) const;
	constexpr const MustBe<T>& 			intersect(const MustBe<T>& other) const;
	constexpr const None<T>& 			intersect(const None<T>& other) const;

};

template <typename T>
class Range {
public:
	constexpr Range() = default;
	constexpr Range(T min, T max);
	constexpr Range(const Range& other) = default;
	constexpr Range(Range&& other) = default;
	~Range() = default;

	constexpr Range&					operator=(const Range& other) = default;
	constexpr Range&					operator=(Range&& other) = default;

	constexpr bool						operator==(const Range& other) const;
	constexpr bool						operator!=(const Range& other) const;
	constexpr bool						operator<(const Range& other) const;
	constexpr bool						operator<=(const Range& other) const;
	constexpr bool						operator>(const Range& other) const;
	constexpr bool						operator>=(const Range& other) const;

	constexpr void						setMin(T min);
    constexpr const T&					getMin() const;

	constexpr void						setMax(T max);
    constexpr const T&					getMax() const;

	constexpr const T&					lowest() const;
	constexpr const T&					highest() const;
	constexpr const T&					value() const;

	constexpr bool						hasValue() const;
	constexpr bool						validate(const T& value) const;
	constexpr Limit<T>					simplify() const;

	constexpr const Range<T>&			intersect(const Any<T>& other) const;
	constexpr Limit<T> 					intersect(const Range<T>& other) const;
	constexpr Limit<T> 					intersect(const DiscreteRange<T>& other) const;
	Limit<T> 							intersect(const Discrete<T>& other) const;
	constexpr Limit<T> 					intersect(const MustBe<T>& other) const;
	constexpr const None<T>&			intersect(const None<T>& other) const;

private:
	T									m_min, m_max;

};

template <typename T>
class DiscreteRange {
public:
	constexpr DiscreteRange() = default;
	constexpr DiscreteRange(T min, T max, T step);
	constexpr DiscreteRange(const DiscreteRange& other) = default;
	constexpr DiscreteRange(DiscreteRange&& other) = default;
	~DiscreteRange() = default;

	constexpr DiscreteRange&			operator=(const DiscreteRange& other) = default;
	constexpr DiscreteRange&			operator=(DiscreteRange&& other) = default;

	constexpr bool						operator==(const DiscreteRange& other) const;
	constexpr bool						operator!=(const DiscreteRange& other) const;
	constexpr bool						operator<(const DiscreteRange& other) const;
	constexpr bool						operator<=(const DiscreteRange& other) const;
	constexpr bool						operator>(const DiscreteRange& other) const;
	constexpr bool						operator>=(const DiscreteRange& other) const;

	constexpr void						setMin(T min);
    constexpr const T&					getMin() const;

	constexpr void						setMax(T max);
    constexpr const T&					getMax() const;

	constexpr void						setStep(T step);
    constexpr const T&					getStep() const;

	constexpr T							lowerBound(T value) const;
	constexpr T							upperBound(T value) const;

	constexpr size_t					size() const;

	constexpr const T&					lowest() const;
	constexpr const T&					highest() const;
	constexpr const T&					value() const;

	constexpr bool						hasValue() const;
	constexpr bool						validate(const T& value) const;
	constexpr Limit<T>					simplify() const;

	constexpr const DiscreteRange<T>&	intersect(const Any<T>& other) const;
	constexpr Limit<T> 					intersect(const Range<T>& other) const;
	constexpr Limit<T> 					intersect(const DiscreteRange<T>& other) const;
	Limit<T> 							intersect(const Discrete<T>& other) const;
	constexpr Limit<T> 					intersect(const MustBe<T>& other) const;
	constexpr const None<T>& 			intersect(const None<T>& other) const;

private:
	T									m_min, m_max, m_step;

};

template <typename T>
class Discrete 
	: public std::vector<T> 
{
public:
	constexpr explicit Discrete(std::vector<T> vec);
	using std::vector<T>::vector;
	using std::vector<T>::operator=;

	constexpr bool						operator==(const Discrete& other) const;
	constexpr bool						operator!=(const Discrete& other) const;
	constexpr bool						operator<(const Discrete& other) const;
	constexpr bool						operator<=(const Discrete& other) const;
	constexpr bool						operator>(const Discrete& other) const;
	constexpr bool						operator>=(const Discrete& other) const;

	constexpr const T&					lowest() const;
	constexpr const T&					highest() const;
	constexpr const T&					value() const;

	constexpr bool						hasValue() const;
	constexpr bool						validate(const T& value) const;
	constexpr Limit<T>					simplify() const;

	constexpr const Discrete<T>&		intersect(const Any<T>& other) const;
	constexpr Limit<T> 					intersect(const Range<T>& other) const;
	constexpr Limit<T> 					intersect(const DiscreteRange<T>& other) const;
	Limit<T> 							intersect(const Discrete<T>& other) const;
	constexpr Limit<T> 					intersect(const MustBe<T>& other) const;
	constexpr const None<T>&			intersect(const None<T>& other) const;
};

template <typename T>
class MustBe {
public:
	constexpr MustBe() = default;
	constexpr explicit MustBe(T value);
	constexpr MustBe(const MustBe& other) = default;
	constexpr MustBe(MustBe&& other) = default;
	~MustBe() = default;

	constexpr MustBe&					operator=(const MustBe& other) = default;
	constexpr MustBe&					operator=(MustBe&& other) = default;

	constexpr bool						operator==(const MustBe& other) const;
	constexpr bool						operator!=(const MustBe& other) const;
	constexpr bool						operator<(const MustBe& other) const;
	constexpr bool						operator<=(const MustBe& other) const;
	constexpr bool						operator>(const MustBe& other) const;
	constexpr bool						operator>=(const MustBe& other) const;

	constexpr void						setValue(T value);
    constexpr const T&					getValue() const;

	constexpr const T&					lowest() const;
	constexpr const T&					highest() const;
	constexpr const T&					value() const;

	constexpr bool						hasValue() const;
	constexpr bool						validate(const T& value) const;
	constexpr MustBe<T>					simplify() const;

	constexpr const MustBe<T>&			intersect(const Any<T>& other) const;
	constexpr Limit<T> 					intersect(const Range<T>& other) const;
	constexpr Limit<T> 					intersect(const DiscreteRange<T>& other) const;
	Limit<T> 							intersect(const Discrete<T>& other) const;
	constexpr Limit<T> 					intersect(const MustBe<T>& other) const;
	constexpr const None<T>&			intersect(const None<T>& other) const;

private:
    T							m_value;

};

template <typename T>
class None {
public:
	constexpr None() = default;
	constexpr None(const None& other) = default;
	constexpr None(None&& other) = default;
	~None() = default;

	constexpr None&						operator=(const None& other) = default;
	constexpr None&						operator=(None&& other) = default;

	constexpr bool						operator==(const None& other) const;
	constexpr bool						operator!=(const None& other) const;
	constexpr bool						operator<(const None& other) const;
	constexpr bool						operator<=(const None& other) const;
	constexpr bool						operator>(const None& other) const;
	constexpr bool						operator>=(const None& other) const;

	constexpr const T&					lowest() const;
	constexpr const T&					highest() const;
	constexpr const T&					value() const;

	constexpr bool						hasValue() const;
	constexpr bool						validate(const T& value) const;
	constexpr const None<T>&			simplify() const;

	constexpr const None<T>&			intersect(const Any<T>& other) const;
	constexpr const None<T>&			intersect(const Range<T>& other) const;
	constexpr const None<T>&			intersect(const DiscreteRange<T>& other) const;
	constexpr const None<T>&			intersect(const Discrete<T>& other) const;
	constexpr const None<T>&			intersect(const MustBe<T>& other) const;
	constexpr const None<T>&			intersect(const None<T>& other) const;

};



template <typename T>
class Limit {
public:
	constexpr Limit() = default;
	constexpr Limit(None<T> none);
	constexpr Limit(MustBe<T> mustBe);
	constexpr Limit(Discrete<T> discrete);
	constexpr Limit(DiscreteRange<T> discreteRange);
	constexpr Limit(Range<T> range);
	constexpr Limit(Any<T> any);
	constexpr Limit(const Limit& other) = default;
	constexpr Limit(Limit&& other) = default;
	~Limit() = default;

	constexpr Limit&					operator=(None<T> none);
	constexpr Limit&					operator=(MustBe<T> mustBe);
	constexpr Limit&					operator=(Discrete<T> discrete);
	constexpr Limit&					operator=(DiscreteRange<T> discreteRange);
	constexpr Limit&					operator=(Range<T> range);
	constexpr Limit&					operator=(Any<T> any);
	constexpr Limit&					operator=(const Limit& other) = default;
	constexpr Limit&					operator=(Limit&& other) = default;

	constexpr bool						operator==(const Limit& other) const;
	constexpr bool						operator!=(const Limit& other) const;
	constexpr bool						operator<(const Limit& other) const;
	constexpr bool						operator<=(const Limit& other) const;
	constexpr bool						operator>(const Limit& other) const;
	constexpr bool						operator>=(const Limit& other) const;

	constexpr 							operator bool() const;

	constexpr LimitType					getType() const;
	constexpr None<T>&					getNone();
	constexpr const None<T>&			getNone() const;
	constexpr MustBe<T>&				getMustBe();
	constexpr const MustBe<T>&			getMustBe() const;
	constexpr Discrete<T>&				getDiscrete();
	constexpr const Discrete<T>&		getDiscrete() const;
	constexpr DiscreteRange<T>&			getDiscreteRange();
	constexpr const DiscreteRange<T>&	getDiscreteRange() const;
	constexpr Range<T>&					getRange();
	constexpr const Range<T>&			getRange() const;
	constexpr Any<T>&					getAny();
	constexpr const Any<T>&				getAny() const;

	constexpr T							lowest() const;
	constexpr T							highest() const;
	constexpr T							value() const;

	constexpr bool						hasValue() const;
	constexpr bool						validate(const T& value) const;
	constexpr Limit<T>					simplify() const;
	constexpr Limit<T> 					intersect(const Limit<T>& other) const;

private:
	using Data = std::variant<	None<T>,
								MustBe<T>,
								Discrete<T>,
								DiscreteRange<T>,
								Range<T>,
								Any<T> >;

	Data								m_data;

};



std::ostream& operator<<(std::ostream& os, LimitType type);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Any<T>& any);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Range<T>& range);
template <typename T>
std::ostream& operator<<(std::ostream& os, const DiscreteRange<T>& discreteRange);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Discrete<T>& discrete);
template <typename T>
std::ostream& operator<<(std::ostream& os, const MustBe<T>& mustBe);
template <typename T>
std::ostream& operator<<(std::ostream& os, const None<T>& none);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Limit<T>& limit);

}

namespace Zuazo {

constexpr std::string_view toString(Utils::LimitType type) noexcept;

}

#include "Limit.inl"

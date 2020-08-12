#pragma once

#include "../StringConversions.h"

#include <vector>
#include <variant>
#include <string_view>

namespace Zuazo::Utils {

template <typename T>
class Any {
public:
	static constexpr std::string_view LIMIT_TYPE = "ANY";

	constexpr Any() = default;
	constexpr Any(const Any& other) = default;
	constexpr Any(Any&& other) = default;
	~Any() = default;

	constexpr Any&				operator=(const Any& other) = default;
	constexpr Any&				operator=(Any&& other) = default;

	constexpr bool				operator==(const Any& other) const;
	constexpr bool				operator!=(const Any& other) const;
	constexpr bool				operator<(const Any& other) const;
	constexpr bool				operator<=(const Any& other) const;
	constexpr bool				operator>(const Any& other) const;
	constexpr bool				operator>=(const Any& other) const;
};

template <typename T>
class Range {
public:
	static constexpr std::string_view LIMIT_TYPE = "RANGE";

	constexpr Range() = default;
	constexpr Range(T min, T max);
	constexpr Range(const Range& other) = default;
	constexpr Range(Range&& other) = default;
	~Range() = default;

	constexpr Range&			operator=(const Range& other) = default;
	constexpr Range&			operator=(Range&& other) = default;

	constexpr bool				operator==(const Range& other) const;
	constexpr bool				operator!=(const Range& other) const;
	constexpr bool				operator<(const Range& other) const;
	constexpr bool				operator<=(const Range& other) const;
	constexpr bool				operator>(const Range& other) const;
	constexpr bool				operator>=(const Range& other) const;

	constexpr void				setMin(T min);
    constexpr const T&			getMin() const;

	constexpr void				setMax(T max);
    constexpr const T&			getMax() const;
private:
	T							m_min, m_max;

};

template <typename T>
class DiscreteRange 
	: public Range<T> 
{
public:
	static constexpr std::string_view LIMIT_TYPE = "DISCRETE_RANGE";

	constexpr DiscreteRange() = default;
	constexpr DiscreteRange(T min, T max, T step);
	constexpr DiscreteRange(const DiscreteRange& other) = default;
	constexpr DiscreteRange(DiscreteRange&& other) = default;
	~DiscreteRange() = default;

	constexpr DiscreteRange&	operator=(const DiscreteRange& other) = default;
	constexpr DiscreteRange&	operator=(DiscreteRange&& other) = default;

	constexpr bool				operator==(const DiscreteRange& other) const;
	constexpr bool				operator!=(const DiscreteRange& other) const;
	constexpr bool				operator<(const DiscreteRange& other) const;
	constexpr bool				operator<=(const DiscreteRange& other) const;
	constexpr bool				operator>(const DiscreteRange& other) const;
	constexpr bool				operator>=(const DiscreteRange& other) const;

	constexpr void				setStep(T step);
    constexpr const T&			getStep() const;

	constexpr T					lowerBound(T value) const;
	constexpr T					upperBound(T value) const;

	constexpr size_t			size() const;

private:
	T							m_step;

};

template <typename T>
class Discrete 
	: public std::vector<T> 
{
public:
	static constexpr std::string_view LIMIT_TYPE = "DISCRETE";

	using std::vector<T>::vector;
	using std::vector<T>::operator=;

	constexpr bool				operator==(const Discrete& other) const;
	constexpr bool				operator!=(const Discrete& other) const;
	constexpr bool				operator<(const Discrete& other) const;
	constexpr bool				operator<=(const Discrete& other) const;
	constexpr bool				operator>(const Discrete& other) const;
	constexpr bool				operator>=(const Discrete& other) const;
};

template <typename T>
class MustBe {
public:
	static constexpr std::string_view LIMIT_TYPE = "MUST_BE";

	constexpr MustBe() = default;
	constexpr explicit MustBe(T value);
	constexpr MustBe(const MustBe& other) = default;
	constexpr MustBe(MustBe&& other) = default;
	~MustBe() = default;

	constexpr MustBe&			operator=(const MustBe& other) = default;
	constexpr MustBe&			operator=(MustBe&& other) = default;

	constexpr bool				operator==(const MustBe& other) const;
	constexpr bool				operator!=(const MustBe& other) const;
	constexpr bool				operator<(const MustBe& other) const;
	constexpr bool				operator<=(const MustBe& other) const;
	constexpr bool				operator>(const MustBe& other) const;
	constexpr bool				operator>=(const MustBe& other) const;

	constexpr void				setValue(T value);
    constexpr const T&			getValue() const;

private:
    T							m_value;

};

template <typename T>
class None {
public:
	static constexpr std::string_view LIMIT_TYPE = "NONE";

	constexpr None() = default;
	constexpr None(const None& other) = default;
	constexpr None(None&& other) = default;
	~None() = default;

	constexpr None&				operator=(const None& other) = default;
	constexpr None&				operator=(None&& other) = default;

	constexpr bool				operator==(const None& other) const;
	constexpr bool				operator!=(const None& other) const;
	constexpr bool				operator<(const None& other) const;
	constexpr bool				operator<=(const None& other) const;
	constexpr bool				operator>(const None& other) const;
	constexpr bool				operator>=(const None& other) const;

};

template <typename T>
using Limit = std::variant<	None<T>,
							MustBe<T>,
							Discrete<T>,
							DiscreteRange<T>,
							Range<T>,
							Any<T> >;

template <typename T>
constexpr bool hasValue(const Any<T>& any);
template <typename T>
constexpr bool hasValue(const Range<T>& range);
template <typename T>
constexpr bool hasValue(const DiscreteRange<T>& discreteRange);
template <typename T>
constexpr bool hasValue(const Discrete<T>& discrete);
template <typename T>
constexpr bool hasValue(const MustBe<T>& mustBe);
template <typename T>
constexpr bool hasValue(const None<T>& none);
template <typename T>
constexpr bool hasValue(const Limit<T>& limit);

constexpr uint8_t lowest(const Any<uint8_t>& any);
constexpr int8_t lowest(const Any<int8_t>& any);
constexpr uint16_t lowest(const Any<uint16_t>& any);
constexpr int16_t lowest(const Any<int16_t>& any);
constexpr uint32_t lowest(const Any<uint32_t>& any);
constexpr int32_t lowest(const Any<int32_t>& any);
constexpr uint64_t lowest(const Any<uint64_t>& any);
constexpr int64_t lowest(const Any<int64_t>& any);
constexpr float lowest(const Any<float>& any);
constexpr double lowest(const Any<double>& any);
template <typename T>
constexpr T lowest(const Range<T>& range);
template <typename T>
constexpr T lowest(const DiscreteRange<T>& discreteRange);
template <typename T>
constexpr T lowest(const Discrete<T>& discrete);
template <typename T>
constexpr T lowest(const MustBe<T>& mustBe);
template <typename T>
constexpr T lowest(const None<T>& none);
template <typename T>
constexpr T lowest(const Limit<T>& limit);

constexpr uint8_t highest(const Any<uint8_t>& any);
constexpr int8_t highest(const Any<int8_t>& any);
constexpr uint16_t highest(const Any<uint16_t>& any);
constexpr int16_t highest(const Any<int16_t>& any);
constexpr uint32_t highest(const Any<uint32_t>& any);
constexpr int32_t highest(const Any<int32_t>& any);
constexpr uint64_t highest(const Any<uint64_t>& any);
constexpr int64_t highest(const Any<int64_t>& any);
constexpr float highest(const Any<float>& any);
constexpr double highest(const Any<double>& any);
template <typename T>
constexpr T highest(const Range<T>& range);
template <typename T>
constexpr T highest(const DiscreteRange<T>& discreteRange);
template <typename T>
constexpr T highest(const Discrete<T>& discrete);
template <typename T>
constexpr T highest(const MustBe<T>& mustBe);
template <typename T>
constexpr T highest(const None<T>& none);
template <typename T>
constexpr T highest(const Limit<T>& limit);


template <typename T>
constexpr T value(const Any<T>& any);
template <typename T>
constexpr T value(const Range<T>& range);
template <typename T>
constexpr T value(const DiscreteRange<T>& discreteRange);
template <typename T>
constexpr T value(const Discrete<T>& discrete);
template <typename T>
constexpr T value(const MustBe<T>& mustBe);
template <typename T>
constexpr T value(const None<T>& none);
template <typename T>
constexpr T value(const Limit<T>& limit);

template <typename T>
constexpr bool validate(const Any<T>& any, const T& value);
template <typename T>
constexpr bool validate(const Range<T>& range, const T& value);
template <typename T>
constexpr bool validate(const DiscreteRange<T>& discreteRange, const T& value);
template <typename T>
constexpr bool validate(const Discrete<T>& discrete, const T& value);
template <typename T>
constexpr bool validate(const MustBe<T>& mustBe, const T& value);
template <typename T>
constexpr bool validate(const None<T>& none, const T& value);
template <typename T>
constexpr bool validate(const Limit<T>& limit, const T& value);

template <typename T>
constexpr Limit<T> simplify(const Any<T>& any);
template <typename T>
constexpr Limit<T> simplify(const Range<T>& range);
template <typename T>
constexpr Limit<T> simplify(const DiscreteRange<T>& discreteRange);
template <typename T>
constexpr Limit<T> simplify(const Discrete<T>& discrete);
template <typename T>
constexpr Limit<T> simplify(const MustBe<T>& mustBe);
template <typename T>
constexpr Limit<T> simplify(const None<T>& none);
template <typename T>
constexpr Limit<T> simplify(const Limit<T>& limit);

template <typename T>
constexpr Limit<T> intersection(const Any<T>& any1, const Any<T>& any2);
template <typename T>
constexpr Limit<T> intersection(const Any<T>& any1, const Range<T>& range2);
template <typename T>
constexpr Limit<T> intersection(const Any<T>& any1, const DiscreteRange<T>& discreteRange2);
template <typename T>
constexpr Limit<T> intersection(const Any<T>& any1, const Discrete<T>& discrete2);
template <typename T>
constexpr Limit<T> intersection(const Any<T>& any1, const MustBe<T>& mustBe2);
template <typename T>
constexpr Limit<T> intersection(const Any<T>& any1, const None<T>& none2);
template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const Any<T>& any2);
template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const Range<T>& range2);
template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const DiscreteRange<T>& discreteRange2);
template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const Discrete<T>& discrete2);
template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const MustBe<T>& mustBe2);
template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const None<T>& none2);
template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>& discreteRange1, const Any<T>& any2);
template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>& discreteRange1, const Range<T>& range2);
template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>& discreteRange1, const DiscreteRange<T>& discreteRange2);
template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>& discreteRange1, const Discrete<T>& discrete2);
template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>& discreteRange1, const MustBe<T>& mustBe2);
template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>& discreteRange1, const None<T>& none2);
template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const Any<T>& any2);
template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const Range<T>& range2);
template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const DiscreteRange<T>& discreteRange2);
template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const Discrete<T>& discrete2);
template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const MustBe<T>& mustBe2);
template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const None<T>& none2);
template <typename T>
constexpr Limit<T> intersection(const MustBe<T>& mustBe1, const Any<T>& any2);
template <typename T>
constexpr Limit<T> intersection(const MustBe<T>& mustBe1, const Range<T>& range2);
template <typename T>
constexpr Limit<T> intersection(const MustBe<T>& mustBe1, const DiscreteRange<T>& discreteRange2);
template <typename T>
constexpr Limit<T> intersection(const MustBe<T>& mustBe1, const Discrete<T>& discrete2);
template <typename T>
constexpr Limit<T> intersection(const MustBe<T>& mustBe1, const MustBe<T>& mustBe2);
template <typename T>
constexpr Limit<T> intersection(const MustBe<T>& mustBe1, const None<T>& none2);
template <typename T>
constexpr Limit<T> intersection(const None<T>& none1, const Any<T>& any2);
template <typename T>
constexpr Limit<T> intersection(const None<T>& none1, const Range<T>& range2);
template <typename T>
constexpr Limit<T> intersection(const None<T>& none1, const DiscreteRange<T>& discreteRange2);
template <typename T>
constexpr Limit<T> intersection(const None<T>& none1, const Discrete<T>& discrete2);
template <typename T>
constexpr Limit<T> intersection(const None<T>& none1, const MustBe<T>& mustBe2);
template <typename T>
constexpr Limit<T> intersection(const None<T>& none1, const None<T>& none2);
template <typename T>
constexpr Limit<T> intersection(const Limit<T>& limit1, const Limit<T>& limit2);

}

namespace Zuazo {

template <typename T>
std::ostream& operator<<(std::ostream& os, const Utils::Any<T>& any);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Utils::Range<T>& range);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Utils::DiscreteRange<T>& discreteRange);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Utils::Discrete<T>& discrete);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Utils::MustBe<T>& mustBe);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Utils::None<T>& none);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Utils::Limit<T>& limit);

}

#include "Limit.inl"
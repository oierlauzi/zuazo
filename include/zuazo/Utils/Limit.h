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

	constexpr int				operator==(const Any& other) const;
	constexpr int				operator!=(const Any& other) const;
	constexpr int				operator<(const Any& other) const;
	constexpr int				operator<=(const Any& other) const;
	constexpr int				operator>(const Any& other) const;
	constexpr int				operator>=(const Any& other) const;
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

	constexpr int				operator==(const Range& other) const;
	constexpr int				operator!=(const Range& other) const;
	constexpr int				operator<(const Range& other) const;
	constexpr int				operator<=(const Range& other) const;
	constexpr int				operator>(const Range& other) const;
	constexpr int				operator>=(const Range& other) const;

	constexpr void				setMin(T min);
    constexpr const T&			getMin() const;

	constexpr void				setMax(T max);
    constexpr const T&			getMax() const;

private:
	T							m_min, m_max;

};

template <typename T>
class Discrete 
	: public std::vector<T> 
{
public:
	static constexpr std::string_view LIMIT_TYPE = "DISCRETE";

	using std::vector<T>::vector;
	using std::vector<T>::operator=;

	constexpr int				operator==(const Discrete& other) const;
	constexpr int				operator!=(const Discrete& other) const;
	constexpr int				operator<(const Discrete& other) const;
	constexpr int				operator<=(const Discrete& other) const;
	constexpr int				operator>(const Discrete& other) const;
	constexpr int				operator>=(const Discrete& other) const;
};

template <typename T>
class MustBe {
public:
	static constexpr std::string_view LIMIT_TYPE = "MUSTBE";

	constexpr MustBe() = default;
	constexpr explicit MustBe(T value);
	constexpr MustBe(const MustBe& other) = default;
	constexpr MustBe(MustBe&& other) = default;
	~MustBe() = default;

	constexpr MustBe&			operator=(const MustBe& other) = default;
	constexpr MustBe&			operator=(MustBe&& other) = default;

	constexpr int				operator==(const MustBe& other) const;
	constexpr int				operator!=(const MustBe& other) const;
	constexpr int				operator<(const MustBe& other) const;
	constexpr int				operator<=(const MustBe& other) const;
	constexpr int				operator>(const MustBe& other) const;
	constexpr int				operator>=(const MustBe& other) const;

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

	constexpr int				operator==(const None& other) const;
	constexpr int				operator!=(const None& other) const;
	constexpr int				operator<(const None& other) const;
	constexpr int				operator<=(const None& other) const;
	constexpr int				operator>(const None& other) const;
	constexpr int				operator>=(const None& other) const;

};

template <typename T>
using Limit = std::variant< None<T>,
							MustBe<T>,
							Discrete<T>,
							Range<T>,
							Any<T> >;

enum LimitIndices {
	LIMIT_TYPE_NONE_INDEX,
	LIMIT_TYPE_MUSTBE_INDEX,
	LIMIT_TYPE_DISCRETE_INDEX,
	LIMIT_TYPE_RANGE_INDEX,
	LIMIT_TYPE_ANY_INDEX
};


template <typename T>
constexpr T lowest(const Any<T>& any);
template <typename T>
constexpr T lowest(const Range<T>& range);
template <typename T>
constexpr T lowest(const Discrete<T>& discrete);
template <typename T>
constexpr T lowest(const MustBe<T>& mustBe);
template <typename T>
constexpr T lowest(const None<T>& none);
template <typename T>
constexpr T lowest(const Limit<T>& limit);

template <typename T>
constexpr T highest(const Any<T>& any);
template <typename T>
constexpr T highest(const Range<T>& range);
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
constexpr Limit<T> intersection(const Range<T>& range1, const Discrete<T>& discrete2);
template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const MustBe<T>& mustBe2);
template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const None<T>& none2);
template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const Any<T>& any2);
template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const Range<T>& range2);
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
std::ostream& operator<<(std::ostream& os, const Utils::Discrete<T>& discrete);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Utils::MustBe<T>& mustBe);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Utils::None<T>& none);
template <typename T>
std::ostream& operator<<(std::ostream& os, const Utils::Limit<T>& limit);

}

#include "Limit.inl"
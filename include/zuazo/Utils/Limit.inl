#include "Limit.h"

#include "../Math/Absolute.h"
#include "../Math/Comparisons.h"
#include "../Math/Factor.h"
#include "../Exception.h"
#include "../StringConversions.h"

#include <algorithm>
#include <type_traits>
#include <cassert>

namespace Zuazo::Utils {

inline const Exception NO_VALUE_EXCEPTION("No value");

/*
 * Any
 */

template <typename T>
constexpr bool Any<T>::operator==(const Any&) const {
	return std::tie() == std::tie();
}

template <typename T>
constexpr bool Any<T>::operator!=(const Any&) const {
	return std::tie() != std::tie();
}

template <typename T>
constexpr bool Any<T>::operator<(const Any&) const {
	return std::tie() < std::tie();
}   

template <typename T>
constexpr bool Any<T>::operator<=(const Any&) const {
	return std::tie() <= std::tie();
}

template <typename T>
constexpr bool Any<T>::operator>(const Any&) const {
	return std::tie() > std::tie();
}

template <typename T>
constexpr bool Any<T>::operator>=(const Any&) const {
	return std::tie() >= std::tie();
}



template <typename T>
constexpr T Any<T>::lowest() const {
	return T(); //TODO
}


template <typename T>
constexpr T Any<T>::highest() const {
	return T(); //TODO
}

template <typename T>
constexpr T Any<T>::value() const {
	return lowest();
}


template <typename T>
constexpr bool Any<T>::hasValue() const {
	return true;
}

template <typename T>
constexpr bool Any<T>::validate(const T&) const {
	return true;
}

template <typename T>
constexpr const Any<T>& Any<T>::simplify() const {
	return *this;
}



template <typename T>
constexpr const Any<T>& Any<T>::intersect(const Any<T>& other) const {
	return other;
}

template <typename T>
constexpr const Range<T>& Any<T>::intersect(const Range<T>& other) const {
	return other;
}

template <typename T>
constexpr const DiscreteRange<T>& Any<T>::intersect(const DiscreteRange<T>& other) const {
	return other;
}

template <typename T>
constexpr const Discrete<T>& Any<T>::intersect(const Discrete<T>& other) const {
	return other;
}

template <typename T>
constexpr const MustBe<T>& Any<T>::intersect(const MustBe<T>& other) const {
	return other;
}

template <typename T>
constexpr const None<T>& Any<T>::intersect(const None<T>& other) const {
	return other;
}


/*
 * Range
 */

template <typename T>
constexpr Range<T>::Range(T min, T max)
    : m_min(std::move(min))
    , m_max(std::move(max))
{
}


template <typename T>
constexpr bool Range<T>::operator==(const Range& other) const {
	return std::tie(m_min, m_max) == std::tie(other.m_min, other.m_max);
}

template <typename T>
constexpr bool Range<T>::operator!=(const Range& other) const {
	return std::tie(m_min, m_max) != std::tie(other.m_min, other.m_max);
}

template <typename T>
constexpr bool Range<T>::operator<(const Range& other) const {
	return std::tie(m_min, m_max) < std::tie(other.m_min, other.m_max);
}   

template <typename T>
constexpr bool Range<T>::operator<=(const Range& other) const {
	return std::tie(m_min, m_max) <= std::tie(other.m_min, other.m_max);
}

template <typename T>
constexpr bool Range<T>::operator>(const Range& other) const {
	return std::tie(m_min, m_max) > std::tie(other.m_min, other.m_max);
}

template <typename T>
constexpr bool Range<T>::operator>=(const Range& other) const {
	return std::tie(m_min, m_max) >= std::tie(other.m_min, other.m_max);
}


template <typename T>
constexpr void Range<T>::setMin(T min) {
    m_min = std::move(min);
}

template <typename T>
constexpr const T& Range<T>::getMin() const {
    return m_min;
}

template <typename T>
constexpr void Range<T>::setMax(T max) {
    m_max = std::move(max);
}

template <typename T>
constexpr const T& Range<T>::getMax() const {
    return m_max;
}



template <typename T>
constexpr const T& Range<T>::lowest() const {
	if(!hasValue()) throw NO_VALUE_EXCEPTION;
	return getMin();
}

template <typename T>
constexpr const T& Range<T>::highest() const {
	if(!hasValue()) throw NO_VALUE_EXCEPTION;
	return getMax();
}

template <typename T>
constexpr const T& Range<T>::value() const {
	return lowest();
}



template <typename T>
constexpr bool Range<T>::hasValue() const {
	return getMin() <= getMax();
}

template <typename T>
constexpr bool Range<T>::validate(const T& value) const {
	return Math::isInRange(value, getMin(), getMax());
}

template <typename T>
constexpr Limit<T> Range<T>::simplify() const {
	if(!hasValue()) {
		return None<T>(); //No value
	} else if(getMin() == getMax()) {
		return MustBe<T>(getMin()); //Single value;
	} else {
		return *this;
	}
}



template <typename T>
constexpr const Range<T>& Range<T>::intersect(const Any<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr Limit<T> Range<T>::intersect(const Range<T>& other) const {
    const Range<T> result (
        Math::max(getMin(), other.getMin()),
        Math::min(getMax(), other.getMax())
	);

    return result.simplify(); //It may be in a invalid state
}

template <typename T>
constexpr Limit<T> Range<T>::intersect(const DiscreteRange<T>& other) const {
   const DiscreteRange<T> result(
		other.upperBound(getMin()),
		other.lowerBound(getMax()),
		other.getStep()
	);

	return result.simplify(); //It may be in a invalid state
}

template <typename T>
inline Limit<T> Range<T>::intersect(const Discrete<T>& other) const {
    Discrete<T> result;
    result.reserve(other.size()); //Worst case scenario

    //Copy each element only if it is valid within the range
	std::copy_if(
		other.cbegin(), other.cend(),
		std::back_inserter(result),
		[this] (const T& value) -> bool {
			return this->validate(value);
		}
	);

    return result.simplify(); //It may be in a invalid state  
}

template <typename T>
constexpr Limit<T> Range<T>::intersect(const MustBe<T>& other) const {
	if (validate(other.getValue())) {
		return other;
	} else {
		return None<T>();
	}
}

template <typename T>
constexpr const None<T>& Range<T>::intersect(const None<T>& other) const {
	return other;
}



/*
 * DiscreteRange
 */

template <typename T>
constexpr DiscreteRange<T>::DiscreteRange(T min, T max, T step)
	: m_min(std::move(min))
	, m_max(std::move(max))
	, m_step(std::move(step))
{
}


template <typename T>
constexpr bool DiscreteRange<T>::operator==(const DiscreteRange& other) const {
	return std::tie(m_min, m_max, m_step) == std::tie(other.m_min, other.m_max, other.m_step);
}

template <typename T>
constexpr bool DiscreteRange<T>::operator!=(const DiscreteRange& other) const {
	return std::tie(m_min, m_max, m_step) != std::tie(other.m_min, other.m_max, other.m_step);
}

template <typename T>
constexpr bool DiscreteRange<T>::operator<(const DiscreteRange& other) const {
	return std::tie(m_min, m_max, m_step) < std::tie(other.m_min, other.m_max, other.m_step);
}   

template <typename T>
constexpr bool DiscreteRange<T>::operator<=(const DiscreteRange& other) const {
	return std::tie(m_min, m_max, m_step) <= std::tie(other.m_min, other.m_max, other.m_step);
}

template <typename T>
constexpr bool DiscreteRange<T>::operator>(const DiscreteRange& other) const {
	return std::tie(m_min, m_max, m_step) > std::tie(other.m_min, other.m_max, other.m_step);
}

template <typename T>
constexpr bool DiscreteRange<T>::operator>=(const DiscreteRange& other) const {
	return std::tie(m_min, m_max, m_step) >= std::tie(other.m_min, other.m_max, other.m_step);
}



template <typename T>
constexpr void DiscreteRange<T>::setMin(T min) {
    m_min = std::move(min);
}

template <typename T>
constexpr const T& DiscreteRange<T>::getMin() const {
	return m_min;
}


template <typename T>
constexpr void DiscreteRange<T>::setMax(T max) {
    m_max = std::move(max);
}

template <typename T>
constexpr const T& DiscreteRange<T>::getMax() const {
	return m_max;
}


template <typename T>
constexpr void DiscreteRange<T>::setStep(T step) {
    m_step = std::move(step);
}

template <typename T>
constexpr const T& DiscreteRange<T>::getStep() const {
    return m_step;
}



template <typename T>
constexpr T DiscreteRange<T>::lowerBound(T value) const {
	value = Math::max(value, getMin());
	return value - Math::mod(value - getMin(), getStep()); //Return the closest value not greater than value
}

template <typename T>
constexpr T DiscreteRange<T>::upperBound(T value) const {
	value = Math::min(value, getMax());
	return value + Math::mod(getMax() - value, getStep()); //Return the closest value not less than value
}

template <typename T>
constexpr size_t DiscreteRange<T>::size() const {
	if(hasValue()) {
		return (getMax() - getMin()) / getStep() + 1;
	} else {
		return 0;
	}
}



template <typename T>
constexpr const T& DiscreteRange<T>::lowest() const {
	if(!hasValue()) throw NO_VALUE_EXCEPTION;
	return getMin();
}

template <typename T>
constexpr const T& DiscreteRange<T>::highest() const {
	if(!hasValue()) throw NO_VALUE_EXCEPTION;
	return getMax();
}

template <typename T>
constexpr const T& DiscreteRange<T>::value() const {
	return lowest();
}



template <typename T>
constexpr bool DiscreteRange<T>::hasValue() const {
	return getMax() >= getMin();
}

template <typename T>
constexpr bool DiscreteRange<T>::validate(const T& value) const {
	//Validate as if it was a normal range
	if(!Math::isInRange(value, getMin(), getMax())) {
		return false;
	}

	//Ensure it is one of the discrete values
	if(static_cast<bool>(getStep())) {
		return Math::mod(value - getMin(), getStep()) == T();
	} else {
		return true;
	}
}

template <typename T>
constexpr Limit<T> DiscreteRange<T>::simplify() const {
	if(!hasValue()) {
		return None<T>(); //No value
	} else if(getMin() == getMax()) {
		return MustBe<T>(getMin()); //Single value;
	} else if(getStep() == T()) {
		return Range<T>(getMin(), getMax()); //Continuos range
	} else {
		return *this;
	}
}



template <typename T>
constexpr const DiscreteRange<T>& DiscreteRange<T>::intersect(const Any<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr Limit<T> DiscreteRange<T>::intersect(const Range<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr Limit<T> DiscreteRange<T>::intersect(const DiscreteRange<T>& other) const {
	//Elaborate a temporary discrete range
	DiscreteRange<T> result (
        Math::max(getMin(), other.getMin()),
        Math::min(getMax(), other.getMax()),
		Math::lcm(getStep(), other.getStep())
	);

	//When the value returned by upperBound becomes invariant, it means that it is coincident with the step.
	//Therefore, if the upperBound of both discreteRanges for is the same, it means that that value 
	//is part of both ranges. Repeating this process until this condition is met will result in the first value
	//that is part of both DiscreteRange-s. However, this condition might never be met. In order to stop an 
	//endless loop, also check that the range remains valid. Each failed iteration the minimum value will be raised.
	while(result.getMin() <= result.getMax()) {
		const auto oldValue = result.getMin();
		const auto newValue = upperBound(other.upperBound(oldValue));

		result.setMin(newValue);
		if(oldValue == newValue) break; //Got it right
	}

	//Same as above but with lowerBound. Each failed iteration the maximum value will be lowered.
	while(result.getMin() <= result.getMax()) {
		const auto oldValue = result.getMax();
		const auto newValue = lowerBound(other.lowerBound(oldValue));

		result.setMax(newValue);
		if(oldValue == newValue) break; //Got it right
	}

	return result.simplify(); //It may be in a invalid state   
}

template <typename T>
inline Limit<T> DiscreteRange<T>::intersect(const Discrete<T>& other) const {
	Discrete<T> result;
    result.reserve(other.size()); //Worst case scenario

    //Copy each element only if it is valid within the range
	std::copy_if(
		other.cbegin(), other.cend(),
		std::back_inserter(result),
		[this] (const T& value) -> bool {
			return this->validate(value);
		}
	);

    return result.simplify(); //It may be in a invalid state  
}

template <typename T>
constexpr Limit<T> DiscreteRange<T>::intersect(const MustBe<T>& other) const {
	if (validate(other.getValue())) {
		return other;
	} else {
		return None<T>();
	}
}

template <typename T>
constexpr const None<T>& DiscreteRange<T>::intersect(const None<T>& other) const {
	return other;
}



/*
 * Discrete
 */

template <typename T>
constexpr bool Discrete<T>::operator==(const Discrete& other) const {
	return std::equal(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend()
	);
}

template <typename T>
constexpr bool Discrete<T>::operator!=(const Discrete& other) const {
	return !operator==(other);
}

template <typename T>
constexpr bool Discrete<T>::operator<(const Discrete& other) const {
	return std::lexicographical_compare(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend(),
		std::less<T>()
	);
}   

template <typename T>
constexpr bool Discrete<T>::operator<=(const Discrete& other) const {
	return std::lexicographical_compare(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend(),
		std::less_equal<T>()
	);
}

template <typename T>
constexpr bool Discrete<T>::operator>(const Discrete& other) const {
	return std::lexicographical_compare(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend(),
		std::greater<T>()
	);
}

template <typename T>
constexpr bool Discrete<T>::operator>=(const Discrete& other) const {
	return std::lexicographical_compare(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend(),
		std::greater_equal<T>()
	);
}



template <typename T>
constexpr const T& Discrete<T>::lowest() const {
	if(!hasValue()) throw NO_VALUE_EXCEPTION;
	return std::vector<T>::front();
}

template <typename T>
constexpr const T& Discrete<T>::highest() const {
	if(!hasValue()) throw NO_VALUE_EXCEPTION;
	return std::vector<T>::back();
}

template <typename T>
constexpr const T& Discrete<T>::value() const {
	return lowest();
}



template <typename T>
constexpr bool Discrete<T>::hasValue() const {
	return std::vector<T>::size() > 0;
}

template <typename T>
constexpr bool Discrete<T>::validate(const T& value) const {
    assert(std::is_sorted(std::vector<T>::cbegin(), std::vector<T>::cend()));
    return std::binary_search(std::vector<T>::cbegin(), std::vector<T>::cend(), value);
}

template <typename T>
constexpr Limit<T> Discrete<T>::simplify() const {
    switch(std::vector<T>::size()) {
        case 0:		return None<T>();							//Invalid state
        case 1:		return MustBe<T>(std::vector<T>::front());	//Single value represented
        default:	return *this;								//All OK
    }
}



template <typename T>
constexpr const Discrete<T>& Discrete<T>::intersect(const Any<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr Limit<T> Discrete<T>::intersect(const Range<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr Limit<T> Discrete<T>::intersect(const DiscreteRange<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
inline Limit<T> Discrete<T>::intersect(const Discrete<T>& other) const {
	Discrete<T> result;
	result.reserve(Math::min(std::vector<T>::size(), other.size())); //Worst case scenario

	//In order to use set_intersect:
	assert(std::is_sorted(std::vector<T>::cbegin(), std::vector<T>::cend()));
	assert(std::is_sorted(other.cbegin(), other.cend()));

	std::set_intersection(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend(),
		std::back_inserter(result)
	);

	return result.simplify(); //It may be in a invalid state 
}

template <typename T>
constexpr Limit<T> Discrete<T>::intersect(const MustBe<T>& other) const {
	if (validate(other.getValue())) {
		return other;
	} else {
		return None<T>();
	}
}

template <typename T>
constexpr const None<T>& Discrete<T>::intersect(const None<T>& other) const {
	return other;
}



/*
 * MustBe
 */

template <typename T>
constexpr MustBe<T>::MustBe(T value)
    : m_value(std::move(value))
{
}


template <typename T>
constexpr bool MustBe<T>::operator==(const MustBe& other) const {
	return m_value == other.m_value;
}

template <typename T>
constexpr bool MustBe<T>::operator!=(const MustBe& other) const {
	return m_value != other.m_value;
}

template <typename T>
constexpr bool MustBe<T>::operator<(const MustBe& other) const {
	return m_value < other.m_value;
}   

template <typename T>
constexpr bool MustBe<T>::operator<=(const MustBe& other) const {
	return m_value <= other.m_value;
}

template <typename T>
constexpr bool MustBe<T>::operator>(const MustBe& other) const {
	return m_value > other.m_value;
}

template <typename T>
constexpr bool MustBe<T>::operator>=(const MustBe& other) const {
	return m_value >= other.m_value;
}


template <typename T>
constexpr void MustBe<T>::setValue(T value) {
    m_value = std::move(value);
}

template <typename T>
constexpr const T& MustBe<T>::getValue() const {
    return m_value;
}



template <typename T>
constexpr const T& MustBe<T>::lowest() const {
	return getValue();
}

template <typename T>
constexpr const T& MustBe<T>::highest() const {
	return getValue();
}

template <typename T>
constexpr const T& MustBe<T>::value() const {
	return lowest();
}



template <typename T>
constexpr bool MustBe<T>::hasValue() const {
	return true;
}

template <typename T>
constexpr bool MustBe<T>::validate(const T& value) const {
	return getValue() == value; 
}

template <typename T>
constexpr MustBe<T> MustBe<T>::simplify() const {
	return *this;
}



template <typename T>
constexpr const MustBe<T>& MustBe<T>::intersect(const Any<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr Limit<T> MustBe<T>::intersect(const Range<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr Limit<T> MustBe<T>::intersect(const DiscreteRange<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
inline Limit<T> MustBe<T>::intersect(const Discrete<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr Limit<T> MustBe<T>::intersect(const MustBe<T>& other) const {
	if (validate(other.getValue())) {
		return other;
	} else {
		return None<T>();
	}
}

template <typename T>
constexpr const None<T>& MustBe<T>::intersect(const None<T>& other) const {
	return other;
}



/*
 * None
 */

template <typename T>
constexpr bool None<T>::operator==(const None&) const {
	return std::tie() == std::tie();
}

template <typename T>
constexpr bool None<T>::operator!=(const None&) const {
	return std::tie() != std::tie();
}

template <typename T>
constexpr bool None<T>::operator<(const None&) const {
	return std::tie() < std::tie();
}   

template <typename T>
constexpr bool None<T>::operator<=(const None&) const {
	return std::tie() <= std::tie();
}

template <typename T>
constexpr bool None<T>::operator>(const None&) const {
	return std::tie() > std::tie();
}

template <typename T>
constexpr bool None<T>::operator>=(const None&) const {
	return std::tie() >= std::tie();
}



template <typename T>
constexpr const T& None<T>::lowest() const {
	throw NO_VALUE_EXCEPTION;
}

template <typename T>
constexpr const T& None<T>::highest() const {
	throw NO_VALUE_EXCEPTION;
}

template <typename T>
constexpr const T& None<T>::value() const {
	return lowest();
}



template <typename T>
constexpr bool None<T>::hasValue() const {
	return false;
}

template <typename T>
constexpr bool None<T>::validate(const T&) const {
	return false;
}

template <typename T>
constexpr const None<T>& None<T>::simplify() const {
	return *this;
}



template <typename T>
constexpr const None<T>& None<T>::intersect(const Any<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr const None<T>& None<T>::intersect(const Range<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr const None<T>& None<T>::intersect(const DiscreteRange<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr const None<T>& None<T>::intersect(const Discrete<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr const None<T>& None<T>::intersect(const MustBe<T>& other) const {
	return other.intersect(*this); //Already implemented
}

template <typename T>
constexpr const None<T>& None<T>::intersect(const None<T>& other) const {
	return other;
}



/*
 * Limit
 */

template <typename T>
constexpr Limit<T>::Limit(None<T> none)
	: m_data(std::move(none))
{
}

template <typename T>
constexpr Limit<T>::Limit(MustBe<T> mustBe)
	: m_data(std::move(mustBe))
{
}

template <typename T>
constexpr Limit<T>::Limit(Discrete<T> discrete)
	: m_data(std::move(discrete))
{
}

template <typename T>
constexpr Limit<T>::Limit(DiscreteRange<T> discreteRange)
	: m_data(std::move(discreteRange))
{
}

template <typename T>
constexpr Limit<T>::Limit(Range<T> range)
	: m_data(std::move(range))
{
}

template <typename T>
constexpr Limit<T>::Limit(Any<T> any)
	: m_data(std::move(any))
{
}



template <typename T>
constexpr Limit<T>& Limit<T>::operator=(None<T> none) {
	m_data = std::move(none);
	return *this;
}

template <typename T>
constexpr Limit<T>& Limit<T>::operator=(MustBe<T> mustBe) {
	m_data = std::move(mustBe);
	return *this;
}

template <typename T>
constexpr Limit<T>& Limit<T>::operator=(Discrete<T> discrete) {
	m_data = std::move(discrete);
	return *this;
}

template <typename T>
constexpr Limit<T>& Limit<T>::operator=(DiscreteRange<T> discreteRange) {
	m_data = std::move(discreteRange);
	return *this;
}

template <typename T>
constexpr Limit<T>& Limit<T>::operator=(Range<T> range) {
	m_data = std::move(range);
	return *this;
}

template <typename T>
constexpr Limit<T>& Limit<T>::operator=(Any<T> any) {
	m_data = std::move(any);
	return *this;
}



template <typename T>
constexpr bool Limit<T>::operator==(const Limit& other) const {
	return m_data == other.m_data;
}

template <typename T>
constexpr bool Limit<T>::operator!=(const Limit& other) const {
	return m_data != other.m_data;
}

template <typename T>
constexpr bool Limit<T>::operator<(const Limit& other) const {
	return m_data < other.m_data;
}   

template <typename T>
constexpr bool Limit<T>::operator<=(const Limit& other) const {
	return m_data <= other.m_data;
}

template <typename T>
constexpr bool Limit<T>::operator>(const Limit& other) const {
	return m_data > other.m_data;
}

template <typename T>
constexpr bool Limit<T>::operator>=(const Limit& other) const {
	return m_data >= other.m_data;
}


template <typename T>
constexpr Limit<T>::operator bool() const {
	return hasValue();
}


template <typename T>
constexpr LimitType Limit<T>::getType() const {
	static_assert(std::is_same<typename std::variant_alternative<static_cast<int>(LimitType::NONE), Data>::type, 			None<T>>::value, "None index does not match");
	static_assert(std::is_same<typename std::variant_alternative<static_cast<int>(LimitType::MUST_BE), Data>::type, 		MustBe<T>>::value, "MustBe index does not match");
	static_assert(std::is_same<typename std::variant_alternative<static_cast<int>(LimitType::DISCRETE), Data>::type, 		Discrete<T>>::value, "Discrete index does not match");
	static_assert(std::is_same<typename std::variant_alternative<static_cast<int>(LimitType::DISCRETE_RANGE), Data>::type, 	DiscreteRange<T>>::value, "DiscreteRange index does not match");
	static_assert(std::is_same<typename std::variant_alternative<static_cast<int>(LimitType::RANGE), Data>::type, 			Range<T>>::value, "Range index does not match");
	static_assert(std::is_same<typename std::variant_alternative<static_cast<int>(LimitType::ANY), Data>::type, 			Any<T>>::value, "Any index does not match");

	return static_cast<LimitType>(m_data.index());
}

template <typename T> 
constexpr None<T>& Limit<T>::getNone() {
	return std::get<None<T>>(m_data);
}

template <typename T> 
constexpr const None<T>& Limit<T>::getNone() const {
	return std::get<None<T>>(m_data);
}

template <typename T> 
constexpr MustBe<T>& Limit<T>::getMustBe() {
	return std::get<MustBe<T>>(m_data);
}

template <typename T> 
constexpr const MustBe<T>& Limit<T>::getMustBe() const {
	return std::get<MustBe<T>>(m_data);
}

template <typename T> 
constexpr Discrete<T>& Limit<T>::getDiscrete() {
	return std::get<Discrete<T>>(m_data);
}

template <typename T> 
constexpr const Discrete<T>& Limit<T>::getDiscrete() const {
	return std::get<Discrete<T>>(m_data);
}

template <typename T> 
constexpr DiscreteRange<T>& Limit<T>::getDiscreteRange() {
	return std::get<DiscreteRange<T>>(m_data);
}

template <typename T> 
constexpr const DiscreteRange<T>& Limit<T>::getDiscreteRange() const {
	return std::get<DiscreteRange<T>>(m_data);
}

template <typename T> 
constexpr Range<T>& Limit<T>::getRange() {
	return std::get<Range<T>>(m_data);
}

template <typename T> 
constexpr const Range<T>& Limit<T>::getRange() const {
	return std::get<Range<T>>(m_data);
}

template <typename T> 
constexpr Any<T>& Limit<T>::getAny() {
	return std::get<Any<T>>(m_data);
}

template <typename T> 
constexpr const Any<T>& Limit<T>::getAny() const {
	return std::get<Any<T>>(m_data);
}



template <typename T>
constexpr T Limit<T>::lowest() const {
    return std::visit(
        [] (const auto& l) -> T {
            return l.lowest();
        },
        m_data
    );
}

template <typename T>
constexpr T Limit<T>::highest() const {
    return std::visit(
        [] (const auto& l) -> T {
            return l.highest();
        },
        m_data
    );
}

template <typename T>
constexpr T Limit<T>::value() const {
	return lowest();
}



template <typename T>
constexpr bool Limit<T>::hasValue() const {
    return std::visit(
        [] (const auto& l) -> bool {
            return l.hasValue();
        },
        m_data
    );
}

template <typename T>
constexpr bool Limit<T>::validate(const T& value) const {
    return std::visit(
        [&value] (const auto& l) -> bool {
            return l.validate(value);
        },
        m_data
    );
}

template <typename T>
constexpr Limit<T> Limit<T>::simplify() const {
    return std::visit(
        [] (const auto& l) -> Limit<T> {
            return l.simplify();
        },
        m_data
    );
}

template <typename T>
constexpr Limit<T> Limit<T>::intersect(const Limit<T>& other) const {
    return std::visit(
        [] (const auto& l1, const auto& l2) -> Limit<T> {
            return l1.intersect(l2);
        },
        m_data, other.m_data
    );
}



/*
 * operator<< os
 */

inline std::ostream& operator<<(std::ostream& os, LimitType type) {
	return os << toString(type);
}


template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Any<T>&) {
	return Zuazo::operator<<(os, std::forward_as_tuple());
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Range<T>& range) {
	return Zuazo::operator<<(
		os, 
		std::forward_as_tuple(
			std::make_pair("min", std::cref(range.getMin())),
			std::make_pair("max", std::cref(range.getMax()))
		)
	);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const DiscreteRange<T>& discreteRange) {
	return Zuazo::operator<<(
		os, 
		std::forward_as_tuple(
			std::make_pair("min", std::cref(discreteRange.getMin())),
			std::make_pair("max", std::cref(discreteRange.getMax())),
			std::make_pair("step", std::cref(discreteRange.getStep()))
		)
	);
}


template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Discrete<T>& discrete) {
	return toOstream(os, discrete.cbegin(), discrete.cend());
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const MustBe<T>& mustBe) {
	return Zuazo::operator<<(
		os, 
		std::forward_as_tuple(
			std::make_pair("value", std::cref(mustBe.getValue()))
		)
	);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const None<T>&) {
	return Zuazo::operator<<(os, std::forward_as_tuple());
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Limit<T>& limit) {
	const auto type = limit.getType();
	switch(type) {
	case LimitType::NONE:
		return Zuazo::operator<<(
			os, 
			std::forward_as_tuple(
				std::make_pair("type", type),
				std::make_pair("data", std::cref(limit.getNone()))
			)
		);
	case LimitType::MUST_BE:
		return Zuazo::operator<<(
			os, 
			std::forward_as_tuple(
				std::make_pair("type", type),
				std::make_pair("data", std::cref(limit.getMustBe()))
			)
		);
	case LimitType::DISCRETE:
		return Zuazo::operator<<(
			os, 
			std::forward_as_tuple(
				std::make_pair("type", type),
				std::make_pair("data", std::cref(limit.getDiscrete()))
			)
		);
	case LimitType::DISCRETE_RANGE:
		return Zuazo::operator<<(
			os, 
			std::forward_as_tuple(
				std::make_pair("type", type),
				std::make_pair("data", std::cref(limit.getDiscreteRange()))
			)
		);
	case LimitType::RANGE:
		return Zuazo::operator<<(
			os, 
			std::forward_as_tuple(
				std::make_pair("type", type),
				std::make_pair("data", std::cref(limit.getRange()))
			)
		);
	case LimitType::ANY:
		return Zuazo::operator<<(
			os, 
			std::forward_as_tuple(
				std::make_pair("type", type),
				std::make_pair("data", std::cref(limit.getAny()))
			)
		);
	default:
		return os;
	}
}

}

namespace Zuazo {

constexpr std::string_view toString(Utils::LimitType type) noexcept {
	switch(type){

	ZUAZO_ENUM2STR_CASE( Utils::LimitType, NONE )
	ZUAZO_ENUM2STR_CASE( Utils::LimitType, MUST_BE )
	ZUAZO_ENUM2STR_CASE( Utils::LimitType, DISCRETE )
	ZUAZO_ENUM2STR_CASE( Utils::LimitType, DISCRETE_RANGE )
	ZUAZO_ENUM2STR_CASE( Utils::LimitType, RANGE )
	ZUAZO_ENUM2STR_CASE( Utils::LimitType, ANY )

	default: return "";
	}
}

}
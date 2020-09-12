#include "Limit.h"

#include "../Math/Functions.h"
#include "../Exception.h"
#include "../StringConversions.h"

#include <algorithm>
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

/*
 * DiscreteRange
 */

template <typename T>
constexpr DiscreteRange<T>::DiscreteRange(T min, T max, T step)
	: Range<T>(std::move(min), std::move(max))
	, m_step(std::move(step))
{
}


template <typename T>
constexpr bool DiscreteRange<T>::operator==(const DiscreteRange& other) const {
	return std::tie(static_cast<const Range<T>&>(*this), m_step) == std::tie(static_cast<const Range<T>&>(other), m_step);
}

template <typename T>
constexpr bool DiscreteRange<T>::operator!=(const DiscreteRange& other) const {
	return std::tie(static_cast<const Range<T>&>(*this), m_step) != std::tie(static_cast<const Range<T>&>(other), m_step);
}

template <typename T>
constexpr bool DiscreteRange<T>::operator<(const DiscreteRange& other) const {
	return std::tie(static_cast<const Range<T>&>(*this), m_step) < std::tie(static_cast<const Range<T>&>(other), m_step);
}   

template <typename T>
constexpr bool DiscreteRange<T>::operator<=(const DiscreteRange& other) const {
	return std::tie(static_cast<const Range<T>&>(*this), m_step) <= std::tie(static_cast<const Range<T>&>(other), m_step);
}

template <typename T>
constexpr bool DiscreteRange<T>::operator>(const DiscreteRange& other) const {
	return std::tie(static_cast<const Range<T>&>(*this), m_step) > std::tie(static_cast<const Range<T>&>(other), m_step);
}

template <typename T>
constexpr bool DiscreteRange<T>::operator>=(const DiscreteRange& other) const {
	return std::tie(static_cast<const Range<T>&>(*this), m_step) >= std::tie(static_cast<const Range<T>&>(other), m_step);
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
	value = Math::max(value, Range<T>::getMin());
	return value - Math::mod(value - Range<T>::getMin(), getStep()); //Return the closest value not greater than value
}

template <typename T>
constexpr T DiscreteRange<T>::upperBound(T value) const {
	value = Math::min(value, Range<T>::getMax());
	return value + Math::mod(Range<T>::getMax() - value, getStep()); //Return the closest value not less than value
}

template <typename T>
constexpr size_t DiscreteRange<T>::size() const {
	if(Range<T>::getMax() >= Range<T>::getMin()) {
		return (Range<T>::getMax() - Range<T>::getMin()) / getStep() + 1;
	} else {
		return 0;
	}
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



/*
 * Functions
 */

template <typename T>
constexpr bool hasValue(const Any<T>&) {
	return true;
}

template <typename T>
constexpr bool hasValue(const Range<T>& range) {
	return range.getMin() <= range.getMax();
}

template <typename T>
constexpr bool hasValue(const DiscreteRange<T>& discreteRange) {
	return hasValue(static_cast<const Range<T>&>(discreteRange));
}

template <typename T>
constexpr bool hasValue(const Discrete<T>& discrete) {
	return discrete.size() > 0;
}

template <typename T>
constexpr bool hasValue(const MustBe<T>&) {
	return true;
}

template <typename T>
constexpr bool hasValue(const None<T>&) {
	return false;
}

template <typename T>
constexpr bool hasValue(const Limit<T>& limit) {
	return std::visit(
        [] (const auto& l) -> bool {
            return hasValue(l);
        },
        limit
    );
}



constexpr uint8_t lowest(const Any<uint8_t>&) {
	return std::numeric_limits<uint8_t>::min();
}

constexpr int8_t lowest(const Any<int8_t>&) {
	return std::numeric_limits<int8_t>::min();
}

constexpr uint16_t lowest(const Any<uint16_t>&) {
	return std::numeric_limits<uint16_t>::min();
}

constexpr int16_t lowest(const Any<int16_t>&) {
	return std::numeric_limits<int16_t>::min();
}

constexpr uint32_t lowest(const Any<uint32_t>&) {
	return std::numeric_limits<uint32_t>::min();
}

constexpr int32_t lowest(const Any<int32_t>&) {
	return std::numeric_limits<int32_t>::min();
}

constexpr uint64_t lowest(const Any<uint64_t>&) {
	return std::numeric_limits<uint64_t>::min();
}

constexpr int64_t lowest(const Any<int64_t>&) {
	return std::numeric_limits<int64_t>::min();
}

constexpr float lowest(const Any<float>&) {
	return std::numeric_limits<float>::min();
}

constexpr double lowest(const Any<double>&) {
	return std::numeric_limits<double>::min();
}

template <typename T>
constexpr T lowest(const Range<T>& range) {
	if(hasValue(range)) return range.getMin();
	throw NO_VALUE_EXCEPTION;
}

template <typename T>
constexpr T lowest(const DiscreteRange<T>& discreteRange) {
	if(hasValue(discreteRange)) return discreteRange.getMin();
	throw NO_VALUE_EXCEPTION;
}

template <typename T>
constexpr T lowest(const Discrete<T>& discrete) {
    assert(std::is_sorted(discrete.cbegin(), discrete.cend()));
	if(hasValue(discrete)) return discrete.front();
	throw NO_VALUE_EXCEPTION;
}

template <typename T>
constexpr T lowest(const MustBe<T>& mustBe) {
    return mustBe.getValue();
}

template <typename T>
constexpr T lowest(const None<T>&) {
	throw NO_VALUE_EXCEPTION;
}

template <typename T>
constexpr T lowest(const Limit<T>& limit) {
    return std::visit(
        [] (const auto& l) -> T {
            return lowest(l);
        },
		limit
    );
}



constexpr uint8_t highest(const Any<uint8_t>&) {
	return std::numeric_limits<uint8_t>::max();
}

constexpr int8_t highest(const Any<int8_t>&) {
	return std::numeric_limits<int8_t>::max();
}

constexpr uint16_t highest(const Any<uint16_t>&) {
	return std::numeric_limits<uint16_t>::max();
}

constexpr int16_t highest(const Any<int16_t>&) {
	return std::numeric_limits<int16_t>::max();
}

constexpr uint32_t highest(const Any<uint32_t>&) {
	return std::numeric_limits<uint32_t>::max();
}

constexpr int32_t highest(const Any<int32_t>&) {
	return std::numeric_limits<int32_t>::max();
}

constexpr uint64_t highest(const Any<uint64_t>&) {
	return std::numeric_limits<uint64_t>::max();
}

constexpr int64_t highest(const Any<int64_t>&) {
	return std::numeric_limits<int64_t>::max();
}

constexpr float highest(const Any<float>&) {
	return std::numeric_limits<float>::max();
}

constexpr double highest(const Any<double>&) {
	return std::numeric_limits<double>::max();
}


template <typename T>
constexpr T highest(const Range<T>& range) {
	if(hasValue(range)) return range.getMax();
	throw NO_VALUE_EXCEPTION;
}

template <typename T>
constexpr T highest(const DiscreteRange<T>& discreteRange) {
	if(hasValue(discreteRange)) return discreteRange.getMax();
	throw NO_VALUE_EXCEPTION;
}


template <typename T>
constexpr T highest(const Discrete<T>& discrete) {
    assert(std::is_sorted(discrete.cbegin(), discrete.cend()));
	if(hasValue(discrete)) return discrete.back();
	throw NO_VALUE_EXCEPTION;
}

template <typename T>
constexpr T highest(const MustBe<T>& mustBe) {
    return mustBe.getValue();
}

template <typename T>
constexpr T highest(const None<T>&) {
	throw NO_VALUE_EXCEPTION;
}

template <typename T>
constexpr T highest(const Limit<T>& limit) {
    return std::visit(
        [] (const auto& l) -> T {
            return highest(l);
        },
        limit
    );
}

template <typename T>
constexpr T value(const Any<T>& any) {
	return lowest(any);
}

template <typename T>
constexpr T value(const Range<T>& range) {
	return lowest(range);
}

template <typename T>
constexpr T value(const DiscreteRange<T>& discreteRange) {
	return lowest(discreteRange);
}

template <typename T>
constexpr T value(const Discrete<T>& discrete) {
	return lowest(discrete);
}

template <typename T>
constexpr T value(const MustBe<T>& mustBe) {
	return lowest(mustBe);
}

template <typename T>
constexpr T value(const None<T>& none) {
	return lowest(none);
}

template <typename T>
constexpr T value(const Limit<T>& limit) {
    return std::visit(
        [] (const auto& l) -> T {
            return value(l);
        },
        limit
    );
}


template <typename T>
constexpr bool validate(const Any<T>&, const T&) {
    return true;
}

template <typename T>
constexpr bool validate(const Range<T>& range, const T& value) {
    return range.getMin() <= value && value <= range.getMax();
}

template <typename T>
constexpr bool validate(const DiscreteRange<T>& discreteRange, const T& value) {
	//Validate as if it was a normal range
	if(!validate(static_cast<const Range<T>&>(discreteRange), value)) {
		return false;
	}

	//Ensure it is one of the discrete values
	if(static_cast<bool>(discreteRange.getStep())) {
		return Math::mod(value - discreteRange.getMin(), discreteRange.getStep()) == T();
	}
	return true; //All OK
}

template <typename T>
constexpr bool validate(const Discrete<T>& discrete, const T& value) {
    assert(std::is_sorted(discrete.cbegin(), discrete.cend()));
    return std::binary_search(discrete.cbegin(), discrete.cend(), value);
}

template <typename T>
constexpr bool validate(const MustBe<T>& mustBe, const T& value) {
    return mustBe.getValue() == value;
}

template <typename T>
constexpr bool validate(const None<T>&, const T&) {
    return false;
}

template <typename T>
constexpr bool validate(const Limit<T>& limit, const T& value) {
    return std::visit(
        [&value] (const auto& l) -> bool{
            return validate(l, value);
        },
        limit
    );
}


template <typename T>
constexpr Limit<T> simplify(const Any<T>&) {
    return Any<T>();
}

template <typename T>
constexpr Limit<T> simplify(const Range<T>& range) {
    if(range.getMin() > range.getMax())							return None<T>();							//Invalid state
    else if(range.getMin() == range.getMax())					return MustBe<T>{ range.getMin() };			//Single value represented
    else														return range;								//All OK
}

template <typename T>
constexpr Limit<T> simplify(const DiscreteRange<T>& discreteRange) {
	if(discreteRange.getMin() > discreteRange.getMax())			return None<T>();							//Invalid state
    else if(discreteRange.getMin() == discreteRange.getMax())	return MustBe<T>{ discreteRange.getMin() };	//Single value represented
	else if(discreteRange.getStep() == T())						return static_cast<Range<T>>(discreteRange);//Continious range
    else														return discreteRange;						//All OK
}

template <typename T>
constexpr Limit<T> simplify(const Discrete<T>& discrete) {
    switch(discrete.size()) {
        case 0:													return None<T>();							//Invalid state
        case 1:													return MustBe<T>{discrete[0]};				//Single value represented
        default:												return discrete;							//All OK
    }
}

template <typename T>
constexpr Limit<T> simplify(const MustBe<T>& mustBe) {
    return mustBe;
}

template <typename T>
constexpr Limit<T> simplify(const None<T>&) {
     return None<T>();
}

template <typename T>
constexpr Limit<T> simplify(const Limit<T>& limit) {
    return std::visit(
        [] (const auto& l) -> Limit<T> {
            return simplify(l);
        },
        limit
    );
}


template <typename T>
constexpr Limit<T> intersection(const Any<T>&, const Any<T>&) {
    return Any<T>();
}

template <typename T>
constexpr Limit<T> intersection(const Any<T>&, const Range<T>& range2) {
    return range2;
}

template <typename T>
constexpr Limit<T> intersection(const Any<T>&, const DiscreteRange<T>& discreteRange2) {
    return discreteRange2;
}

template <typename T>
constexpr Limit<T> intersection(const Any<T>&, const Discrete<T>& discrete2) {
    return discrete2;
}

template <typename T>
constexpr Limit<T> intersection(const Any<T>&, const MustBe<T>& mustBe2) {
    return mustBe2;
}

template <typename T>
constexpr Limit<T> intersection(const Any<T>&, const None<T>&) {
	return None<T>();
}

template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const Any<T>& any2) {
	return intersection(any2, range1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const Range<T>& range2) {
    const Range<T> result (
        Math::max(range1.getMin(), range2.getMin()),
        Math::min(range1.getMax(), range2.getMax())
	);

    return simplify(result); //It may be in a invalid state
}

template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const DiscreteRange<T>& discreteRange2) {
   const  DiscreteRange<T> result(
		discreteRange2.upperBound(range1.getMin()),
		discreteRange2.lowerBound(range1.getMax()),
		discreteRange2.getStep()
	);

	return simplify(result); //It may be in a invalid state
}

template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const Discrete<T>& discrete2) {
    Discrete<T> result;
    result.reserve(discrete2.size()); //Worst case scenario

    //Copy each element only if it is valid within the range
	std::copy_if(
		discrete2.cbegin(), discrete2.cend(),
		std::back_inserter(result),
		[&range1] (const T& value) -> bool {
			return validate(range1, value);
		}
	);

    return simplify(result); //It may be in a invalid state    
}

template <typename T>
constexpr Limit<T> intersection(const Range<T>& range1, const MustBe<T>& mustBe2) {
    if (validate(range1, mustBe2.getValue())) {
        return mustBe2;
    } else {
        return None<T>();
    }
}

template <typename T>
constexpr Limit<T> intersection(const Range<T>&, const None<T>&) {
    return None<T>();
}

template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>& discreteRange1, const Any<T>& any2) {
	return intersection(any2, discreteRange1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>& discreteRange1, const Range<T>& range2) {
	return intersection(range2, discreteRange1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>& discreteRange1, const DiscreteRange<T>& discreteRange2) {
	//Elaborate a temporary discrete range
	DiscreteRange<T> result (
        Math::max(discreteRange1.getMin(), discreteRange2.getMin()),
        Math::min(discreteRange1.getMax(), discreteRange2.getMax()),
		Math::lcm(discreteRange1.getStep(), discreteRange1.getStep())
	);

	//When the value retuned by upperBound becomes invariant, it means that it is coincident with the step.
	//Therefore, if the upperBound of both discreteRanges for is the same, it means that that value 
	//is part of both ranges. Repeating this process until this condition is met will result in the first value
	//that is part of both DiscreteRange-s. However, this condition might never be met. In order to stop an 
	//endless loop, also check that the range remains valid. Each failed iteration the minimum value will be raised.
	while(result.getMin() <= result.getMax()) {
		const auto oldValue = result.getMin();
		const auto newValue = discreteRange1.upperBound(discreteRange2.upperBound(oldValue));
		assert(newValue >= oldValue);

		result.setMin(newValue);
		if(oldValue == newValue) break; //Got it right
	}

	//Same as above but with lowerBound. Each failed iteration the maximum value will be lowered.
	while(result.getMin() <= result.getMax()) {
		const auto oldValue = result.getMax();
		const auto newValue = discreteRange1.lowerBound(discreteRange2.lowerBound(oldValue));
		assert(newValue <= oldValue);

		result.setMax(newValue);
		if(oldValue == newValue) break; //Got it right
	}

	return simplify(result); //It may be in a invalid state    
}

template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>& discreteRange1, const Discrete<T>& discrete2) {
	Discrete<T> result;
    result.reserve(discrete2.size()); //Worst case scenario

    //Copy each element only if it is valid within the range
	std::copy_if(
		discrete2.cbegin(), discrete2.cend(),
		std::back_inserter(result),
		[&discreteRange1] (const T& value) -> bool {
			return validate(discreteRange1, value);
		}
	);

    return simplify(result); //It may be in a invalid state    
}

template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>& discreteRange1, const MustBe<T>& mustBe2) {
    if (validate(discreteRange1, mustBe2.getValue())) {
        return mustBe2;
    } else {
        return None<T>();
    }
}

template <typename T>
constexpr Limit<T> intersection(const DiscreteRange<T>&, const None<T>&) {
	return None<T>();
}

template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const Any<T>& any2) {
    return intersection(any2, discrete1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const Range<T>& range2) {
    return intersection(range2, discrete1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const DiscreteRange<T>& discreteRange2) {
	return intersection(discreteRange2, discrete1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const Discrete<T>& discrete2) {
	Discrete<T> result;
	result.reserve(Math::min(discrete1.size(), discrete2.size())); //Worst case scenario

	//In order to use set_intersection:
	assert(std::is_sorted(discrete1.cbegin(), discrete1.cend()));
	assert(std::is_sorted(discrete2.cbegin(), discrete2.cend()));

	std::set_intersection(
		discrete1.cbegin(), discrete1.cend(),
		discrete2.cbegin(), discrete2.cend(),
		std::back_inserter(result)
	);

	return simplify(result); //It may be in a invalid state 
}

template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const MustBe<T>& mustBe2) {
    if (validate(discrete1, mustBe2.getValue())) {
        return mustBe2;
    } else {
        return None<T>();
    }
}

template <typename T>
constexpr Limit<T> intersection(const Discrete<T>&, const None<T>&) {
	return None<T>();
}

template <typename T>
constexpr Limit<T> intersection(const MustBe<T>& mustBe1, const Any<T>& any2) {
	return intersection(any2, mustBe1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const MustBe<T>& mustBe1, const Range<T>& range2) {
	return intersection(range2, mustBe1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const MustBe<T>& mustBe1, const DiscreteRange<T>& discreteRange2) {
	return intersection(discreteRange2, mustBe1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const MustBe<T>& mustBe1, const Discrete<T>& discrete2) {
	return intersection(discrete2, mustBe1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const MustBe<T>& mustBe1, const MustBe<T>& mustBe2) {
	if (validate(mustBe1, mustBe2.getValue())) {
        return mustBe2;
    } else {
        return None<T>();
    }
}

template <typename T>
constexpr Limit<T> intersection(const MustBe<T>&, const None<T>&) {
	return None<T>();
}

template <typename T>
constexpr Limit<T> intersection(const None<T>& none1, const Any<T>& any2) {
	return intersection(any2, none1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const None<T>& none1, const Range<T>& range2) {
	return intersection(range2, none1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const None<T>& none1, const DiscreteRange<T>& discreteRange2) {
	return intersection(discreteRange2, none1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const None<T>& none1, const Discrete<T>& discrete2) {
	return intersection(discrete2, none1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const None<T>& none1, const MustBe<T>& mustBe2) {
	return intersection(mustBe2, none1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const None<T>&, const None<T>&) {
	return None<T>();
}

template <typename T>
constexpr Limit<T> intersection(const Limit<T>& limit1, const Limit<T>& limit2) {
    return std::visit(
        [] (const auto& l1, const auto& l2) -> Limit<T> {
            return intersection(l1, l2);
        },
        limit1, limit2
    );
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
	return std::visit(
		[&os] (const auto& l) -> std::ostream& {
			return Zuazo::operator<<(
				os, 
				std::forward_as_tuple(
					std::make_pair("type", std::decay<decltype(l)>::type::LIMIT_TYPE),
					std::make_pair("data", std::cref(l))
				)
			);
		},
		limit
	);
}

}
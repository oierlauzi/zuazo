#include "Limit.h"

#include "../Math/Functions.h"
#include "../Exception.h"

#include <algorithm>

namespace Zuazo::Utils {

/*
 * Any
 */

template <typename T>
constexpr int Any<T>::operator==(const Any&) const {
	return std::tie() == std::tie();
}

template <typename T>
constexpr int Any<T>::operator!=(const Any&) const {
	return std::tie() != std::tie();
}

template <typename T>
constexpr int Any<T>::operator<(const Any&) const {
	return std::tie() < std::tie();
}   

template <typename T>
constexpr int Any<T>::operator<=(const Any&) const {
	return std::tie() <= std::tie();
}

template <typename T>
constexpr int Any<T>::operator>(const Any&) const {
	return std::tie() > std::tie();
}

template <typename T>
constexpr int Any<T>::operator>=(const Any&) const {
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
constexpr int Range<T>::operator==(const Range& other) const {
	return std::tie(m_min, m_max) == std::tie(other.m_min, other.m_max);
}

template <typename T>
constexpr int Range<T>::operator!=(const Range& other) const {
	return std::tie(m_min, m_max) != std::tie(other.m_min, other.m_max);
}

template <typename T>
constexpr int Range<T>::operator<(const Range& other) const {
	return std::tie(m_min, m_max) < std::tie(other.m_min, other.m_max);
}   

template <typename T>
constexpr int Range<T>::operator<=(const Range& other) const {
	return std::tie(m_min, m_max) <= std::tie(other.m_min, other.m_max);
}

template <typename T>
constexpr int Range<T>::operator>(const Range& other) const {
	return std::tie(m_min, m_max) > std::tie(other.m_min, other.m_max);
}

template <typename T>
constexpr int Range<T>::operator>=(const Range& other) const {
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
 * Discrete
 */

template <typename T>
constexpr int Discrete<T>::operator==(const Discrete& other) const {
	return std::equal(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend()
	);
}

template <typename T>
constexpr int Discrete<T>::operator!=(const Discrete& other) const {
	return !operator==(other);
}

template <typename T>
constexpr int Discrete<T>::operator<(const Discrete& other) const {
	return std::lexicographical_compare(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend(),
		std::less<T>()
	);
}   

template <typename T>
constexpr int Discrete<T>::operator<=(const Discrete& other) const {
	return std::lexicographical_compare(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend(),
		std::less_equal<T>()
	);
}

template <typename T>
constexpr int Discrete<T>::operator>(const Discrete& other) const {
	return std::lexicographical_compare(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend(),
		std::greater<T>()
	);
}

template <typename T>
constexpr int Discrete<T>::operator>=(const Discrete& other) const {
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
constexpr int MustBe<T>::operator==(const MustBe& other) const {
	return m_value == other.m_value;
}

template <typename T>
constexpr int MustBe<T>::operator!=(const MustBe& other) const {
	return m_value != other.m_value;
}

template <typename T>
constexpr int MustBe<T>::operator<(const MustBe& other) const {
	return m_value < other.m_value;
}   

template <typename T>
constexpr int MustBe<T>::operator<=(const MustBe& other) const {
	return m_value <= other.m_value;
}

template <typename T>
constexpr int MustBe<T>::operator>(const MustBe& other) const {
	return m_value > other.m_value;
}

template <typename T>
constexpr int MustBe<T>::operator>=(const MustBe& other) const {
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
constexpr int None<T>::operator==(const None&) const {
	return std::tie() == std::tie();
}

template <typename T>
constexpr int None<T>::operator!=(const None&) const {
	return std::tie() != std::tie();
}

template <typename T>
constexpr int None<T>::operator<(const None&) const {
	return std::tie() < std::tie();
}   

template <typename T>
constexpr int None<T>::operator<=(const None&) const {
	return std::tie() <= std::tie();
}

template <typename T>
constexpr int None<T>::operator>(const None&) const {
	return std::tie() > std::tie();
}

template <typename T>
constexpr int None<T>::operator>=(const None&) const {
	return std::tie() >= std::tie();
}



/*
 * Functions
 */


template <typename T>
constexpr T lowest(const Any<T>&) {
	static_assert("lowest function must be spetialized for each type");
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
	return range.getMin();
}

template <typename T>
constexpr T lowest(const Discrete<T>& discrete) {
    assert(std::is_sorted(discrete.cbegin(), discrete.cend()));
    return discrete.front();
}

template <typename T>
constexpr T lowest(const MustBe<T>& mustBe) {
    return mustBe.getValue();
}

template <typename T>
constexpr T lowest(const None<T>&) {
	throw Exception("No value");
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


template <typename T>
constexpr T highest(const Any<T>&) {
    static_assert("highest function must be spetialized for each type");
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
    return range.getMax();
}

template <typename T>
constexpr T highest(const Discrete<T>& discrete) {
    assert(std::is_sorted(discrete.cbegin(), discrete.cend()));
    return discrete.back();
}

template <typename T>
constexpr T highest(const MustBe<T>& mustBe) {
    return mustBe.getValue();
}

template <typename T>
constexpr T highest(const None<T>&) {
    throw Exception("No value");
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
    if(range.getMin() > range.getMax())			return None<T>();					//Invalid state
    else if(range.getMin() == range.getMax())	return MustBe<T>{ range.getMin() };	//Single value represented
    else										return range;						//All OK
}

template <typename T>
constexpr Limit<T> simplify(const Discrete<T>& discrete) {
    switch(discrete.size()) {
        case 0:									return None<T>();					//Invalid state
        case 1:									return MustBe<T>{discrete[0]};		//Single value represented
        default:								return discrete;					//All OK
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
        [] (const auto& l) {
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
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const Any<T>& any2) {
    return intersection(any2, discrete1); //Already implemented
}

template <typename T>
constexpr Limit<T> intersection(const Discrete<T>& discrete1, const Range<T>& range2) {
    return intersection(range2, discrete1); //Already implemented
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

}

namespace Zuazo {

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Utils::Any<T>&) {
	return os << std::forward_as_tuple();
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Utils::Range<T>& range) {
	return os << std::forward_as_tuple(
		std::make_pair("min", std::cref(range.getMin())),
		std::make_pair("max", std::cref(range.getMax()))
	);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Utils::Discrete<T>& discrete) {
	return toOstream(os, discrete.cbegin(), discrete.cend());
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Utils::MustBe<T>& mustBe) {
	return os << std::forward_as_tuple(
		std::make_pair("value", std::cref(mustBe.getValue()))
	);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Utils::None<T>&) {
	return os << std::forward_as_tuple();
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Utils::Limit<T>& limit) {
	return std::visit(
		[&os] (const auto& l) -> std::ostream& {
			return os << std::forward_as_tuple(
				std::make_pair("type", decltype(l)::LIMIT_TYPE),
				std::make_pair("value", std::cref(l))
			);
		},
		limit
	);
}

}
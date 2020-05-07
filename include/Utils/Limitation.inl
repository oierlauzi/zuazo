#include "Limitation.h"

#include "Functions.h"

#include <algorithm>

namespace Zuazo::Utils {

template <typename T>
constexpr bool Limitation<T>::isValid(const T&, Any) {
	return true;
}

template <typename T>
constexpr bool Limitation<T>::isValid(const T& value, const Range& limit) {
	return isInRange(value, limit.min, limit.max);
}

template <typename T>
constexpr bool Limitation<T>::isValid(const T& value, const Discrete& limit) {
	return std::find(limit.cbegin(), limit.cend(), value) != limit.cend();
}

template <typename T>
constexpr bool Limitation<T>::isValid(const T& value, const MustBe& limit) {
	return limit.value == value;
}

template <typename T>
constexpr bool Limitation<T>::isValid(const T&, None) {
	return false;
}

template <typename T>
constexpr bool Limitation<T>::isValid(const T& value, const Limit& limit) {
	return std::visit(
		std::bind(isValid, std::cref(value), std::placeholders::_1), 
		limit
	);
}



template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::simplify(Any) {
	return Any();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::simplify(const Range& limit) {
	if(limit.min < limit.max) {
		return limit;
	} else if(limit.min == limit.max) {
		return MustBe{limit.min};
	} else { // limit.min > limit.max
		return None();
	}
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::simplify(const Discrete& limit) {
	if(limit.size() > 1) {
		return limit;
	} else if(limit.size() == 1) {
		return MustBe{limit[0]};
	} else { // limit.size() == 0
		return None();
	}
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::simplify(const MustBe& limit) {
	return limit;
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::simplify(None) {
	return None();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::simplify(const Limit& limit) {
	return std::visit(simplify, limit);
}



template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(Any, Any) {
	return Any();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(Any, const Range& b) {
	return b;
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(Any, const Discrete& b) {
	return b;
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(Any, const MustBe& b) {
	return b;
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(Any, None) {
	return None();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const Range& a, Any) {
	return a;
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const Range& a, const Range& b) {
	return simplify(
		Range{
			max(a.min, b.min),
			min(a.max, b.max)
		}
	);
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const Range& a, const Discrete& b) {
	Discrete result;

	for(const auto& val : b) {
		if(isValid(val, a)){
			result.push_back(val);
		}
	}

	return simplify(result);
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const Range& a, const MustBe& b) {
	if(isValid(b.value, a)) {
		return b;
	} else {
		return None();
	}
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const Range&, None) {
	return None();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const Discrete& a, Any) {
	return a;
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const Discrete& a, const Range& b) {
	return intersection(b, a);
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const Discrete& a, const Discrete& b) {
	Discrete result;

	for(const auto& val : b) {
		if(isValid(val, a)){
			result.push_back(val);
		}
	}

	return simplify(result);
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const Discrete& a, const MustBe& b) {
	if(isValid(b.value, a)) {
		return b;
	} else {
		return None();
	}
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const Discrete&, None) {
	return None();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const MustBe& a, Any) {
	return a;
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const MustBe& a, const Range& b) {
	return intersection(b, a);
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const MustBe& a, const Discrete& b) {
	return intersection(b, a);
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const MustBe& a, const MustBe& b) {
	if(a.value == b.value) {
		return b;
	} else {
		return None();
	}
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const MustBe&, None) {
	return None();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(None, Any) {
	return None();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(None, const Range&) {
	return None();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(None, const Discrete&) {
	return None();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(None, const MustBe&) {
	return None();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(None, None) {
	return None();
}

template <typename T>
constexpr typename Limitation<T>::Limit Limitation<T>::intersection(const Limit& a, const Limit& b) {
	return std::visit(
		[] (const auto& la, const auto& lb) -> auto {
			return intersection(la, lb);
		},
		a, b
	);
}



template <typename T>
constexpr T Limitation<T>::worst(Any) {
	return {}; //TODO
}

template <typename T>
constexpr T Limitation<T>::worst(const Range& limit) {
	return limit.min;
}

template <typename T>
constexpr T Limitation<T>::worst(const Discrete& limit) {
	assert(std::is_sorted(limit.cbegin(), limit.cend()));
	return limit.front();
}

template <typename T>
constexpr T Limitation<T>::worst(const MustBe& limit) {
	return limit.value;
}

template <typename T>
constexpr T Limitation<T>::worst(None) {
	assert(false);
}

template <typename T>
constexpr T Limitation<T>::worst(const Limit& limit) {
	return std::visit(Limitation::best, limit);
}



template <typename T>
constexpr T Limitation<T>::best(Any) {
	return {}; //TODO
}

template <typename T>
constexpr T Limitation<T>::best(const Range& limit) {
	return limit.max;
}

template <typename T>
constexpr T Limitation<T>::best(const Discrete& limit) {
	assert(std::is_sorted(limit.cbegin(), limit.cend()));
	return limit.back();
}

template <typename T>
constexpr T Limitation<T>::best(const MustBe& limit) {
	return limit.value;
}

template <typename T>
constexpr T Limitation<T>::best(None) {
	assert(false);
}

template <typename T>
constexpr T Limitation<T>::best(const Limit& limit) {
	return std::visit(Limitation::best, limit);
}



}
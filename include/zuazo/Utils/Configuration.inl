#include "Configuration.h"

#include "../Exception.h"
#include "../StringConversions.h"
#include "../Utils/Functions.h"

#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace Zuazo::Utils {
 
/*
 * Configuration
 */

inline Configuration::Configuration(std::initializer_list<value_type> ilist) 
	: m_data(ilist)
{
}

inline int Configuration::operator==(const Configuration& other) const {
	//Both must have the same size
	if(size() != other.size()) {
		return false;
	}

	for(auto ite1 = cbegin(); ite1 != cend(); ++ite1) {
		//Find an entry with the same name
		const auto ite2 = other.find(ite1->first);

		if(ite2 != other.cend()) {
			return false; //Entry not found
		}

		if(!equals(ite1->second, ite2->second)){
			return false; //Entries missmatch
		}
	}

	//All test passed.
	return true;
}

inline int Configuration::operator!=(const Configuration& other) const {
	return !operator==(other);
}


inline const Configuration::mapped_type&
Configuration::operator[](const std::string& option) const {
	return get(option);
}

inline Configuration::operator bool() const {
	if(size() == 0) return false; //Return false if it is empty

	return std::all_of(
		cbegin(), cend(),
		[] (const value_type& pair) -> bool {
			return static_cast<bool>(pair.second);
		}
	);
}


inline void Configuration::reserve(size_t size) {
	m_data.reserve(size);
}

inline size_t Configuration::size() const {
	return m_data.size();
}

inline Configuration::iterator Configuration::begin() {
	return m_data.begin();
}

inline Configuration::const_iterator Configuration::begin() const {
	return m_data.begin();
}

inline Configuration::const_iterator Configuration::cbegin() const {
	return m_data.begin();
}

inline Configuration::iterator Configuration::end() {
	return m_data.end();
}

inline Configuration::const_iterator Configuration::end() const {
	return m_data.end();
}

inline Configuration::const_iterator Configuration::cend() const {
	return m_data.end();
}

inline Configuration::iterator Configuration::find(const std::string& option) {
	return m_data.find(option);
}

inline Configuration::const_iterator Configuration::find(const std::string& option) const {
	return m_data.find(option);
}



template<typename Str>					
inline void Configuration::set(Str&& option, mapped_type limit) {
	m_data[std::forward<Str>(option)] = std::move(limit);
}

inline const Configuration::mapped_type&
Configuration::get(const std::string& option) const {
	const auto ite = find(option);
	return (ite != cend()) ? ite->second : NONE;
}

template<typename T>
inline Configuration::template LimitPtr<T>
Configuration::getLimit(const std::string& option) const {
	static_assert(std::is_base_of<LimitBase, T>::value, "T must inherit from LimitBase");
	return std::dynamic_pointer_cast<typename LimitPtr<T>::element_type>(get(option));
}

template<typename T>
inline std::optional<T> Configuration::getValue(const std::string& option) const {
	const auto limit = getLimit<TypedLimitBase<T>>(option);
	return static_cast<bool>(limit) ? std::make_optional<T>(limit->lowest()) : std::optional<T>(); //If the returned limit is valid, return its lowest value
}



inline Configuration Configuration::intersection(const Configuration& other) const {
	Configuration result;

	//Create a set with all the key values
	const auto value2key = [] (const value_type& x) -> key_type {
		return x.first;
	};
	std::unordered_set<key_type> options;
	std::transform(
		cbegin(), cend(),
		std::inserter(options, options.end()),
		value2key
	);
	std::transform(
		other.cbegin(), other.cend(),
		std::inserter(options, options.end()),
		value2key
	);

	//Reserve the needed amount of data
	result.reserve(options.size());

	//Intersect all the values
	for(auto ite = options.begin(); ite != options.end(); ++ite) {
		const auto ite1 = find(*ite);
		const auto ite2 = other.find(*ite);

		mapped_type ptr;
		if(ite1 != cend() && ite2 != other.cend()) {
			//Both config-s have this entry
			ptr = intersection(ite1->second, ite2->second);
		} else if(ite1 != cend()) {
			//I have this entry
			ptr = ite1->second;
		} else if (ite2 != other.cend()) {
			//The other has this entry
			ptr = ite2->second;
		} else {
			assert(false); //Unexpected
		}
		result.set(std::move(*ite), std::move(ptr));
	}

	return result;
}

template<typename Ite>			
inline std::pair<Configuration, Ite> 
Configuration::intersection(Ite begin, Ite end) const {
	auto result = std::make_pair(Configuration(), end);
	size_t bestCnt = std::numeric_limits<size_t>::max();
	auto ite = begin;

	while(ite != end && bestCnt > 0) {
		const Configuration intersec = intersection(*ite);
		const size_t cnt = std::count_if(
			intersec.cbegin(), intersec.cend(), 
			[] (const value_type& x) -> bool {
				return !static_cast<bool>(x.second);
			}
		);

		if(cnt < bestCnt) {
			result = std::make_pair(intersec, ite);
			bestCnt = cnt;
		}

		++ite;
	}

	return result;
}

template<typename T, typename... Args>
inline Configuration::template LimitPtr<T> 
Configuration::makeLimit(Args&&... args) {
	static_assert(std::is_base_of<LimitBase, T>::value, "T must inherit from LimitBase");
	return makeShared<T>(std::forward<Args>(args)...);
}


inline bool Configuration::equals(const mapped_type& x, const mapped_type& y) {
	//In order to compare more efficiently, compare the pointers
	//themselves as if they are the same, both objects are guarenteed to be equal
	if(x == y) {
		return true;
	}

	//Compare only if they are not nullptr-s
	if(x && y) {
		return x->equals(*y);
	}

	//One of them is nullptr whilst the other isn't
	return false;
}

inline Configuration::mapped_type Configuration::intersection(const mapped_type& x, const mapped_type& y) {
	return (x && y) ? x->intersection(*y) : nullptr; //Intersection is only valid if both ptr-s are valid
}

/*
 * Configuration::TypedLimitBase
 */

template <typename T>
inline std::unique_ptr<Configuration::LimitBase> 
Configuration::TypedLimitBase<T>::intersection(const LimitBase& other) const {
	const auto* typedOther = dynamic_cast<const TypedLimitBase<T>*>(&other);
	return typedOther ? intersection(*typedOther) : nullptr;
}

/*
 * Configuration::MustBe
 */

template <typename T>
template<typename P>
inline Configuration::MustBe<T>::MustBe(P&& value) 
	: m_value(std::forward<P>(value))
{
}

template <typename T>
inline bool Configuration::MustBe<T>::operator==(const MustBe& other) const {
	return m_value == other.m_value;
}

template <typename T>
inline bool Configuration::MustBe<T>::operator!=(const MustBe& other) const {
	return !operator==(other);
}


template <typename T>
inline bool Configuration::MustBe<T>::equals(const LimitBase& other) const {
	if(typeid(other) == typeid(*this)){
		return static_cast<const MustBe<T>&>(other) == *this;
	} else {
		return false;
	}
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::MustBe<T>::intersection(const TypedLimitBase<T>& other) const {
	return other.intersection(*this);
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::MustBe<T>::intersection(const MustBe<T>& other) const {
	if(other.validate(m_value)) {
		return simplified();
	} else {
		return nullptr;
	}
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::MustBe<T>::intersection(const Discrete<T>& other) const {
	if(other.validate(m_value)) {
		return simplified();
	} else {
		return nullptr;
	}
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::MustBe<T>::intersection(const Range<T>& other) const {
	if(other.validate(m_value)) {
		return simplified();
	} else {
		return nullptr;
	}
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::MustBe<T>::intersection(const Any<T>&) const {
	return simplified();
}

template <typename T>
inline T Configuration::MustBe<T>::lowest() const {
	return m_value;
}

template <typename T>
inline T Configuration::MustBe<T>::highest() const {
	return m_value;
}

template <typename T>
inline bool Configuration::MustBe<T>::validate(const T& v) const {
	return m_value == v;
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>>
Configuration::MustBe<T>::simplified() const 
{
	return std::make_unique<MustBe<T>>(*this);
}

template <typename T>
inline std::ostream& Configuration::MustBe<T>::toOstream(std::ostream& os) const {
	return os << std::forward_as_tuple(
		std::make_pair(LimitBase::VALUE_TYPE_ENTRY, std::type_index(typeid(T))),
		std::make_pair(LimitBase::LIMIT_TYPE_ENTRY, LimitBase::LIMIT_TYPE_MUSTBE),
		std::make_pair(VALUE_ENTRY, std::cref(m_value))
	);
}

 
template <typename T>
template<typename P>
inline void Configuration::MustBe<T>::setValue(P&& value) {
	m_value = std::forward<P>(value);
}

template <typename T>
inline const T& Configuration::MustBe<T>::getValue() const {
	return m_value;
}


/*
 * Configuration::Discrete
 */

template <typename T>
inline bool Configuration::Discrete<T>::operator==(const Discrete& other) const {
	assert(std::is_sorted(std::vector<T>::cbegin(), std::vector<T>::cend()));
	assert(std::is_sorted(other.cbegin(), other.cend()));
	return std::equal(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend()
	);
}

template <typename T>
inline bool Configuration::Discrete<T>::operator!=(const Discrete& other) const {
	return !operator==(other);
}


template <typename T>
inline bool Configuration::Discrete<T>::equals(const LimitBase& other) const {
	if(typeid(other) == typeid(*this)){
		return static_cast<const Discrete<T>&>(other) == *this;
	} else {
		return false;
	}
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Discrete<T>::intersection(const TypedLimitBase<T>& other) const {
	return other.intersection(*this);
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Discrete<T>::intersection(const MustBe<T>& other) const {
	return other.intersection(*this); //Already implmented
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Discrete<T>::intersection(const Discrete<T>& other) const {
	Discrete<T> result;
	result.reserve(Math::min(std::vector<T>::size(), other.size())); //Worst case scenario

	//In order to use set_intersection:
	assert(std::is_sorted(std::vector<T>::cbegin(), std::vector<T>::cend()));
	assert(std::is_sorted(other.cbegin(), other.cend()));

	std::set_intersection(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		other.cbegin(), other.cend(),
		std::back_inserter(result)
	);

	return result.simplified();
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Discrete<T>::intersection(const Range<T>& other) const {
	Discrete<T> result;
	result.reserve(std::vector<T>::size()); //Worst case scenario

	std::copy_if(
		std::vector<T>::cbegin(), std::vector<T>::cend(),
		std::back_inserter(result),
		[&other] (const T& value) -> bool {
			return other.validate(value);
		}
	);

	return result.simplified();
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Discrete<T>::intersection(const Any<T>&) const {
	return simplified();
}

template <typename T>
inline T Configuration::Discrete<T>::lowest() const {
	assert(std::is_sorted(std::vector<T>::cbegin(), std::vector<T>::cend()));
	assert(std::vector<T>::size());
	return std::vector<T>::front();
}

template <typename T>
inline T Configuration::Discrete<T>::highest() const {
	assert(std::is_sorted(std::vector<T>::cbegin(), std::vector<T>::cend()));
	assert(std::vector<T>::size());
	return std::vector<T>::back();
}

template <typename T>
inline bool Configuration::Discrete<T>::validate(const T& v) const {
	//In order to use std::binary_search:
	assert(std::is_sorted(std::vector<T>::cbegin(), std::vector<T>::cend()));
	return std::binary_search(std::vector<T>::cbegin(), std::vector<T>::cend(), v);
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>>
Configuration::Discrete<T>::simplified() const 
{
	switch(std::vector<T>::size()) {
	case 0: return nullptr; //No value
	case 1: return std::make_unique<MustBe<T>>(std::vector<T>::front()); //Single value
	default: return  std::make_unique<Discrete<T>>(*this);
	}
}

template <typename T>
inline std::ostream& Configuration::Discrete<T>::toOstream(std::ostream& os) const {
	return os << std::forward_as_tuple(
		std::make_pair(LimitBase::VALUE_TYPE_ENTRY, std::type_index(typeid(T))),
		std::make_pair(LimitBase::LIMIT_TYPE_ENTRY, LimitBase::LIMIT_TYPE_DISCRETE),
		std::make_pair(VALUES_ENTRY, std::cref(static_cast<const std::vector<T>&>(*this)))
	);
}



/*
 * Configuration::Range
 */

template <typename T>
template<typename P, typename Q>
inline Configuration::Range<T>::Range(P&& min, Q&& max) 
	: m_min(std::forward<P>(min))
	, m_max(std::forward<Q>(max))
{
}

template <typename T>
inline bool Configuration::Range<T>::operator==(const Range& other) const {
	return m_min == other.m_min && m_max == other.m_max;
}

template <typename T>
inline bool Configuration::Range<T>::operator!=(const Range& other) const {
	return !operator==(other);
}

template <typename T>
inline bool Configuration::Range<T>::equals(const LimitBase& other) const {
	if(typeid(other) == typeid(*this)){
		return static_cast<const Range<T>&>(other) == *this;
	} else {
		return false;
	}
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Range<T>::intersection(const TypedLimitBase<T>& other) const {
	return other.intersection(*this);
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Range<T>::intersection(const MustBe<T>& other) const {
	return other.intersection(*this); //Already implmented
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Range<T>::intersection(const Discrete<T>& other) const {
	return other.intersection(*this); //Already implmented
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Range<T>::intersection(const Range<T>& other) const {
	Range<T> result(
		Math::max(m_min, other.m_min),
		Math::min(m_max, other.m_max)
	);

	return result.simplified();
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Range<T>::intersection(const Any<T>&) const {
	return simplified();
}

template <typename T>
inline T Configuration::Range<T>::lowest() const {
	return m_min;
}

template <typename T>
inline T Configuration::Range<T>::highest() const {
	return m_max;
}

template <typename T>
inline bool Configuration::Range<T>::validate(const T& v) const {
	return m_min <= v && v <= m_max;
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>>
Configuration::Range<T>::simplified() const 
{
	if(m_min > m_max) {
		return nullptr; //Invalid arrangement
	} else if(m_min == m_max) {
		return std::make_unique<MustBe<T>>(m_min); //Single value
	} else {
		return std::make_unique<Range<T>>(*this); //Range
	}
}

template <typename T>
inline std::ostream& Configuration::Range<T>::toOstream(std::ostream& os) const {
	return os << std::forward_as_tuple(
		std::make_pair(LimitBase::VALUE_TYPE_ENTRY, std::type_index(typeid(T))),
		std::make_pair(LimitBase::LIMIT_TYPE_ENTRY, LimitBase::LIMIT_TYPE_RANGE),
		std::make_pair(MIN_ENTRY, std::cref(m_min)),
		std::make_pair(MAX_ENTRY, std::cref(m_max))
	);
}



template <typename T>
template<typename P>
inline void Configuration::Range<T>::setMin(P&& value) {
	m_min = std::forward<P>(value);
}

template <typename T>
inline const T& Configuration::Range<T>::getMin() const {
	return m_min;
}

template <typename T>
template<typename P>
inline void Configuration::Range<T>::setMax(P&& value) {
	m_max = std::forward<P>(value);
}

template <typename T>
inline const T& Configuration::Range<T>::getMax() const {
	return m_max;
}


/*
 * Configuration::Any
 */

template <typename T>
inline bool Configuration::Any<T>::operator==(const Any&) const {
	return true;
}

template <typename T>
inline bool Configuration::Any<T>::operator!=(const Any& other) const {
	return !operator==(other);
}

template <typename T>
inline bool Configuration::Any<T>::equals(const LimitBase& other) const {
	if(typeid(other) == typeid(*this)){
		return static_cast<const Any<T>&>(other) == *this;
	} else {
		return false;
	}
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Any<T>::intersection(const TypedLimitBase<T>& other) const {
	return other.intersection(*this);
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Any<T>::intersection(const MustBe<T>& other) const {
	return other.intersection(*this); //Already implmented
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Any<T>::intersection(const Discrete<T>& other) const {
	return other.intersection(*this); //Already implmented
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Any<T>::intersection(const Range<T>& other) const {
	return other.intersection(*this); //Already implmented
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>> 
Configuration::Any<T>::intersection(const Any<T>&) const {
	return simplified();
}

template <typename T>
inline T Configuration::Any<T>::lowest() const {
	return T();
}

template <typename T>
inline T Configuration::Any<T>::highest() const {
	return T();
}

template <typename T>
inline bool Configuration::Any<T>::validate(const T&) const {
	return true;
}

template <typename T>
inline typename std::unique_ptr<Configuration::template TypedLimitBase<T>>
Configuration::Any<T>::simplified() const 
{
	return std::make_unique<Any<T>>();
}

template <typename T>
inline std::ostream& Configuration::Any<T>::toOstream(std::ostream& os) const {
	return os << std::forward_as_tuple(
		std::make_pair(LimitBase::VALUE_TYPE_ENTRY, std::type_index(typeid(T))),
		std::make_pair(LimitBase::LIMIT_TYPE_ENTRY, LimitBase::LIMIT_TYPE_ANY)
	);
}

}

namespace Zuazo {

inline std::ostream& operator<<(std::ostream& os, const Utils::Configuration::LimitBase& limit) {
	return limit.toOstream(os);
}

inline std::ostream& operator<<(std::ostream& os, const Utils::Configuration& conf) {
	os << '{';

	auto ite = conf.cbegin();
	while(ite != conf.cend()) {
		os << make_pair(std::cref(ite->first), std::cref(*(ite->second)));
		if(++ite != conf.cend()) os << ", "; //Add a comma to all elements except the last one
	}

	os << '}';

	return os;
}

}
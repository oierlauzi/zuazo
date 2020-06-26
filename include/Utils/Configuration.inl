#include "Configuration.h"

#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace Zuazo::Utils {
 
/*
 * Configuration
 */

inline Configuration::DispatchTable Configuration::s_dispatcher;

inline int Configuration::operator==(const Configuration& other) const {
	//Both must have the same size
	if(size() != other.size()) {
		return false;
	}

	//Both must have the same elemets
	//FIXME n² complexity.
	for(const auto& option1 : *this) {
		for(const auto& option2 : other) {
			//Both elements must have the same name and same type
			if( option1.second.type() != option2.second.type() ||
				option1.first != option2.first ) 
			{
				return false;
			}

			//Compare the elements themselves
			if(!operatorEQ(option1.second, option2.second)) {
				return false;
			}
		}
	}

	//All test passed.
	return true;
}

inline int Configuration::operator!=(const Configuration& other) const {
	return !operator==(other);
}

 
inline Configuration::operator bool() const {
	for(const auto& x : m_data) {
		if(!operatorBOOL(x.second)) {
			return false; //This element is not valid.
		}
	}

	return true; //All elements were valid
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




template<typename T>
inline void Configuration::set(std::string&& option, T&& value) {
	
	m_data[std::move(option)] = std::forward<T>(value);
	registerDispatch<typename std::remove_reference<T>::type>();
}

template<typename T>
inline void Configuration::set(const std::string& option, T&& value) {
	m_data[option] = std::forward<T>(value);
	registerDispatch<typename std::remove_reference<T>::type>();
}

template<typename T>
inline const T& Configuration::get(const std::string& option) const {
	return std::any_cast<const T&>(m_data.at(option));
}

template<typename T>
inline bool Configuration::get(const std::string& option, T* value) const {
	const auto ite = m_data.find(option);

	if(ite != m_data.cend()) {
		auto* x = std::any_cast<const T*>(&(ite->second)); //Test if the type is correct
		if(x) {
			if(x) *value = *x; //Assign if needed
			return true;
		}
	}

	return false; //No entry with that name
}

inline std::string Configuration::toString() const {
	std::stringstream ss;

	ss << "{";

	auto ite = m_data.cbegin();
	while(ite != m_data.cend()) {
		ss << "\"" << ite->first << "\":\"" << toString(ite->second) << "\"";
		if(++ite != m_data.cend()) ss << ","; //Add a "," to all elements except the last one
	}

	ss << "}";
	return ss.str();
}




template<typename T>
inline void Configuration::registerDispatch() {
	//Setting an any value would lead to a infinite recursive loop on these functions
	static_assert(!std::is_same<T, std::any>::value, "std::any will lead to an infinite loop");

	if(s_dispatcher.find(typeid(T)) == s_dispatcher.cend()) {
		s_dispatcher.emplace(
			typeid(T),
			Dispatch {
				[] (const std::any& a, const std::any& b) -> bool { return std::any_cast<const T&>(a) == std::any_cast<const T&>(b); },	//EQ
				[] (const std::any& a) -> bool { return static_cast<bool>(std::any_cast<const T&>(a)); },								//BOOL
				[] (const std::any& a) -> std::string { return toString(std::any_cast<const T&>(a)); },									//toString
			}
		);
	}
}

inline const Configuration::Dispatch& Configuration::getDispatch(const std::any& option) {
	return s_dispatcher.at(option.type());
}


inline bool Configuration::operatorEQ(const std::any& x, const std::any& y) {
	assert(x.type() == y.type());
	return getDispatch(x).operatorEQ(x, y);
}

inline bool Configuration::operatorBOOL(const std::any& x) {
	return getDispatch(x).operatorBOOL(x);
}

inline std::string Configuration::toString(const std::any& x) {
	return getDispatch(x).toString(x);
}





/*
 * Compatibility
 */

inline Compatibility::DispatchTable Compatibility::s_dispatcher;

inline void Compatibility::reserve(size_t size) {
	m_data.reserve(size);
}

inline size_t Compatibility::size() const {
	return m_data.size();
}

inline Compatibility::iterator Compatibility::begin() {
	return m_data.begin();
}

inline Compatibility::const_iterator Compatibility::begin() const {
	return m_data.begin();
}

inline Compatibility::const_iterator Compatibility::cbegin() const {
	return m_data.begin();
}

inline Compatibility::iterator Compatibility::end() {
	return m_data.end();
}

inline Compatibility::const_iterator Compatibility::end() const {
	return m_data.end();
}

inline Compatibility::const_iterator Compatibility::cend() const {
	return m_data.end();
}


template<typename T>			
inline void Compatibility::set(const std::string& option, Any<T> value) {
	set(option, Limit<T>(value));
}

template<typename T>			
inline void Compatibility::set(std::string&& option, Any<T> value) {
	set(std::move(option), Limit<T>(value));
}

template<typename T>			
inline void Compatibility::set(const std::string& option, Range<T> value) {
	set(option, Limit<T>(value));
}

template<typename T>			
inline void Compatibility::set(std::string&& option, Range<T> value) {
	set(std::move(option), Limit<T>(value));
}

template<typename T>			
inline void Compatibility::set(const std::string& option, Discrete<T> value) {
	set(option, Limit<T>(value));
}

template<typename T>			
inline void Compatibility::set(std::string&& option, Discrete<T> value) {
	set(std::move(option), Limit<T>(value));
}

template<typename T>			
inline void Compatibility::set(const std::string& option, MustBe<T> value) {
	set(option, Limit<T>(value));
}

template<typename T>			
inline void Compatibility::set(std::string&& option, MustBe<T> value) {
	set(std::move(option), Limit<T>(value));
}

template<typename T>			
inline void Compatibility::set(const std::string& option, None<T> value) {
	set(option, Limit<T>(value));
}

template<typename T>			
inline void Compatibility::set(std::string&& option, None<T> value) {
	set(std::move(option), Limit<T>(value));
}

template<typename T>			
inline void Compatibility::set(const std::string& option, Limit<T> value) {
	m_data[option] = std::move(value);
	registerDispatch<T>();
}

template<typename T>			
inline void Compatibility::set(std::string&& option, Limit<T> value) {
	m_data[std::move(option)] = std::move(value);
	registerDispatch<T>();
}

template<typename T>	
inline const Compatibility::Limit<T>& Compatibility::get(const std::string& option) const {
	return std::any_cast<const Limit<T>&>(m_data.at(option));
}

template<typename T>	
inline bool Compatibility::get(const std::string& option, Limit<T>* value) const {
	const auto ite = m_data.find(option);

	if(ite != m_data.cend()) {
		auto* x = std::any_cast<const Limit<T>*>(&(ite->second)); //Test if the type is correct
		if(x) {
			if(x) *value = *x; //Assign if needed
			return true;
		}
	}

	return false; //No entry with that name
}



inline Compatibility::Validation Compatibility::validate(const Configuration& conf) const {
	std::vector<bool> result;
	result.reserve(size());

	//Find each element of the compatibility on the configuration
	//FIXME n² complexity
	for(auto ite1 = cbegin(); ite1 != cend(); ++ite1) {
		//Find each entry in the configutration with the same name
		auto ite2 = std::find_if(
			conf.cbegin(), conf.cend(),
			[&ite1] (const Configuration::const_iterator::value_type& val) {
				return val.first == ite1->first;
			}
		);

		if(ite2 == conf.cend()) {
			result.push_back(false); //Entry not found, treat as invalid configuration
		} else {
			result.push_back(validate(ite2->second, ite1->second)); //Validate
		}


	}

	return Validation { 
		*this, 												
		result,
		static_cast<size_t>(std::count(result.cbegin(), result.cend(), false))
	};
}

inline Configuration Compatibility::lowest() const {
	Configuration result;

	for(const auto& limit : m_data) {
		storeValue(result, limit.first, lowest(limit.second));
	}

	return result;
}

inline Configuration Compatibility::highest() const {
	Configuration result;

	for(const auto& limit : m_data) {
		storeValue(result, limit.first, highest(limit.second));
	}


	return result;
}

inline Compatibility Compatibility::simplify(const Compatibility& comp) {
	Compatibility result;
	result.m_data.reserve(comp.m_data.size());

	for(const auto& limit : comp.m_data) {
		result.m_data[limit.first] = simplify(limit.second);
	}

	return result;
}

inline Compatibility 
Compatibility::intersection(const Compatibility& a, 
							const Compatibility& b )
{
	Compatibility result;

	//Create a set with all the key values
	std::unordered_set<std::string> options;
	std::transform(
		a.cbegin(), a.cend(),
		std::inserter(options, options.end()),
		[] (const auto& pair) -> std::string {
			return pair.first;
		}
	);
	std::transform(
		b.cbegin(), b.cend(),
		std::inserter(options, options.end()),
		[] (const auto& pair) -> std::string {
			return pair.first;
		}
	);

	//The size of the result is known at this point
	result.reserve(options.size());

	//Merge both compatibilietes
	for(const auto& option : options) {
		auto ite1 = std::find_if(
			a.cbegin(), a.cend(),
			[&option] (const auto& pair) -> bool {
				return pair.first == option;
			}
		);
		auto ite2 = std::find_if(
			b.cbegin(), b.cend(),
			[&option] (const auto& pair) -> bool {
				return pair.first == option;
			}
		);

		if(ite1 != a.cend() && ite2 != b.cend()) {
			//Entry found in both of them
			result.m_data[option] = intersection(ite1->second, ite2->second);
		} else if(ite1 != a.cend()) {
			result.m_data[option] = ite1->second;
		} else if(ite2 != b.cend()) {
			result.m_data[option] = ite2->second;
		} else {
			//Must have found at least in one of them
			assert(false);
		}
	}

	return result;
}

inline Compatibility::Validation
Compatibility::validate(Utils::BufferView<const Compatibility> compatibilities,
						const Configuration& config )
{
	assert(compatibilities.size() > 0);

	auto ite = compatibilities.cbegin();
	Validation best = ite->validate(config); //Validate the first one in order to be able to compare
	++ite;

	while(ite != compatibilities.cend()) {
		const auto validation = ite->validate(config);
		++ite;
		
		if(validation.missmatchCount < best.missmatchCount) {
			//This compatibility suits better the requested configuration
			best = validation;
		}
	}
	
	return best;
}



template<typename T>
inline void Compatibility::registerDispatch() {
	if(s_dispatcher.find(typeid(Limit<T>)) == s_dispatcher.cend()) {
		s_dispatcher.emplace(
			typeid(Limit<T>),
			Dispatch {
				typeid(T),																																			//underlayingType
				[] (const std::any& a, const std::any& b) -> bool { return validate(std::any_cast<const T&>(a), std::any_cast<const Limit<T>&>(b)); }, 				//validate
				[] (const std::any& a) -> std::any { return simplify(std::any_cast<const Limit<T>&>(a)); },															//simplify
				[] (const std::any& a, const std::any& b) -> std::any { return intersection(std::any_cast<const Limit<T>&>(a), std::any_cast<const Limit<T>&>(b)); },//intersection
				[] (const std::any& a) -> std::any { return lowest(std::any_cast<const Limit<T>&>(a)); },															//lowest
				[] (const std::any& a) -> std::any { return highest(std::any_cast<const Limit<T>&>(a)); },															//highest
				[] (Configuration& conf, const std::string& option, const std::any& value) { conf.set(option, std::any_cast<const T&>(value)); }
			}
		);
}
}

inline const Compatibility::Dispatch& Compatibility::getDispatch(const std::any& option) {
	return s_dispatcher.at(option.type());
}


inline const std::type_index& Compatibility::underlayingType(const std::any& limit) {
	return getDispatch(limit).underlayingType;
}

inline void Compatibility::storeValue(Configuration& conf, const std::string& option, const std::any& value) {
	auto ite = std::find_if(
		s_dispatcher.cbegin(), s_dispatcher.cend(),
		[&value] (const auto& pair) {
			return pair.second.underlayingType == value.type();
		}
	); 

	assert(ite != s_dispatcher.cend()); //Must have found it
	ite->second.storeValue(conf, option, value);
}




template <typename T>
constexpr bool Compatibility::validate(const T&, Any<T>) {
	return true;
}
 
template <typename T>
constexpr bool Compatibility::validate(const T& value, const Range<T>& limit) {
	return Math::isInRange(value, limit.min, limit.max);
}
 
template <typename T>
constexpr bool Compatibility::validate(const T& value, const Discrete<T>& limit) {
	return std::find(limit.cbegin(), limit.cend(), value) != limit.cend();
}
 
template <typename T>
constexpr bool Compatibility::validate(const T& value, const MustBe<T>& limit) {
	return limit.value == value;
}

template <typename T>
constexpr bool Compatibility::validate(const T&, None<T>) {
	return false;
}

template <typename T>
constexpr bool Compatibility::validate(const T& value, const Limit<T>& limit) {
	return std::visit(
		[&] (const auto& x) -> auto{
			return Compatibility::validate(value, x);
		},
		limit
	);
}

inline bool Compatibility::validate(const std::any& value, const std::any& limit) {
	if(underlayingType(limit) == value.type()) {
		//Ensure that the type matches
		return getDispatch(limit).validate(value, limit);
	} else {
		return false;
	}
}




template <typename T>
constexpr T Compatibility::lowest(Any<T>) {
	return T(); //TODO
}

template <typename T>
constexpr T Compatibility::lowest(const Range<T>& limit) {
	return limit.min;
}

template <typename T>
constexpr T Compatibility::lowest(const Discrete<T>& limit) {
	assert(std::is_sorted(limit.cbegin(), limit.cend()));
	return limit.front();
}

template <typename T>
constexpr T Compatibility::lowest(const MustBe<T>& limit) {
	return limit.value;
}

template <typename T>
constexpr T Compatibility::lowest(None<T>) {
	return T();
}

template <typename T>
constexpr T Compatibility::lowest(const Limit<T>& limit) {
	return std::visit(
		[&] (const auto& x) -> auto{
			return Compatibility::lowest(x);
		},
		limit
	);
}

inline std::any Compatibility::lowest(const std::any& limit) {
	return getDispatch(limit).lowest(limit);
}




template <typename T>
constexpr T Compatibility::highest(Any<T>) {
	return T(); //TODO
}

template <typename T>
constexpr T Compatibility::highest(const Range<T>& limit) {
	return limit.max;
}

template <typename T>
constexpr T Compatibility::highest(const Discrete<T>& limit) {
	assert(std::is_sorted(limit.cbegin(), limit.cend()));
	return limit.back();
}

template <typename T>
constexpr T Compatibility::highest(const MustBe<T>& limit) {
	return limit.value;
}

template <typename T>
constexpr T Compatibility::highest(None<T>) {
	return T();
}

template <typename T>
constexpr T Compatibility::highest(const Limit<T>& limit) {
	return std::visit(
		[&] (const auto& x) -> auto{
			return Compatibility::highest(x);
		},
		limit
	);
}

inline std::any Compatibility::highest(const std::any& limit) {
	return getDispatch(limit).highest(limit);
}




template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::simplify(Any<T>) 
{
	return Any<T>();
}

template <typename T>
constexpr Compatibility::template Limit<T> 
Compatibility::simplify(const Range<T>& limit) 
{
	if(limit.min < limit.max) {
		return limit;
	} else if(limit.min == limit.max) {
		return MustBe<T>{limit.min};
	} else { // limit.min > limit.max
		return None<T>();
	}
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::simplify(const Discrete<T>& limit) 
{
	if(limit.size() > 1) {
		return limit;
	} else if(limit.size() == 1) {
		return MustBe<T>{limit[0]};
	} else { // limit.size() == 0
		return None<T>();
	}
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::simplify(const MustBe<T>& limit) 
{
	return limit;
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::simplify(None<T>) 
{
	return None<T>();
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::simplify(const Limit<T>& limit) 
{
	return std::visit(
		[] (const auto& x) -> auto{
			return Compatibility::simplify(x);
		},
		limit
	);
}

inline std::any Compatibility::simplify(const std::any& limit)  {
	return getDispatch(limit).simplify(limit);
}



template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(Any<T>, Any<T>) 
{
	return Any<T>();
}


template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(Any<T>, const Range<T>& b) 
{
	return b;
}


template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(Any<T>, const Discrete<T>& b) 
{
	return b;
}


template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(Any<T>, const MustBe<T>& b) 
{
	return b;
}


template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(Any<T>, None<T>) 
{
	return None<T>();
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const Range<T>& a, Any<T>)
{
	return a;
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const Range<T>& a, const Range<T>& b) 
{
	return simplify(
		Range<T>{
			Math::max(a.min, b.min),
			Math::min(a.max, b.max)
		}
	);
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const Range<T>& a, const Discrete<T>& b)
{
	Discrete<T> result;

	for(const auto& val : b) {
		if(validate(val, a)){
			result.push_back(val);
		}
	}

	return simplify(result);
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const Range<T>& a, const MustBe<T>& b) 
{
	if(validate(b.value, a)) {
		return b;
	} else {
		return None<T>();
	}
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const Range<T>&, None<T>) 
{
	return None<T>();
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const Discrete<T>& a, Any<T>) 
{
	return a;
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const Discrete<T>& a, const Range<T>& b) 
{
	return intersection(b, a);
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const Discrete<T>& a, const Discrete<T>& b) 
{
	Discrete<T> result;

	for(const auto& val : b) {
		if(validate(val, a)){
			result.push_back(val);
		}
	}

	return simplify(result);
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const Discrete<T>& a, const MustBe<T>& b) 
{
	if(validate(b.value, a)) {
		return b;
	} else {
		return None<T>();
	}
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const Discrete<T>&, None<T>) 
{
	return None<T>();
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const MustBe<T>& a, Any<T>)
{
	return a;
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const MustBe<T>& a, const Range<T>& b) 
{
	return intersection(b, a);
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const MustBe<T>& a, const Discrete<T>& b) 
{
	return intersection(b, a);
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const MustBe<T>& a, const MustBe<T>& b) 
{
	if(a.value == b.value) {
		return b;
	} else {
		return None<T>();
	}
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const MustBe<T>&, None<T>) 
{
	return None<T>();
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(None<T>, Any<T>) 
{
	return None<T>();
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(None<T>, const Range<T>&) 
{
	return None<T>();
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(None<T>, const Discrete<T>&) 
{
	return None<T>();
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(None<T>, const MustBe<T>&) 
{
	return None<T>();
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(None<T>, None<T>) 
{
	return None<T>();
}

template <typename T>
constexpr Compatibility::template Limit<T>
Compatibility::intersection(const Limit<T>& a, const Limit<T>& b) 
{
	return std::visit(
		[&] (const auto& x, const auto& y) -> auto{
			return Compatibility::intersection(x, y);
		},
		a, b
	);
}
 

inline std::any Compatibility::intersection(const std::any& a, const std::any& b) {
	if(a.type() == b.type()) {
		return getDispatch(a).intersection(a, b);
	} else {
		return std::any();
	}
}

}

namespace Zuazo {


inline std::string toString(const Utils::Configuration& conf) {
	return conf.toString();
}

}
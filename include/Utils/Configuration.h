#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <variant>
#include <any>
#include <unordered_map>
#include <functional>
#include <typeindex>

#include "BufferView.h"

namespace Zuazo::Utils {

class Configuration {
public:
	using Data = std::unordered_map<std::string, std::any>;
	using iterator = Data::iterator;
	using const_iterator = Data::const_iterator;

	Configuration() = default;
	Configuration(const Configuration& other) = default;
	Configuration(Configuration&& other) = default;
	~Configuration() = default;

	Configuration&					operator=(const Configuration& other) = default;
	Configuration&					operator=(Configuration&& other) = default;

	int								operator==(const Configuration& other) const;
	int								operator!=(const Configuration& other) const;

	operator bool() const;

	void							reserve(size_t size);

	size_t							size() const;
	iterator						begin();
	const_iterator					begin() const;
	const_iterator					cbegin() const;
	iterator						end();
	const_iterator					end() const;
	const_iterator					cend() const;

	template<typename T>
	void							set(std::string&& option, T&& value);
	template<typename T>
	void							set(const std::string& option, T&& value);
	template<typename T>
	const T&						get(const std::string& option) const;
	template<typename T>
	bool							get(const std::string& option, T* value) const;

	std::string						toString() const;

private:
	Data 							m_data;

	struct Dispatch {
		std::function<bool(const std::any&, const std::any&)>	operatorEQ;
		std::function<bool(const std::any&)>					operatorBOOL;
		std::function<std::string(const std::any&)>				toString;
	};

	using DispatchTable = std::unordered_map<std::type_index, Dispatch>;

	static DispatchTable			s_dispatcher;

	template<typename T>
	static void						registerDispatch();
	static const Dispatch&			getDispatch(const std::any& option);

	static bool 					operatorEQ(const std::any& x, const std::any& y);
	static bool 					operatorBOOL(const std::any& x);
	static std::string 				toString(const std::any& x);
};

class Compatibility {
public:
	using Data = std::unordered_map<std::string, std::any>;
	using iterator = Data::iterator;
	using const_iterator = Data::const_iterator;

	template <typename T>
	struct Any {};
	template <typename T>
	struct Range { T min, max; };
	template <typename T>
	using Discrete = std::vector<T>;
	template <typename T>
	struct MustBe { T value; };
	template <typename T>
	struct None {};

	template <typename T>
	using Limit = std::variant<	Any<T>,
								Range<T>,
								Discrete<T>,
								MustBe<T>,
								None<T> >;


	struct Validation {
		std::reference_wrapper<const Compatibility>	compatibility;
		std::vector<bool> matches;
		size_t missmatchCount;
	};

	void							reserve(size_t size);

	size_t							size() const;
	iterator						begin();
	const_iterator					begin() const;
	const_iterator					cbegin() const;
	iterator						end();
	const_iterator					end() const;
	const_iterator					cend() const;

	template<typename T>			
	void							set(const std::string& option, Any<T> value);
	template<typename T>			
	void							set(std::string&& option, Any<T> value);
	template<typename T>			
	void							set(const std::string& option, Range<T> value);
	template<typename T>			
	void							set(std::string&& option, Range<T> value);
	template<typename T>			
	void							set(const std::string& option, Discrete<T> value);
	template<typename T>			
	void							set(std::string&& option, Discrete<T> value);
	template<typename T>			
	void							set(const std::string& option, MustBe<T> value);
	template<typename T>			
	void							set(std::string&& option, MustBe<T> value);
	template<typename T>			
	void							set(const std::string& option, None<T> value);
	template<typename T>			
	void							set(std::string&& option, None<T> value);
	template<typename T>			
	void							set(const std::string& option, Limit<T> value);
	template<typename T>			
	void							set(std::string&& option, Limit<T> value);

	template<typename T>	
	const Limit<T>&					get(const std::string& option) const;
	template<typename T>	
	bool							get(const std::string& option, Limit<T>* value) const;

	Validation						validate(const Configuration& conf) const;
	Configuration					lowest() const;
	Configuration					highest() const;
	static Compatibility			simplify(const Compatibility& comp);
	static Compatibility			intersection(	const Compatibility& a, 
													const Compatibility& b );

	static Validation 				validate(	Utils::BufferView<	const Compatibility> compatibilities,
																	const Configuration& config );

private:
	Data 							m_data;

	struct Dispatch {
		std::type_index												underlayingType;
		std::function<bool(const std::any&, const std::any&)>		validate;
		std::function<std::any(const std::any&)>					simplify;
		std::function<std::any(const std::any&, const std::any&)>	intersection;
		std::function<std::any(const std::any&)>					lowest;
		std::function<std::any(const std::any&)>					highest;
		std::function<void(Configuration&, const std::string&, const std::any&)> storeValue;
	};

	using DispatchTable = std::unordered_map<std::type_index, Dispatch>;

	static DispatchTable			s_dispatcher;

	template<typename T>
	static void						registerDispatch();
	static const Dispatch&			getDispatch(const std::any& option);


	static const std::type_index&	underlayingType(const std::any& limit);
	static void						storeValue(Configuration& conf, const std::string& option, const std::any& value);

	template <typename T>
	static constexpr bool 			validate(const T& value, Any<T> limit);
	template <typename T>
	static constexpr bool 			validate(const T& value, const Range<T>& limit);
	template <typename T>
	static constexpr bool 			validate(const T& value, const Discrete<T>& limit);
	template <typename T>
	static constexpr bool 			validate(const T& value, const MustBe<T>& limit);
	template <typename T>
	static constexpr bool 			validate(const T& value, None<T> limit);
	template <typename T>
	static constexpr bool 			validate(const T& value, const Limit<T>& limit);
	static bool 					validate(const std::any& value, const std::any& limit);

	template <typename T>
	static constexpr T 				lowest(Any<T> limit);
	template <typename T>
	static constexpr T 				lowest(const Range<T>& limit);
	template <typename T>
	static constexpr T 				lowest(const Discrete<T>& limit);
	template <typename T>
	static constexpr T 				lowest(const MustBe<T>& limit);
	template <typename T>
	static constexpr T 				lowest(None<T> limit);
	template <typename T>
	static constexpr T 				lowest(const Limit<T>& limit);
	static std::any 				lowest(const std::any& limit);


	template <typename T>
	static constexpr T 				highest(Any<T> limit);
	template <typename T>
	static constexpr T 				highest(const Range<T>& limit);
	template <typename T>
	static constexpr T 				highest(const Discrete<T>& limit);
	template <typename T>
	static constexpr T 				highest(const MustBe<T>& limit);
	template <typename T>
	static constexpr T 				highest(None<T> limit);
	template <typename T>
	static constexpr T 				highest(const Limit<T>& limit);
	static std::any					highest(const std::any& limit);


	template <typename T>
	static constexpr Limit<T> 		simplify(Any<T> limit);
	template <typename T>
	static constexpr Limit<T> 		simplify(const Range<T>& limit);
	template <typename T>
	static constexpr Limit<T> 		simplify(const Discrete<T>& limit);
	template <typename T>
	static constexpr Limit<T> 		simplify(const MustBe<T>& limit);
	template <typename T>
	static constexpr Limit<T> 		simplify(None<T> limit);
	template <typename T>
	static constexpr Limit<T> 		simplify(const Limit<T>& limit);
	static std::any					simplify(const std::any& limit);


	template <typename T>
	static constexpr Limit<T> 		intersection(Any<T> a, Any<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(Any<T> a, const Range<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(Any<T> a, const Discrete<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(Any<T> a, const MustBe<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(Any<T> a, None<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Range<T>& a, Any<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Range<T>& a, const Range<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Range<T>& a, const Discrete<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Range<T>& a, const MustBe<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Range<T>& a, None<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Discrete<T>& a, Any<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Discrete<T>& a, const Range<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Discrete<T>& a, const Discrete<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Discrete<T>& a, const MustBe<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Discrete<T>& a, None<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const MustBe<T>& a, Any<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const MustBe<T>& a, const Range<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const MustBe<T>& a, const Discrete<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const MustBe<T>& a, const MustBe<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const MustBe<T>& a, None<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(None<T> a, Any<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(None<T> a, const Range<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(None<T> a, const Discrete<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(None<T> a, const MustBe<T>& b);
	template <typename T>
	static constexpr Limit<T> 		intersection(None<T> a, None<T> b);
	template <typename T>
	static constexpr Limit<T> 		intersection(const Limit<T>& a, const Limit<T>& b);
	static std::any					intersection(const std::any& a, const std::any& b);

};

}

namespace Zuazo {

std::string toString(const Utils::Configuration& conf);

}

#include "Configuration.inl"
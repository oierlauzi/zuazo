#pragma once

#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <ostream>

#include "BufferView.h"

namespace Zuazo::Utils {

class Configuration {
public:
	class LimitBase;

	template <typename T>
	class TypedLimitBase ;

	template <typename T>
	class MustBe;
	template <typename T>
	class Discrete;
	template <typename T>
	class Range;
	template <typename T>
	class Any;

	template <typename T>
	using Pointer = std::shared_ptr<const T>;

	using Data = std::unordered_map<std::string, Pointer<LimitBase>>;
	using iterator = Data::iterator;
	using const_iterator = Data::const_iterator;
	using reference = Data::reference;
	using const_reference = Data::const_reference;
	using key_type = Data::key_type;
	using mapped_type = Data::mapped_type;
	using value_type = Data::value_type;

	template <typename T>
	using Limit = Pointer<TypedLimitBase<T>>;

	static inline const mapped_type NONE = mapped_type();

	Configuration() = default;
	explicit Configuration(std::initializer_list<value_type> ilist);
	Configuration(const Configuration& other) = default;
	Configuration(Configuration&& other) = default;
	virtual ~Configuration() = default;

	Configuration&					operator=(const Configuration& other) = default;
	Configuration&					operator=(Configuration&& other) = default;

	int								operator==(const Configuration& other) const;
	int								operator!=(const Configuration& other) const;

	const mapped_type&				operator[](const std::string& option) const;

	operator bool() const;

	void							reserve(size_t size);

	size_t							size() const;
	iterator						begin();
	const_iterator					begin() const;
	const_iterator					cbegin() const;
	iterator						end();
	const_iterator					end() const;
	const_iterator					cend() const;
	iterator						find(const std::string& option);
	const_iterator					find(const std::string& option) const;

	template<typename Str>					
	void							set(Str&& option, mapped_type limit);
	const mapped_type& 				get(const std::string& option) const;

	template<typename T>			
	Limit<T>						getLimit(const std::string& option) const;
	template<typename T>			
	std::optional<T>				getValue(const std::string& option) const;


	Configuration					intersection(const Configuration& other) const;

	template<typename T, typename... Args>
	static Pointer<T>				makeLimit(Args&&... args);


private:
	Data 							m_data;

	static bool						equals(const mapped_type& x, const mapped_type& y);
	static mapped_type				intersection(const mapped_type& x, const mapped_type& y);

};


class Configuration::LimitBase {
public:
	LimitBase() = default;
	LimitBase(const LimitBase& other) = default;
	virtual ~LimitBase() = default;

	LimitBase& operator=(const LimitBase& other) = default;

	virtual bool equals(const LimitBase& other) const = 0;
	virtual std::unique_ptr<LimitBase> intersection(const LimitBase& other) const = 0;
	virtual std::ostream& toOstream(std::ostream& os) const = 0;

	static constexpr std::string_view VALUE_TYPE_ENTRY = "valueType";
	static constexpr std::string_view LIMIT_TYPE_ENTRY = "limitType";
	static constexpr std::string_view LIMIT_TYPE_MUSTBE = "MUSTBE";
	static constexpr std::string_view LIMIT_TYPE_DISCRETE = "DISCRETE";
	static constexpr std::string_view LIMIT_TYPE_RANGE = "RANGE";
	static constexpr std::string_view LIMIT_TYPE_ANY = "ANY";

};

template <typename T>
class Configuration::TypedLimitBase : public LimitBase {
public:
	TypedLimitBase() = default;
	TypedLimitBase(const TypedLimitBase& other) = default;
	virtual ~TypedLimitBase() = default;

	TypedLimitBase& operator=(const TypedLimitBase& other) = default;

	virtual std::unique_ptr<LimitBase> intersection(const LimitBase& other) const final;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const TypedLimitBase<T>& other) const = 0;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const MustBe<T>& other) const = 0;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Discrete<T>& other) const = 0;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Range<T>& other) const = 0;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Any<T>& other) const = 0;
	virtual T lowest() const = 0;
	virtual T highest() const = 0;
	virtual bool validate(const T& value) const = 0;
	virtual std::unique_ptr<TypedLimitBase<T>> simplified() const = 0;

};

template <typename T>
class Configuration::MustBe final : public TypedLimitBase<T> { 
public:
	template<typename P>
	explicit MustBe(P&& value);
	MustBe() = default;
	MustBe(const MustBe& other) = default;
	MustBe(MustBe&& other) = default;
	virtual ~MustBe() = default;

	MustBe& operator=(const MustBe& other) = default;
	MustBe& operator=(MustBe&& other) = default;

	bool operator==(const MustBe& other) const;
	bool operator!=(const MustBe& other) const;

	virtual bool equals(const LimitBase& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const TypedLimitBase<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const MustBe<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Discrete<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Range<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Any<T>& other) const;
	virtual T lowest() const;
	virtual T highest() const;
	virtual bool validate(const T& value) const;
	virtual std::unique_ptr<TypedLimitBase<T>> simplified() const;
	virtual std::ostream& toOstream(std::ostream& os) const;

	template<typename P>
	void setValue(P&& value);
	const T& getValue() const;

	static constexpr std::string_view VALUE_ENTRY = "value";

private:
	T m_value;
};

template <typename T>
class Configuration::Discrete final 
	: public TypedLimitBase<T>
	, public std::vector<T> 
{ 
public:
	using std::vector<T>::vector;
	using std::vector<T>::operator=;

	bool operator==(const Discrete& other) const;
	bool operator!=(const Discrete& other) const;
	
	virtual bool equals(const LimitBase& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const TypedLimitBase<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const MustBe<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Discrete<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Range<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Any<T>& other) const;
	virtual T lowest() const;
	virtual T highest() const;
	virtual bool validate(const T& value) const;
	virtual std::unique_ptr<TypedLimitBase<T>> simplified() const;
	virtual std::ostream& toOstream(std::ostream& os) const;

	static constexpr std::string_view VALUES_ENTRY = "values";

};

template <typename T>
class Configuration::Range final : public TypedLimitBase<T> { 
public:
	template<typename P, typename Q>
	Range(P&& min, Q&& max);
	Range() = default;
	Range(const Range& other) = default;
	Range(Range&& other) = default;
	virtual ~Range() = default;

	Range& operator=(const Range& other) = default;
	Range& operator=(Range&& other) = default;

	bool operator==(const Range& other) const;
	bool operator!=(const Range& other) const;

	virtual bool equals(const LimitBase& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const TypedLimitBase<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const MustBe<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Discrete<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Range<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Any<T>& other) const;
	virtual T lowest() const;
	virtual T highest() const;
	virtual bool validate(const T& value) const;
	virtual std::unique_ptr<TypedLimitBase<T>> simplified() const;
	virtual std::ostream& toOstream(std::ostream& os) const;

	template<typename P>
	void setMin(P&& value);
	const T& getMin() const;
	
	template<typename P>
	void setMax(P&& value);
	const T& getMax() const;

	static constexpr std::string_view MIN_ENTRY = "min";
	static constexpr std::string_view MAX_ENTRY = "max";

private:
	T m_min, m_max;
};

template <typename T>
class Configuration::Any final : public TypedLimitBase<T> {
public:
	Any() = default;
	Any(const Any& other) = default;
	virtual ~Any() = default;

	Any& operator=(const Any& other) = default;

	bool operator==(const Any& other) const;
	bool operator!=(const Any& other) const;

	virtual bool equals(const LimitBase& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const TypedLimitBase<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const MustBe<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Discrete<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Range<T>& other) const;
	virtual std::unique_ptr<TypedLimitBase<T>> intersection(const Any<T>& other) const;
	virtual T lowest() const;
	virtual T highest() const;
	virtual bool validate(const T& value) const;
	virtual std::unique_ptr<TypedLimitBase<T>> simplified() const;
	virtual std::ostream& toOstream(std::ostream& os) const;

};

}

namespace Zuazo {

std::ostream& operator<<(std::ostream& os, const Utils::Configuration::LimitBase& limit);
std::ostream& operator<<(std::ostream& os, const Utils::Configuration& conf);

}

#include "Configuration.inl"
#pragma once

#include "Vector.h"
#include "../Macros.h"

#include <array>
#include <cstddef>

namespace Zuazo::Math {

template<typename T, size_t Deg>
class Polynomial {
public:
	using value_type = T;

	template<typename... Values>
	constexpr explicit Polynomial(Values&&... values);
	constexpr Polynomial(const Polynomial& other) = default;
	constexpr Polynomial(Polynomial&& other) = default;
	template<typename Q>
	constexpr Polynomial(const Polynomial<Q, Deg>& other);	
	~Polynomial() = default;

	constexpr Polynomial&					operator=(const Polynomial& other) = default;
	constexpr Polynomial&					operator=(Polynomial&& other) = default;
	template<typename Q>
	constexpr Polynomial&					operator=(const Polynomial<Q, Deg>& other);	

	constexpr value_type&					operator[](size_t i) noexcept;
	constexpr const value_type&				operator[](size_t i) const noexcept;

	template<typename Q>
	constexpr value_type 					operator()(Q t) const;

	constexpr value_type&					front() noexcept;
	constexpr const value_type&				front() const noexcept;
	constexpr value_type&					back() noexcept;
	constexpr const value_type&				back() const noexcept;

	constexpr value_type*					begin() noexcept;
	constexpr const value_type*				begin() const noexcept;
	constexpr const value_type*				cbegin() const noexcept;
	constexpr value_type*					end() noexcept;
	constexpr const value_type*				end() const noexcept;
	constexpr const value_type*				cend() const noexcept;

	constexpr value_type*					data() noexcept;
	constexpr const value_type*				data() const noexcept;

	static constexpr size_t					size() noexcept;
	static constexpr size_t					degree() noexcept;

private:
	std::array<value_type, Deg+1>			m_values;

};

enum class PolynomialSolutionCount {
	INFINITE = -1,
	NONE = 0,
};

ZUAZO_ENUM_COMP_OPERATORS(PolynomialSolutionCount)
ZUAZO_ENUM_ADD_OPERATORS(PolynomialSolutionCount)
ZUAZO_ENUM_SUB_OPERATORS(PolynomialSolutionCount)


template<typename T, size_t Deg>
constexpr bool operator==(const Polynomial<T, Deg>& lhs, const Polynomial<T, Deg>& rhs) noexcept;
template<typename T, size_t Deg>
constexpr bool operator!=(const Polynomial<T, Deg>& lhs, const Polynomial<T, Deg>& rhs) noexcept;


template<typename T, size_t Deg>
constexpr Polynomial<T, Deg> operator+(const Polynomial<T, Deg>& b);

template<typename T, size_t Deg>
constexpr Polynomial<typename std::invoke_result<std::negate<void>, T>::type, Deg>
operator-(const Polynomial<T, Deg>& b);


template<typename T, typename Q, size_t Deg>
constexpr Polynomial<typename std::invoke_result<std::plus<void>, T, Q>::type, Deg>
operator+(const Polynomial<T, Deg>& lhs, const Polynomial<Q, Deg>& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Polynomial<typename std::invoke_result<std::minus<void>, T, Q>::type, Deg>
operator-(const Polynomial<T, Deg>& lhs, const Polynomial<Q, Deg>& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Polynomial<typename std::invoke_result<std::multiplies<void>, T, Q>::type, Deg> 
operator*(const Polynomial<T, Deg>& lhs, const Polynomial<Q, Deg>& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Polynomial<typename std::invoke_result<std::multiplies<void>, T, Q>::type, Deg> 
operator*(const Polynomial<T, Deg>& lhs, const Q& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Polynomial<typename std::invoke_result<std::multiplies<void>, T, Q>::type, Deg> 
operator*(const T& lhs, const Polynomial<Q, Deg>& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Polynomial<typename std::invoke_result<std::divides<void>, T, Q>::type, Deg> 
operator/(const Polynomial<T, Deg>& lhs, const Polynomial<Q, Deg>& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Polynomial<typename std::invoke_result<std::divides<void>, T, Q>::type, Deg> 
operator/(const Polynomial<T, Deg>& lhs, const Q& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Polynomial<typename std::invoke_result<std::divides<void>, T, Q>::type, Deg>
operator/(const T& lhs, const Polynomial<Q, Deg>& rhs);


template<typename T, size_t Deg>
constexpr Polynomial<T, Deg>& operator+=(Polynomial<T, Deg>& lhs, const Polynomial<T, Deg>& rhs);
template<typename T, size_t Deg>
constexpr Polynomial<T, Deg>& operator-=(Polynomial<T, Deg>& lhs, const Polynomial<T, Deg>& rhs);
template<typename T, size_t Deg>
constexpr Polynomial<T, Deg>& operator*=(Polynomial<T, Deg>& lhs, const Polynomial<T, Deg>& rhs);
template<typename T, size_t Deg>
constexpr Polynomial<T, Deg>& operator*=(Polynomial<T, Deg>& lhs, const typename Polynomial<T, Deg>::value_type& rhs);
template<typename T, size_t Deg>
constexpr Polynomial<T, Deg>& operator/=(Polynomial<T, Deg>& lhs, const Polynomial<T, Deg>& rhs);
template<typename T, size_t Deg>
constexpr Polynomial<T, Deg>& operator/=(Polynomial<T, Deg>& lhs, const typename Polynomial<T, Deg>::value_type& rhs);



template<typename Func, typename... T, size_t Deg>
constexpr Polynomial<typename std::invoke_result<Func, const typename Polynomial<T, Deg>::value_type&...>::type, Deg> 
transform(Func f, const Polynomial<T, Deg>&... v);


template<typename T, size_t N, size_t Deg>
constexpr void setComponent(Polynomial<Vec<T, N>, Deg>& p, size_t i, const Polynomial<typename Vec<T, N>::value_type, Deg>& c);

template<typename T, size_t N, size_t Deg>
constexpr Polynomial<typename Vec<T, N>::value_type, Deg> getComponent(const Polynomial<Vec<T, N>, Deg>& p, size_t i);

template<typename T, size_t Deg>
constexpr Polynomial<T, Deg-1> derivate(const Polynomial<T, Deg>& p);


template<typename T>
constexpr std::array<T, 0> solve(const Polynomial<T, 0>& poly, PolynomialSolutionCount* cnt = nullptr);

template<typename T>
constexpr std::array<T, 1> solve(const Polynomial<T, 1>& poly, PolynomialSolutionCount* cnt = nullptr);

template<typename T>
constexpr std::array<T, 2> solve(const Polynomial<T, 2>& poly, PolynomialSolutionCount* cnt = nullptr);


template<typename T, size_t N, size_t Deg>
constexpr std::array<Vec<T, N>, Deg> solve(const Polynomial<Vec<T, N>, Deg>& poly, Vec<PolynomialSolutionCount, N>* cnt = nullptr) noexcept;

}

#include "Polynomial.inl"
#pragma once

#include "Polynomial.h"
#include "../Utils/BufferView.h"
#include "../Utils/Limit.h"

#include <array>

namespace Zuazo::Math {

template<typename T, size_t Deg>
class Bezier {
public:
	using value_type = T;

	template<typename... Values>
	constexpr explicit Bezier(Values&&... values);
	constexpr Bezier(const Bezier& other) = default;
	constexpr Bezier(Bezier&& other) = default;
	template<typename Q>
	constexpr Bezier(const Bezier<Q, Deg>& other);	
	~Bezier() = default;

	constexpr Bezier&						operator=(const Bezier& other) = default;
	constexpr Bezier&						operator=(Bezier&& other) = default;
	template<typename Q>
	constexpr Bezier&						operator=(const Bezier<Q, Deg>& other);	

	constexpr value_type&					operator[](size_t i) noexcept;
	constexpr const value_type&				operator[](size_t i) const noexcept;

	template<typename Q>
	constexpr value_type 					operator()(const Q& t) const;

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
	
	void 									reverse() noexcept;

	constexpr Bezier<value_type, 1>			getAxis() const noexcept;
	constexpr value_type					getDelta() const noexcept;

	static constexpr size_t					size() noexcept;
	static constexpr size_t					degree() noexcept;

private:
	std::array<value_type, Deg+1>			m_values;

};

template<typename T>
using LinearBezier = Bezier<T, 1>;

template<typename T>
using QuadraticBezier = Bezier<T, 2>;

template<typename T>
using CubicBezier = Bezier<T, 3>;

template<typename T, size_t N>
using Line = LinearBezier<Vec<T, N>>;


template<typename T, size_t Deg>
constexpr bool operator==(const Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs) noexcept;
template<typename T, size_t Deg>
constexpr bool operator!=(const Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs) noexcept;


template<typename T, size_t Deg>
constexpr Bezier<T, Deg> operator+(const Bezier<T, Deg>& b);

template<typename T, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::negate<void>, T>::type, Deg>
operator-(const Bezier<T, Deg>& b);


template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::plus<void>, T, Q>::type, Deg>
operator+(const Bezier<T, Deg>& lhs, const Bezier<Q, Deg>& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::minus<void>, T, Q>::type, Deg>
operator-(const Bezier<T, Deg>& lhs, const Bezier<Q, Deg>& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::multiplies<void>, T, Q>::type, Deg> 
operator*(const Bezier<T, Deg>& lhs, const Bezier<Q, Deg>& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::multiplies<void>, T, Q>::type, Deg> 
operator*(const Bezier<T, Deg>& lhs, const Q& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::multiplies<void>, T, Q>::type, Deg> 
operator*(const T& lhs, const Bezier<Q, Deg>& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::divides<void>, T, Q>::type, Deg> 
operator/(const Bezier<T, Deg>& lhs, const Bezier<Q, Deg>& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::divides<void>, T, Q>::type, Deg> 
operator/(const Bezier<T, Deg>& lhs, const Q& rhs);

template<typename T, typename Q, size_t Deg>
constexpr Bezier<typename std::invoke_result<std::divides<void>, T, Q>::type, Deg>
operator/(const T& lhs, const Bezier<Q, Deg>& rhs);


template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator+=(Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs);
template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator-=(Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs);
template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator*=(Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs);
template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator*=(Bezier<T, Deg>& lhs, const typename Bezier<T, Deg>::value_type& rhs);
template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator/=(Bezier<T, Deg>& lhs, const Bezier<T, Deg>& rhs);
template<typename T, size_t Deg>
constexpr Bezier<T, Deg>& operator/=(Bezier<T, Deg>& lhs, const typename Bezier<T, Deg>::value_type& rhs);



template<typename Func, typename... T, size_t Deg>
constexpr Bezier<typename std::invoke_result<Func, const typename Bezier<T, Deg>::value_type&...>::type, Deg> 
transform(Func f, const Bezier<T, Deg>&... v);


template<typename T, size_t N, size_t Deg>
constexpr void setComponent(Bezier<Vec<T, N>, Deg>& s, size_t i, const Bezier<typename Vec<T, N>::value_type, Deg>& c);

template<typename T, size_t N, size_t Deg>
constexpr Bezier<typename Vec<T, N>::value_type, Deg> getComponent(const Bezier<Vec<T, N>, Deg>& s, size_t i);


template<typename T, size_t Deg>
constexpr Polynomial<typename Bezier<T, Deg>::value_type, Deg> toPolynomial(const Bezier<T, Deg>& s);

template<typename T, size_t Deg>
constexpr Bezier<T, Deg-1> derivate(const Bezier<T, Deg>& s);

template<typename T, size_t Deg>
constexpr std::array<typename Bezier<T, Deg>::value_type, Deg> solve(	const Bezier<T, Deg>& s, 
																		SolutionCount* cnt = nullptr);

template<typename T, size_t N, size_t Deg>
constexpr std::array<typename Bezier<Vec<T, N>, Deg>::value_type, Deg> solve(	const Bezier<Vec<T, N>, Deg>& s, 
																				Vec<SolutionCount, N>* cnt = nullptr );

template<typename T, size_t Deg, typename Q>
constexpr std::array<Bezier<T, Deg>, 2> split(const Bezier<T, Deg>& bezier, const Q& t) noexcept;


template<typename T, size_t Deg>
constexpr Utils::Range<typename Bezier<T, Deg>::value_type> getBoundaries(const Bezier<T, Deg>& s);

template<typename T, size_t N, size_t Deg>
constexpr Utils::Range<typename Bezier<Vec<T, N>, Deg>::value_type> getBoundaries(const Bezier<Vec<T, N>, Deg>& s);

}

#include "Bezier.inl"
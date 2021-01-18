#pragma once

#include "Vector.h"

#include <array>
#include <cstddef>
#include <limits>

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
	NONE = 0,
	INFINITE = std::numeric_limits<int>::max()
};


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
#pragma once

#include "Vector.h"

#include <array>

namespace Zuazo::Math {

template <typename T, size_t M, size_t N>
class Mat;

template <typename T>
class Mat<T, 2, 2> {
public:
	using column_type = Vec<T, 2>;
	using row_type = Vec<T, 2>;
	using diagonal_type = Vec<T, 2>;
	using value_type = typename column_type::value_type;

	constexpr explicit Mat(value_type diag = value_type(1));
	constexpr Mat(const Mat& other) = default;
	constexpr Mat(Mat&& other) = default;

	constexpr Mat(value_type diag0, value_type diag1);
	constexpr Mat(	value_type m00, value_type m01,
					value_type m10, value_type m11 );
	constexpr Mat(column_type col0, column_type col1);

	template<typename Q>
	constexpr Mat(const Mat<Q, 2, 2>& mat2);

	~Mat() = default;

	constexpr Mat&					operator=(const Mat& other) = default;
	constexpr Mat&					operator=(Mat&& other) = default;

	template<typename Q>
	constexpr Mat&					operator=(const Mat<Q, 2, 2>& mat2);

	constexpr column_type&			operator[](size_t i);
	constexpr const column_type&	operator[](size_t i) const;

	constexpr void					setColumn(size_t i, column_type value);
	constexpr const column_type&	getColumn(size_t i) const;
	constexpr void					setRow(size_t j, row_type value);
	constexpr row_type				getRow(size_t j) const;
	constexpr void					setDiagonal(diagonal_type value);
	constexpr diagonal_type			getDiagonal() const;

	constexpr value_type*			begin();
	constexpr const value_type*		begin() const;
	constexpr const value_type*		cbegin() const;
	constexpr value_type*			end();
	constexpr const value_type*		end() const;
	constexpr const value_type*		cend() const;

	constexpr value_type*			data();
	constexpr const value_type*		data() const;

	static constexpr size_t			size();
	static constexpr size_t			columns();
	static constexpr size_t			rows();

private:
	std::array<column_type, row_type::size()> m_values;

};

template <typename T>
class Mat<T, 3, 3> {
public:
	using column_type = Vec<T, 3>;
	using row_type = Vec<T, 3>;
	using diagonal_type = Vec<T, 3>;
	using value_type = typename column_type::value_type;

	constexpr explicit Mat(value_type diag = value_type(1));
	constexpr Mat(const Mat& other) = default;
	constexpr Mat(Mat&& other) = default;

	constexpr Mat(value_type diag0, value_type diag1, value_type diag2);
	constexpr Mat(	value_type m00, value_type m01, value_type m02,
					value_type m10, value_type m11, value_type m12,
					value_type m20, value_type m21, value_type m22 );
	constexpr Mat(column_type col0, column_type col1, column_type col2);

	template<typename Q>
	constexpr Mat(const Mat<Q, 2, 2>& mat2);
	template<typename Q>
	constexpr Mat(const Mat<Q, 3, 3>& mat3);

	~Mat() = default;

	constexpr Mat&					operator=(const Mat& other) = default;
	constexpr Mat&					operator=(Mat&& other) = default;

	template<typename Q>
	constexpr Mat&					operator=(const Mat<Q, 2, 2>& mat2);
	template<typename Q>
	constexpr Mat&					operator=(const Mat<Q, 3, 3>& mat3);

	constexpr column_type&			operator[](size_t i);
	constexpr const column_type&	operator[](size_t i) const;

	constexpr void					setColumn(size_t i, column_type value);
	constexpr const column_type&	getColumn(size_t i) const;
	constexpr void					setRow(size_t j, row_type value);
	constexpr row_type				getRow(size_t j) const;
	constexpr void					setDiagonal(diagonal_type value);
	constexpr diagonal_type			getDiagonal() const;

	constexpr value_type*			begin();
	constexpr const value_type*		begin() const;
	constexpr const value_type*		cbegin() const;
	constexpr value_type*			end();
	constexpr const value_type*		end() const;
	constexpr const value_type*		cend() const;

	constexpr value_type*			data();
	constexpr const value_type*		data() const;

	static constexpr size_t			size();
	static constexpr size_t			columns();
	static constexpr size_t			rows();
	
private:
	std::array<column_type, row_type::size()> m_values;

};

template <typename T>
class Mat<T, 4, 4> {
public:
	using column_type = Vec<T, 4>;
	using row_type = Vec<T, 4>;
	using diagonal_type = Vec<T, 4>;
	using value_type = typename column_type::value_type;

	constexpr explicit Mat(value_type diag = value_type(1));
	constexpr Mat(const Mat& other) = default;
	constexpr Mat(Mat&& other) = default;

	constexpr Mat(value_type diag0, value_type diag1, value_type diag2, value_type diag3);
	constexpr Mat(	value_type m00, value_type m01, value_type m02, value_type m03,
					value_type m10, value_type m11, value_type m12, value_type m13,
					value_type m20, value_type m21, value_type m22, value_type m23, 
					value_type m30, value_type m31, value_type m32, value_type m33 );
	constexpr Mat(column_type col0, column_type col1, column_type col2, column_type col3);

	template<typename Q>
	constexpr Mat(const Mat<Q, 2, 2>& mat2);
	template<typename Q>
	constexpr Mat(const Mat<Q, 3, 3>& mat3);
	template<typename Q>
	constexpr Mat(const Mat<Q, 4, 4>& mat4);

	~Mat() = default;

	constexpr Mat&					operator=(const Mat& other) = default;
	constexpr Mat&					operator=(Mat&& other) = default;

	template<typename Q>
	constexpr Mat&					operator=(const Mat<Q, 2, 2>& mat2);
	template<typename Q>
	constexpr Mat&					operator=(const Mat<Q, 3, 3>& mat3);
	template<typename Q>
	constexpr Mat&					operator=(const Mat<Q, 4, 4>& mat4);

	constexpr column_type&			operator[](size_t i);
	constexpr const column_type&	operator[](size_t i) const;

	constexpr void					setColumn(size_t i, column_type value);
	constexpr const column_type&	getColumn(size_t i) const;
	constexpr void					setRow(size_t j, row_type value);
	constexpr row_type				getRow(size_t j) const;
	constexpr void					setDiagonal(diagonal_type value);
	constexpr diagonal_type			getDiagonal() const;

	constexpr value_type*			begin();
	constexpr const value_type*		begin() const;
	constexpr const value_type*		cbegin() const;
	constexpr value_type*			end();
	constexpr const value_type*		end() const;
	constexpr const value_type*		cend() const;

	constexpr value_type*			data();
	constexpr const value_type*		data() const;

	static constexpr size_t			size();
	static constexpr size_t			columns();
	static constexpr size_t			rows();

private:
	std::array<column_type, row_type::size()> m_values;

};


template <typename T>
using Mat2x2 = Mat<T, 2, 2>;
template <typename T>
using Mat3x3 = Mat<T, 3, 3>;
template <typename T>
using Mat4x4 = Mat<T, 4, 4>;

typedef Mat2x2<float> Mat2x2f;
typedef Mat3x3<float> Mat3x3f;
typedef Mat4x4<float> Mat4x4f;

typedef Mat2x2<double> Mat2x2d;
typedef Mat3x3<double> Mat3x3d;
typedef Mat4x4<double> Mat4x4d;

typedef Mat2x2<int32_t> Mat2x2i;
typedef Mat3x3<int32_t> Mat3x3i;
typedef Mat4x4<int32_t> Mat4x4i;


template<typename T, size_t M, size_t N>
constexpr bool operator==(const Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept;
template<typename T, size_t M, size_t N>
constexpr bool operator!=(const Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept;

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator+(const Mat<T, M, N>& a);
template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator-(const Mat<T, M, N>& a);

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator+(const Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept;
template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator-(const Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept;
template<typename T, size_t M, size_t N, size_t P>
constexpr Mat<T, M, P> operator*(const Mat<T, M, N>& lhs, const Mat<T, N, P>& rhs) noexcept;
template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator*(const Mat<T, M, N>& lhs, const typename Mat<T, M, N>::value_type& rhs) noexcept;
template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator*(const typename Mat<T, M, N>::value_type& lhs, const Mat<T, M, N>& rhs) noexcept;
template<typename T, size_t M, size_t N>
constexpr typename Mat<T, M, N>::column_type operator*(const Mat<T, M, N>& lhs, const typename Mat<T, M, N>::row_type& rhs) noexcept;
template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator/(const Mat<T, M, N>& lhs, const typename Mat<T, M, N>::value_type& rhs) noexcept;

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N>& operator+=(Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept;
template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N>& operator-=(Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept;
template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N>& operator*=(Mat<T, M, N>& lhs, const typename Mat<T, M, N>::value_type& rhs) noexcept;
template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N>& operator/=(Mat<T, M, N>& lhs, const typename Mat<T, M, N>::value_type& rhs) noexcept;


template<typename T, size_t M, size_t N>
constexpr typename Mat<T, M, N>::column_type transform(const Mat<T, M, N>& m, const typename Mat<T, M, N>::row_type& v);

template<typename T, size_t M, size_t N>
constexpr Mat<T, N, M> transpose(const Mat<T, M, N>& m);


template<typename T>
constexpr const typename Mat<T, 2, 2>::value_type& cofactor(const Mat<T, 2, 2>& m, size_t i, size_t j);

template<typename T, size_t M, size_t N>
constexpr Mat<T, M-1, N-1> cofactor(const Mat<T, M, N>& m, size_t i, size_t j);

template<typename T>
constexpr const T& det(const T& m);
template<typename T>
constexpr typename Mat<T, 2, 2>::value_type det(const Mat<T, 2, 2>& m);
template<typename T>
constexpr typename Mat<T, 3, 3>::value_type det(const Mat<T, 3, 3>& m);
template<typename T, size_t N>
constexpr typename Mat<T, N, N>::value_type det(const Mat<T, N, N>& m);

template<typename T, size_t N>
constexpr Mat<T, N ,N> adj(const Mat<T, N, N>& m);

template<typename T, size_t N>
constexpr Mat<T, N ,N> inv(const Mat<T, N, N>& m);

}



namespace Zuazo {

namespace Utils {

template<typename T, typename H>
struct Hasher;

template <typename T, size_t N, size_t M, typename H>
struct Hasher<Math::Mat<T, N, M>, H> {
	using value_type = Math::Mat<T, N, M>;
	using hash_type = H;

	constexpr hash_type operator()(const value_type& v) const noexcept;
};

}

}

#include "Matrix.inl"
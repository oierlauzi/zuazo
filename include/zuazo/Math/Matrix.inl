#include "Matrix.h"

namespace Zuazo::Math {

/*
 * Mat2x2
 */

template<typename T>
constexpr Mat<T, 2, 2>::Mat(value_type diag)
	: Mat(diag, std::move(diag))
{
}

template<typename T>
constexpr Mat<T, 2, 2>::Mat(value_type diag0, value_type diag1)
	: Mat(	column_type(std::move(diag0), 0), 
			column_type(0, std::move(diag1)))
{
}

template<typename T>
constexpr Mat<T, 2, 2>::Mat(value_type m00, value_type m01,
							value_type m10, value_type m11 )
	: Mat(	column_type(std::move(m00), std::move(m10)), 
			column_type(std::move(m01), std::move(m11)))
{
}


template<typename T>
constexpr Mat<T, 2, 2>::Mat(column_type col0, column_type col1) 
	: m_values{std::move(col0), std::move(col1)}
{
}

template<typename T>
template<typename Q>
constexpr Mat<T, 2, 2>::Mat(const Mat<Q, 2, 2>& mat2)
	: Mat(	column_type(mat2[0]),
			column_type(mat2[1]))
{
}


template <typename T>
template<typename Q>
constexpr Mat<T, 2, 2>& Mat<T, 2, 2>::operator=(const Mat<Q, 2, 2>& mat2) {
	for(size_t i = 0; i < columns(); ++i) {
		(*this)[i] = mat2[i];
	}

	return *this;
}


template<typename T>
constexpr typename Mat<T, 2, 2>::column_type& Mat<T, 2, 2>::operator[](size_t i) {
	assert(i < columns());
	return m_values[i];
}

template<typename T>
constexpr const typename Mat<T, 2, 2>::column_type& Mat<T, 2, 2>::operator[](size_t i) const {
	assert(i < columns());
	return m_values[i];
}


template<typename T>
constexpr void Mat<T, 2, 2>::setColumn(size_t i, column_type value) {
	(*this)[i] = std::move(value);
}

template<typename T>
constexpr const typename Mat<T, 2, 2>::column_type&	Mat<T, 2, 2>::getColumn(size_t i) const {
	return (*this)[i];
}

template<typename T>
constexpr void Mat<T, 2, 2>::setRow(size_t j, row_type value) {
	for(size_t i = 0; i < row_type::size(); ++i) {
		(*this)[i][j] = value[i];
	}
}

template<typename T>
constexpr typename Mat<T, 2, 2>::row_type Mat<T, 2, 2>::getRow(size_t j) const {
	row_type result;

	for(size_t i = 0; i < row_type::size(); ++i) {
		result[i] = (*this)[i][j];
	}

	return result;
}

template<typename T>
constexpr void Mat<T, 2, 2>::setDiagonal(diagonal_type value) {
	for(size_t i = 0; i < diagonal_type::size(); ++i) {
		(*this)[i][i] = value[i];
	}
}

template<typename T>
constexpr typename Mat<T, 2, 2>::diagonal_type Mat<T, 2, 2>::getDiagonal() const {
	diagonal_type result;

	for(size_t i = 0; i < diagonal_type::size(); ++i) {
		result[i] = (*this)[i][i];
	}

	return result;
}


template<typename T>
constexpr typename Mat<T, 2, 2>::value_type* Mat<T, 2, 2>::begin() {
	return data();
}

template<typename T>
constexpr const typename Mat<T, 2, 2>::value_type* Mat<T, 2, 2>::begin() const {
	return data();
}

template<typename T>
constexpr const typename Mat<T, 2, 2>::value_type* Mat<T, 2, 2>::cbegin() const {
	return begin();
}

template<typename T>
constexpr typename Mat<T, 2, 2>::value_type* Mat<T, 2, 2>::end() {
	return data() + size();
}

template<typename T>
constexpr const typename Mat<T, 2, 2>::value_type* Mat<T, 2, 2>::end() const {
	return data() + size();
}

template<typename T>
constexpr const typename Mat<T, 2, 2>::value_type* Mat<T, 2, 2>::cend() const {
	return end();
}

template<typename T>
constexpr typename Mat<T, 2, 2>::value_type* Mat<T, 2, 2>::data() {
	return m_values.front().data();
}

template<typename T>
constexpr const typename Mat<T, 2, 2>::value_type* Mat<T, 2, 2>::data() const {
	return m_values.front().data();
}

template<typename T>
constexpr size_t Mat<T, 2, 2>::size() {
	return columns() * rows();
}

template<typename T>
constexpr size_t Mat<T, 2, 2>::columns() {
	return row_type::size();
}

template<typename T>
constexpr size_t Mat<T, 2, 2>::rows() {
	return column_type::size();
}


/*
 * Mat3x3
 */

template <typename T>
constexpr Mat<T, 3, 3>::Mat(value_type diag)
	: Mat(diag, diag, std::move(diag))
{
}

template <typename T>
constexpr Mat<T, 3, 3>::Mat(value_type diag0, value_type diag1, value_type diag2)
	: Mat(	column_type(std::move(diag0), 0, 0),
			column_type(0, std::move(diag1), 0),
			column_type(0, 0, std::move(diag2)))
{
}

template <typename T>
constexpr Mat<T, 3, 3>::Mat(value_type m00, value_type m01, value_type m02,
							value_type m10, value_type m11, value_type m12,
							value_type m20, value_type m21, value_type m22 )
	: Mat(	column_type(std::move(m00), std::move(m10), std::move(m20)), 
			column_type(std::move(m01), std::move(m11), std::move(m21)), 
			column_type(std::move(m02), std::move(m12), std::move(m22)))
{
}

template <typename T>
constexpr Mat<T, 3, 3>::Mat(column_type col0, column_type col1, column_type col2)
	: m_values{std::move(col0), std::move(col1), std::move(col2)}
{
}

template <typename T>
template<typename Q>
constexpr Mat<T, 3, 3>::Mat(const Mat<Q, 2, 2>& mat2)
	: Mat(	column_type(mat2[0], 0),
			column_type(mat2[1], 0),
			column_type(0,  0,   1))
{
}

template <typename T>
template<typename Q>
constexpr Mat<T, 3, 3>::Mat(const Mat<Q, 3, 3>& mat3)
	: Mat(	column_type(mat3[0]),
			column_type(mat3[1]),
			column_type(mat3[3]))
{
}


template <typename T>
template<typename Q>
constexpr Mat<T, 3, 3>& Mat<T, 3, 3>::operator=(const Mat<Q, 2, 2>& mat2) {
	return (*this) = Mat<Q, 3, 3>(mat2);
}

template <typename T>
template<typename Q>
constexpr Mat<T, 3, 3>& Mat<T, 3, 3>::operator=(const Mat<Q, 3, 3>& mat3) {
	for(size_t i = 0; i < columns(); ++i) {
		(*this)[i] = mat3[i];
	}

	return *this;
}


template<typename T>
constexpr typename Mat<T, 3, 3>::column_type& Mat<T, 3, 3>::operator[](size_t i) {
	assert(i < columns());
	return m_values[i];
}

template<typename T>
constexpr const typename Mat<T, 3, 3>::column_type& Mat<T, 3, 3>::operator[](size_t i) const {
	assert(i < columns());
	return m_values[i];
}


template<typename T>
constexpr void Mat<T, 3, 3>::setColumn(size_t i, column_type value) {
	(*this)[i] = std::move(value);
}

template<typename T>
constexpr const typename Mat<T, 3, 3>::column_type&	Mat<T, 3, 3>::getColumn(size_t i) const {
	return (*this)[i];
}

template<typename T>
constexpr void Mat<T, 3, 3>::setRow(size_t j, row_type value) {
	for(size_t i = 0; i < row_type::size(); ++i) {
		(*this)[i][j] = value[i];
	}
}

template<typename T>
constexpr typename Mat<T, 3, 3>::row_type Mat<T, 3, 3>::getRow(size_t j) const {
	row_type result;

	for(size_t i = 0; i < row_type::size(); ++i) {
		result[i] = (*this)[i][j];
	}

	return result;
}

template<typename T>
constexpr void Mat<T, 3, 3>::setDiagonal(diagonal_type value) {
	for(size_t i = 0; i < diagonal_type::size(); ++i) {
		(*this)[i][i] = value[i];
	}
}

template<typename T>
constexpr typename Mat<T, 3, 3>::diagonal_type Mat<T, 3, 3>::getDiagonal() const {
	diagonal_type result;

	for(size_t i = 0; i < diagonal_type::size(); ++i) {
		result[i] = (*this)[i][i];
	}

	return result;
}


template<typename T>
constexpr typename Mat<T, 3, 3>::value_type* Mat<T, 3, 3>::begin() {
	return data();
}

template<typename T>
constexpr const typename Mat<T, 3, 3>::value_type* Mat<T, 3, 3>::begin() const {
	return data();
}

template<typename T>
constexpr const typename Mat<T, 3, 3>::value_type* Mat<T, 3, 3>::cbegin() const {
	return begin();
}

template<typename T>
constexpr typename Mat<T, 3, 3>::value_type* Mat<T, 3, 3>::end() {
	return data() + size();
}

template<typename T>
constexpr const typename Mat<T, 3, 3>::value_type* Mat<T, 3, 3>::end() const {
	return data() + size();
}

template<typename T>
constexpr const typename Mat<T, 3, 3>::value_type* Mat<T, 3, 3>::cend() const {
	return end();
}

template<typename T>
constexpr typename Mat<T, 3, 3>::value_type* Mat<T, 3, 3>::data() {
	return m_values.front().data();
}

template<typename T>
constexpr const typename Mat<T, 3, 3>::value_type* Mat<T, 3, 3>::data() const {
	return m_values.front().data();
}

template<typename T>
constexpr size_t Mat<T, 3, 3>::size() {
	return columns() * rows();
}

template<typename T>
constexpr size_t Mat<T, 3, 3>::columns() {
	return row_type::size();
}

template<typename T>
constexpr size_t Mat<T, 3, 3>::rows() {
	return column_type::size();
}

/*
 * Mat4x4
 */

template <typename T>
constexpr Mat<T, 4, 4>::Mat(value_type diag)
	: Mat(diag, diag, diag, std::move(diag))
{
}

template <typename T>
constexpr Mat<T, 4, 4>::Mat(value_type diag0, value_type diag1, value_type diag2, value_type diag3)
	: Mat(	column_type(std::move(diag0), 0, 0, 0),
			column_type(0, std::move(diag1), 0, 0),
			column_type(0, 0, std::move(diag2), 0),
			column_type(0, 0, 0, std::move(diag3)))
{
}

template <typename T>
constexpr Mat<T, 4, 4>:: Mat(	value_type m00, value_type m01, value_type m02, value_type m03,
								value_type m10, value_type m11, value_type m12, value_type m13,
								value_type m20, value_type m21, value_type m22, value_type m23, 
								value_type m30, value_type m31, value_type m32, value_type m33 )
	: Mat(	column_type(std::move(m00), std::move(m10), std::move(m20), std::move(m30)), 
			column_type(std::move(m01), std::move(m11), std::move(m21), std::move(m31)), 
			column_type(std::move(m02), std::move(m12), std::move(m22), std::move(m32)),
			column_type(std::move(m03), std::move(m13), std::move(m23), std::move(m33)))
{
}

template <typename T>
constexpr Mat<T, 4, 4>::Mat(column_type col0, column_type col1, column_type col2, column_type col3) 
	: m_values{std::move(col0), std::move(col1), std::move(col2), std::move(col3)}
{
}

template <typename T>
template<typename Q>
constexpr Mat<T, 4, 4>::Mat(const Mat<Q, 2, 2>& mat2)
	: Mat(	column_type(mat2[0], 0,  0),
			column_type(mat2[1], 0,  0),
			column_type(0,  0,   1,  0),
			column_type(0,  0,   0,  1))
{
}

template <typename T>
template<typename Q>
constexpr Mat<T, 4, 4>::Mat(const Mat<Q, 3, 3>& mat3)
	: Mat(	column_type(mat3[0], 0),
			column_type(mat3[1], 0),
			column_type(mat3[2], 0),
			column_type(0, 0, 0, 1))
{
}


template <typename T>
template<typename Q>
constexpr Mat<T, 4, 4>::Mat(const Mat<Q, 4, 4>& mat4)
	: Mat(	column_type(mat4[0]),
			column_type(mat4[1]),
			column_type(mat4[2]),
			column_type(mat4[3]))
{
}


template <typename T>
template<typename Q>
constexpr Mat<T, 4, 4>& Mat<T, 4, 4>::operator=(const Mat<Q, 2, 2>& mat2) {
	return (*this) = Mat<Q, 4, 4>(mat2);
}

template <typename T>
template<typename Q>
constexpr Mat<T, 4, 4>& Mat<T, 4, 4>::operator=(const Mat<Q, 3, 3>& mat3) {
	return (*this) = Mat<Q, 4, 4>(mat3);
}

template <typename T>
template<typename Q>
constexpr Mat<T, 4, 4>& Mat<T, 4, 4>::operator=(const Mat<Q, 4, 4>& mat4) {
	for(size_t i = 0; i < columns(); ++i) {
		(*this)[i] = mat4[i];
	}

	return *this;
}


template<typename T>
constexpr typename Mat<T, 4, 4>::column_type& Mat<T, 4, 4>::operator[](size_t i) {
	assert(i < columns());
	return m_values[i];
}

template<typename T>
constexpr const typename Mat<T, 4, 4>::column_type& Mat<T, 4, 4>::operator[](size_t i) const {
	assert(i < columns());
	return m_values[i];
}


template<typename T>
constexpr void Mat<T, 4, 4>::setColumn(size_t i, column_type value) {
	(*this)[i] = std::move(value);
}

template<typename T>
constexpr const typename Mat<T, 4, 4>::column_type&	Mat<T, 4, 4>::getColumn(size_t i) const {
	return (*this)[i];
}

template<typename T>
constexpr void Mat<T, 4, 4>::setRow(size_t j, row_type value) {
	for(size_t i = 0; i < row_type::size(); ++i) {
		(*this)[i][j] = value[i];
	}
}

template<typename T>
constexpr typename Mat<T, 4, 4>::row_type Mat<T, 4, 4>::getRow(size_t j) const {
	row_type result;

	for(size_t i = 0; i < row_type::size(); ++i) {
		result[i] = (*this)[i][j];
	}

	return result;
}

template<typename T>
constexpr void Mat<T, 4, 4>::setDiagonal(diagonal_type value) {
	for(size_t i = 0; i < diagonal_type::size(); ++i) {
		(*this)[i][i] = value[i];
	}
}

template<typename T>
constexpr typename Mat<T, 4, 4>::diagonal_type Mat<T, 4, 4>::getDiagonal() const {
	diagonal_type result;

	for(size_t i = 0; i < diagonal_type::size(); ++i) {
		result[i] = (*this)[i][i];
	}

	return result;
}


template<typename T>
constexpr typename Mat<T, 4, 4>::value_type* Mat<T, 4, 4>::begin() {
	return data();
}

template<typename T>
constexpr const typename Mat<T, 4, 4>::value_type* Mat<T, 4, 4>::begin() const {
	return data();
}

template<typename T>
constexpr const typename Mat<T, 4, 4>::value_type* Mat<T, 4, 4>::cbegin() const {
	return begin();
}

template<typename T>
constexpr typename Mat<T, 4, 4>::value_type* Mat<T, 4, 4>::end() {
	return data() + size();
}

template<typename T>
constexpr const typename Mat<T, 4, 4>::value_type* Mat<T, 4, 4>::end() const {
	return data() + size();
}

template<typename T>
constexpr const typename Mat<T, 4, 4>::value_type* Mat<T, 4, 4>::cend() const {
	return end();
}

template<typename T>
constexpr typename Mat<T, 4, 4>::value_type* Mat<T, 4, 4>::data() {
	return m_values.data()->data();
}

template<typename T>
constexpr const typename Mat<T, 4, 4>::value_type* Mat<T, 4, 4>::data() const {
	return m_values.data()->data();
}

template<typename T>
constexpr size_t Mat<T, 4, 4>::size() {
	return columns() * rows();
}

template<typename T>
constexpr size_t Mat<T, 4, 4>::columns() {
	return row_type::size();
}

template<typename T>
constexpr size_t Mat<T, 4, 4>::rows() {
	return column_type::size();
}


/*
 * Operators
 */

template<typename T, size_t M, size_t N>
constexpr bool operator==(const Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept {
	bool result = true;

	//Iterate until it becomes false
	for(size_t i = 0; (i < Mat<T, M, N>::columns()) && result; ++i) {
		result = (lhs[i] == rhs[i]);
	}

	return result;
}


template<typename T, size_t M, size_t N>
constexpr bool operator!=(const Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept {
	return !operator==(lhs, rhs);
}


template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator+(const Mat<T, M, N>& a) {
	return a;
}

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator-(const Mat<T, M, N>& a) {
	Mat<T, M, N> result;

	for(size_t i = 0; i < Mat<T, M, N>::columns(); ++i) {
		result[i] = -a[i];
	}

	return result;
}

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator+(const Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept {
	return Mat<T, M, N> (lhs) += rhs;
}

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator-(const Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept {
	return Mat<T, M, N> (lhs) -= rhs;
}

template<typename T, size_t M, size_t N, size_t P>
constexpr Mat<T, M, P> operator*(const Mat<T, M, N>& lhs, const Mat<T, N, P>& rhs) noexcept {
	Mat<T, M, P> result(0);

	for(size_t i = 0; i < Mat<T, M, P>::columns(); ++i) {
		for(size_t j = 0; j < Mat<T, M, P>::rows(); ++j) {
			for(size_t k = 0; k < N; ++k) {
				result[i][j] += lhs[k][j] * rhs[i][k];
			}
		}
	}

	return result;
}

template<typename T, size_t M, size_t N>
constexpr typename Mat<T, M, N>::column_type operator*(const Mat<T, M, N>& lhs, const typename Mat<T, M, N>::row_type& rhs) noexcept {
	return transform(lhs, rhs);
}

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator*(const Mat<T, M, N>& lhs, const typename Mat<T, M, N>::value_type& rhs) noexcept {
	Mat<T, M, N> result;
	
	for(size_t i = 0; i < Mat<T, M, N>::columns(); ++i) {
		result[i] = lhs[i] * rhs;
	}

	return result;
}

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator*(const typename Mat<T, M, N>::value_type& lhs, const Mat<T, M, N>& rhs) noexcept {
	return rhs * lhs; //Defined above
}

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N> operator/(const Mat<T, M, N>& lhs, const typename Mat<T, M, N>::value_type& rhs) noexcept {
	Mat<T, M, N> result;
	
	for(size_t i = 0; i < Mat<T, M, N>::columns(); ++i) {
		result[i] = lhs[i] / rhs;
	}

	return result;
}

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N>& operator+=(Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept {
	for(size_t i = 0; i < Mat<T, M, N>::columns(); ++i) {
		lhs[i] += rhs[i];
	}

	return lhs;
}

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N>& operator-=(Mat<T, M, N>& lhs, const Mat<T, M, N>& rhs) noexcept {
	for(size_t i = 0; i < Mat<T, M, N>::columns(); ++i) {
		lhs[i] -= rhs[i];
	}

	return lhs;
}

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N>& operator*=(Mat<T, M, N>& lhs, const typename Mat<T, M, N>::value_type& rhs) noexcept {
	for(size_t i = 0; i < Mat<T, M, N>::columns(); ++i) {
		lhs[i] *= rhs;
	}

	return lhs;
}

template<typename T, size_t M, size_t N>
constexpr Mat<T, M, N>& operator/=(Mat<T, M, N>& lhs, const typename Mat<T, M, N>::value_type& rhs) noexcept {
	for(size_t i = 0; i < Mat<T, M, N>::columns(); ++i) {
		lhs[i] /= rhs;
	}

	return lhs;
}


/*
 * Matrix operations
 */

template<typename T, size_t M, size_t N>
constexpr typename Mat<T, M, N>::column_type transform(const Mat<T, M, N>& m, const typename Mat<T, M, N>::row_type& v) {
	typename Mat<T, M, N>::column_type result;
	
	for(size_t i = 0; i < result.size(); ++i) {
		result[i] = dot(m.getRow(i), v);
	}

	return result;
}


template<typename T, size_t M, size_t N>
constexpr Mat<T, N, M> transpose(const Mat<T, M, N>& m) {
	Mat<T, N, M> result;
	static_assert(m.rows() == result.columns());
	static_assert(m.columns() == result.rows());

	for(size_t i = 0; i < result.columns(); ++i) {
		for(size_t j = 0; j < result.rows(); ++j) {
			result[i][j] = m[j][i];
		}
	}

	return result;
}

template<typename T, size_t M, size_t N>
constexpr Mat<T, M-1, N-1> cofactor(const Mat<T, M ,N>& m, size_t i, size_t j) {
	Mat<T, M-1, N-1> result(0);

	for(size_t col = 0; col < result.columns(); ++col) {
		for(size_t row = 0; row < result.rows(); ++row) {
			result[col][row] = m[(col < i) ? col : col+1][(row < j) ? row : row+1];
		}
	}

	return result;
}

template<typename T>
constexpr typename Mat<T, 2 ,2>::value_type det(const Mat<T, 2, 2>& m) {
	//Cramer's rule for 2x2 matrices
	return m[0][0]*m[1][1] - m[0][1]*m[1][0];
}


template<typename T, size_t N>
constexpr typename Mat<T, N ,N>::value_type det(const Mat<T, N ,N>& m) {
	//General solution for calculating determinants
	typename Mat<T, N, N>::value_type result(0);

	for(size_t i = 0; i < m.rows(); ++i) {
		//Obtain the cofactor of m[0][i]
		const auto minor = cofactor(m, 0, i);
		const auto sign = (i % 2) ? +1 : -1; //1 for even, -1 for odd

		//Add the result with the apropiate sign
		result += sign * m[0][i] * det(minor);
	}

	return result;
}

template<typename T, size_t N>
constexpr Mat<T, N ,N> adj(const Mat<T, N, N>& m) {
	Mat<T, N ,N> result(0);

	for(size_t i = 0; i < result.columns(); ++i) {
		for(size_t j = 0; j < result.rows(); ++j) {
			//Obtain the cofactor of m[0][i]
			const auto minor = cofactor(m, i, j);
			const auto sign = ((i+j) % 2) ? +1 : -1; //1 for even, -1 for odd

            //Write the values transposing them
			result[j][i] = sign * det(minor);
		}
	}

	return result;
}

template<typename T, size_t N>
constexpr Mat<T, N, N> inv(const Mat<T, N, N>& m) {
	return adj(m) / det(m);
}



template<typename T, size_t M, size_t N>
inline std::ostream& operator<<(std::ostream& os, const Mat<T, M, N>& m) {
	os << "[";

	for(size_t i = 0; i < m.rows(); ++i) {
		if(i > 0) os << ", ";
		os << m.getRow(i);
	}

	return os << "]";
}

}
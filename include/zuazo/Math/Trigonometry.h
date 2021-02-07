#pragma once

namespace Zuazo::Math {

template<typename T, size_t N>
class Vec;


template <typename T>
constexpr T rad2deg(const T& radians) noexcept;

template <typename T>
constexpr T deg2rad(const T& degrees) noexcept;


float cos(float x) noexcept;
double cos(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> cos(const Vec<T, N>& v);


float cosh(float x) noexcept;
double cosh(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> cosh(const Vec<T, N>& v);


float sin(float x) noexcept;
double sin(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> sin(const Vec<T, N>& v);


float sinh(float x) noexcept;
double sinh(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> sinh(const Vec<T, N>& v);


float tan(float x) noexcept;
double tan(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> tan(const Vec<T, N>& v);


float tanh(float x) noexcept;
double tanh(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> tanh(const Vec<T, N>& v);


float acos(float x) noexcept;
double acos(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> acos(const Vec<T, N>& v);


float acosh(float x) noexcept;
double acosh(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> acosh(const Vec<T, N>& v);


float asin(float x) noexcept;
double asin(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> asin(const Vec<T, N>& v);


float asinh(float x) noexcept;
double asinh(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> asinh(const Vec<T, N>& v);


float atan(float x) noexcept;
float atan(float y, float x) noexcept;
double atan(double x) noexcept;
double atan(double y, double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> atan(const Vec<T, N>& v);

template<typename T, size_t N>
constexpr Vec<T, N> atan(const Vec<T, N>& vy, const Vec<T, N>& vx);


float atanh(float x) noexcept;
double atanh(double x) noexcept;

template<typename T, size_t N>
constexpr Vec<T, N> atanh(const Vec<T, N>& v);

}

#include "Trigonometry.inl"
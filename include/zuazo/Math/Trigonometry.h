#pragma once

namespace Zuazo::Math {

template <typename T>
constexpr T rad2deg(const T& radians) noexcept;

template <typename T>
constexpr T deg2rad(const T& degrees) noexcept;


float cos(float x) noexcept;
double cos(double x) noexcept;

float cosh(float x) noexcept;
double cosh(double x) noexcept;

float sin(float x) noexcept;
double sin(double x) noexcept;

float sinh(float x) noexcept;
double sinh(double x) noexcept;

float tan(float x) noexcept;
double tan(double x) noexcept;

float tanh(float x) noexcept;
double tanh(double x) noexcept;


float acos(float x) noexcept;
double acos(double x) noexcept;

float acosh(float x) noexcept;
double acosh(double x) noexcept;

float asin(float x) noexcept;
double asin(double x) noexcept;

float asinh(float x) noexcept;
double asinh(double x) noexcept;

float atan(float x) noexcept;
double atan(double x) noexcept;

float atan(float y, float x) noexcept;
double atan(double y, double x) noexcept;

float atanh(float x) noexcept;
double atanh(double x) noexcept;

}

#include "Trigonometry.inl"
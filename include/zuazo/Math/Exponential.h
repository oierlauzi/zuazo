#pragma once

namespace Zuazo::Math {

float exp(float x) noexcept;
double exp(double x) noexcept;

float exp2(float x) noexcept;
double exp2(double x) noexcept;

float exp10(float x) noexcept;
double exp10(double x) noexcept;

float log(float x) noexcept;
double log(double x) noexcept;

float log2(float x) noexcept;
double log2(double x) noexcept;

float log10(float x) noexcept;
double log10(double x) noexcept;

float pow(float base, float power) noexcept;
double pow(double base, double power) noexcept;

float sqrt(float x) noexcept;
double sqrt(double x) noexcept;

}

#include "Exponential.inl"
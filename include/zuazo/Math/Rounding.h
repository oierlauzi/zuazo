#pragma once

namespace Zuazo::Math {

float floor(float x) noexcept;
double floor(double x) noexcept;

float trunc(float x) noexcept;
double trunc(double x) noexcept;

float ceil(float x) noexcept;
double ceil(double x) noexcept;

float round(float x) noexcept;
double round(double x) noexcept;

float fract(float x) noexcept;
double fract(double x) noexcept;


}

#include "Rounding.inl"
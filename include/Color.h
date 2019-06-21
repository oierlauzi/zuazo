#pragma once

#include <boost/algorithm/string/erase.hpp>
#include <stddef.h>
#include <sys/types.h>
#include <sstream>
#include <string>

namespace Zuazo {

struct Color{
	float	r;
	float	g;
	float	b;
	float	a;

	constexpr Color();
	constexpr Color(float red, float green, float blue, float alpha=1.0);
	constexpr Color(u_int8_t red, u_int8_t green, u_int8_t blue, u_int8_t alpha=0xff);
	constexpr Color(const Color& color, float alpha);
	constexpr Color(const Color& color)=default;
	constexpr Color(u_int32_t rgba);
	Color(const std::string& str);
	~Color()=default;

	operator std::string();
private:
	static u_int32_t parseColor(std::string str);
};

namespace Colors{
	//										        R G B A
	constexpr const Color Color::RED=		Color(0xff0000ff);
	constexpr const Color Color::GREEN=		Color(0x00ff00ff);
	constexpr const Color Color::BLUE=		Color(0x0000ffff);
	constexpr const Color Color::CYAN=		Color(0x00ffffff);
	constexpr const Color Color::PURPLE=	Color(0xff00ffff);
	constexpr const Color Color::YELLOW=	Color(0xffff00ff);
	constexpr const Color Color::ORANGE=	Color(0xff4500ff);
	constexpr const Color Color::WHITE=		Color(0xffffffff);
	constexpr const Color Color::GRAY=		Color(0x7f7f7fff);
	constexpr const Color Color::BLACK=		Color(0x000000ff);
}

}

#include "Colors.inl"

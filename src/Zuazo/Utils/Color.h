#pragma once

#include <boost/algorithm/string/erase.hpp>
#include <stddef.h>
#include <sys/types.h>
#include <iostream>
#include <sstream>
#include <string>

namespace Zuazo::Utils{

struct Color{
	float	r;
	float	g;
	float	b;
	float	a;

	/*
	 * Constructors
	 */
	Color(){
		r=1.0;
		g=1.0;
		b=1.0;
		a=1.0;
	}

	Color(float red, float green, float blue, float alpha=1.0){
		r=normalize(red);
		g=normalize(green);
		b=normalize(blue);
		a=normalize(alpha);
	}

	Color(u_int8_t red, u_int8_t green, u_int8_t blue, u_int8_t alpha=0xff){
		r=normalize(red);
		g=normalize(green);
		b=normalize(blue);
		a=normalize(alpha);
	}

	Color(const Color& color, float alpha){
		r=color.r;
		g=color.g;
		b=color.b;
		a=alpha;
	}

	Color(const Color& color)=default;

	Color(u_int32_t rgba) : Color(
			(u_int8_t)((rgba >> 8*3) & 0xff),
			(u_int8_t)((rgba >> 8*2) & 0xff),
			(u_int8_t)((rgba >> 8*1) & 0xff),
			(u_int8_t)((rgba >> 8*0) & 0xff)){

	}

	Color(const std::string& str) : Color(parseColor(str)){

	}

	~Color()=default;

	/*
	 * Operator overloads
	 */

	Color operator*(const Color& other){
		return Color(
				r * other.r,
				g * other.g,
				b * other.b,
				a * other.a
		);
	}

	Color operator*(float flt){
		return Color(
				r * flt,
				g * flt,
				b * flt,
				a * flt
		);
	}

	Color& operator*=(const Color& other){
		r=normalize(r * other.r);
		g=normalize(g * other.g);
		b=normalize(b * other.b);
		a=normalize(a * other.a);
		return (*this);
	}

	Color& operator*=(float flt){
		r=normalize(r * flt);
		g=normalize(g * flt);
		b=normalize(b * flt);
		a=normalize(a * flt);
		return (*this);
	}

	Color operator+(const Color& other){
		return Color(
				r + other.r,
				g + other.g,
				b + other.b,
				a + other.a
		);
	}

	Color& operator+=(const Color& other){
		r=normalize(r + other.r);
		g=normalize(g + other.g);
		b=normalize(b + other.b);
		a=normalize(a + other.a);
		return (*this);
	}

	operator std::string(){
		std::stringstream ss;
		ss << "#";
		ss << std::hex << (u_int32_t)(r*0xff);
		ss << std::hex << (u_int32_t)(g*0xff);
		ss << std::hex << (u_int32_t)(b*0xff);
		ss << std::hex << (u_int32_t)(a*0xff);
		return ss.str();
	}

	/*
	 * Default colors
	 */

	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color CYAN;
	static const Color PURPLE;
	static const Color YELLOW;
	static const Color ORANGE;
	static const Color WHITE;
	static const Color GRAY;
	static const Color BLACK;

private:
	static float normalize(float cmp);
	static float normalize(u_int8_t cmp);
	static u_int32_t parseColor(std::string str);
};


inline float Color::normalize(float cmp){
	if(cmp<0)
		cmp=0;
	else if(cmp>1.0)
		cmp=1;
	return cmp;
}

inline float Color::normalize(u_int8_t cmp){
	return (float)cmp/0xff;
}

inline u_int32_t Color::parseColor(std::string str){
	boost::erase_all(str, "#");
	boost::erase_all(str, "0x");

	//Try to parse a number from string
	u_int32_t rgba;
	std::stringstream ss; //A bit nazi
	ss << std::hex << str;
	ss >> rgba;

	size_t length=str.length();
	if(length==8){
		//Contains 8 characters -> Includes alpha
		//Nothing to change
	}else if(length==6){
		//Contains 6 characters -> Does not include alpha
		rgba<<=8; //Shift bits to make room for alpha
		rgba+=0xff; //Add alpha
	}else{
		//Not a valid string
		rgba=0; //Simply set it black and zero opacity
	}

	return rgba;
}



inline const Color Color::RED=		Color(0xff0000ff);
inline const Color Color::GREEN=	Color(0x00ff00ff);
inline const Color Color::BLUE=	Color(0x0000ffff);
inline const Color Color::CYAN=	Color(0x00ffffff);
inline const Color Color::PURPLE=	Color(0xff00ffff);
inline const Color Color::YELLOW=	Color(0xffff00ff);
inline const Color Color::ORANGE=	Color(0xff4500ff);
inline const Color Color::WHITE=	Color(0xffffffff);
inline const Color Color::GRAY=	Color(0x7f7f7fff);
inline const Color Color::BLACK=	Color(0x000000ff);

}
